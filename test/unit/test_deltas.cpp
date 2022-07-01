#include <stdlib.h>
#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include "file_read.h"
#include "delta.h"

using namespace std;

struct summary {
    int min;
    int max;
    float avg;
    float variance;
};

bool compareInterval(struct interval i1, struct interval i2) {
    return (i1.start < i2.start);
}

// TODO fix combo B in basic BTIS algo
int get_combo_B_file(char ** argv, vector<struct interval> * combinationB) {

    string bedAfile = (string) argv[1];
    fstream bedBfileList;

    bedBfileList.open(argv[2], ios::in);

    vector<string> bedBfiles;
    string l;
    if (bedBfileList.is_open()) {
        while (getline(bedBfileList, l)) {
            bedBfiles.push_back(l);
            l = "";
        }
    }


    struct interval *A, *B;
    string genome_file = argv[3];
    GenomeFile * genome = new GenomeFile(genome_file);
    map<string,CHRPOS> offsets;
    BedFile * bedA = new BedFile(bedAfile);
    unsigned int A_size, B_size;
    BedFile * bedB;

    float overall_diff_total = 0;
    float overall_diff_delt_total = 0;
    float n_total = 0;
    for (int i = 0; i < bedBfiles.size(); i++) {

        bedB = new BedFile(bedBfiles[i]);

	    read_and_map_files_to_interval_arrays_skip_vector(genome, 
                                                      &offsets,
                                                      bedA,
                                                      bedB,
                                                      &A,
                                                      &A_size,
                                                      &B,
                                                      &B_size);

        for (int j = 0; j < B_size; j++) {

            combinationB->push_back(B[j]);

        }
    }
    
    return 0;
}

summary * calculate_arr_sum(int * arr, int count) {
    summary * arr_summary = new summary;
    arr_summary->avg = 0;
    arr_summary->max = 0;
    arr_summary->min = 0;
    arr_summary->variance = 0;

    // calculate average
    float sum = 0;
    int tmp_max = 0;
    int tmp_min = INT32_MAX;
    for (int i = 0; i < count; i++) {
        sum += arr[i];

        if (arr[i] > tmp_max) {
            tmp_max = arr[i];
        }
        if (arr[i] < tmp_min) {
            tmp_min = arr[i];
        }
    }

    arr_summary->max = tmp_max;
    arr_summary->min = tmp_min;
    arr_summary->avg = sum / count;

    float var = 0;
    for (int i = 0; i < count; i++) {
        var += pow(arr[i]-arr_summary->avg, 2);
    }
    arr_summary->variance = var / count;

    return arr_summary;
}

int test_deltas(vector<struct interval> * combinationB) {
    
    // Pads combinationB array
    int remainder = combinationB->size() % 4;
    for (int i = 0; i < remainder; i++) {
        struct interval * si = new struct interval;
        si->start = 0;
        si->end = 0;
        si->order = 0;
        combinationB->push_back(*si);
    }

    // Allocates Bstarts, Ends, and b_count for Delta class constructor`
    unsigned int Bc = combinationB->size();
    unsigned int * Bstarts = (unsigned int *)malloc(combinationB->size() * sizeof(int));
    unsigned int * Bends = (unsigned int *)malloc(combinationB->size() * sizeof(int));
    
    // fills Bstarts and B ends
    for (int i = 0; i < combinationB->size(); i++) {
        Bstarts[i] = (*combinationB)[i].start;
        Bends[i] = (*combinationB)[i].end;
    }

    // Sorts Bstarts and Bends
    std::sort(Bstarts, Bstarts+Bc);
    std::sort(Bends, Bends+Bc);

    // Verifies that Bstarts and Bends are sorted
    for (int i = 0; i < Bc -1; i++) {
        if (Bstarts[i] > Bstarts[i+1] || Bends[i] > Bends[i+1]) {
            std::cout << "B not sorted" << endl;
            std::cout << "i: " << i << endl;
            return 0;
        }
    }

    // calculate array summary; array must be sorted
    summary * starts_summary = calculate_arr_sum((int*)Bstarts, Bc);
    summary * ends_summary = calculate_arr_sum((int*)Bends, Bc);



    // TODO write these to files for analysis
    fstream out_file;
    out_file.open("test_delta_stats.txt", ios::out);

    out_file << "Starts Summary:" << endl;
    out_file << "Average: " << starts_summary->avg << endl;
    out_file << "Max: " << starts_summary->max << endl;
    out_file << "Min: " << starts_summary->min << endl;
    out_file << "Variance: " << starts_summary->variance << endl;
    out_file << endl;
    out_file << "Ends Summary:" << endl;
    out_file << "Average: " << ends_summary->avg << endl;
    out_file << "Max: " << ends_summary->max << endl;
    out_file << "Min: " << ends_summary->min << endl;
    out_file << "Variance: " << ends_summary->variance << endl;
    out_file << endl;
    out_file << "=================" << endl;

    int compression_schemes[] = {1, 2, 4};
    for (int i = 0; i < 3; i++) {

        std::cout << "===== Beginning Delta Test: " << "D" << compression_schemes[i] << " =====" << endl;
        out_file << "===== Beginning Delta Test =====" << endl;
        out_file << "D" << compression_schemes[i] << " compression:" << endl;
        out_file << "------------------" << endl;


        Delta * D = new Delta(Bstarts, Bends, Bc, compression_schemes[i]);
        summary * dstarts_summary = calculate_arr_sum((int*)D->delta_starts, Bc);
        summary * dends_summary = calculate_arr_sum((int*)D->delta_ends, Bc);

        out_file << "Delta Starts Summary:" << endl;
        out_file << "Average: " << dstarts_summary->avg << endl;
        out_file << "Max: " << dstarts_summary->max << endl;
        out_file << "Min: " << dstarts_summary->min << endl;
        out_file << "Variance: " << dstarts_summary->variance << endl;
        out_file << endl;
        out_file << "Delta Ends Summary:" << endl;
        out_file << "Average: " << dends_summary->avg << endl;
        out_file << "Max: " << dends_summary->max << endl;
        out_file << "Min: " << dends_summary->min << endl;
        out_file << "Variance: " << dends_summary->variance << endl;
        out_file << endl;
       
        out_file << "Printing delta values:" << endl;
        //TODO make sure b length > 30
        for (int i = 0; i < 30; i++) {
            out_file << "Position " << i << ": " << D->delta_starts[i] << endl;
        }
    
        out_file << "Bprime start: " << D->bprim_starts << endl;;
        out_file << "Bprim end: " <<  D->bprim_ends << endl;

    
        std::cout << "===== Completed delta test =====" << endl;
        out_file << "===== Completed delta test =====" << endl << endl;

    }
    return 0;
}

int main(int argc, char ** argv) {

    vector<struct interval> combinationB;

    get_combo_B_file(argv, &combinationB);
    test_deltas(&combinationB);

/*I
    bedBfileList.open(argv[2], ios::in);

    vector<string> bedBfiles;
    string l;
    if (bedBfileList.is_open()) {
        while (getline(bedBfileList, l)) {
            bedBfiles.push_back(l);
            l = "";
        }
    }

    vector<struct interval> combinationB;

    struct interval *A, *B;
    string genome_file = argv[3];
    GenomeFile * genome = new GenomeFile(genome_file);
    map<string,CHRPOS> offsets;
    BedFile * bedA = new BedFile(bedAfile);
    unsigned int A_size, B_size;
    BedFile * bedB;

    float overall_diff_total = 0;
    float overall_diff_delt_total = 0;
    float n_total = 0;
    for (int i = 0; i < bedBfiles.size(); i++) {

        bedB = new BedFile(bedBfiles[i]);

    	read_and_map_files_to_interval_arrays_skip_vector(genome, 
                                                          &offsets,
                                                          bedA,
                                                          bedB,
                                                          &A,
                                                          &A_size,
                                                          &B,
                                                          &B_size);

        for (int j = 0; j < B_size; j++) {

            combinationB.push_back(B[j]);
        }
        


        
        float diff_delt = 0;
        float diff_delt_total = 0;
        float diff = 0;
        float diff_total = 0;
        float limit = 0;
        if (A_size < B_size) {
            limit = A_size;
        } else {
            limit = B_size;
        }
        n_total += (limit-1);
        for (int i = 0; i < limit - 1; i++) {
            diff_delt = B[i].end - B[i+1].start;
            diff_delt = abs(diff_delt);
            diff_delt_total += diff_delt;
            overall_diff_delt_total += diff_delt;


            diff = B[i].end + B[i].start;
            diff = diff /2;
            diff_total += diff;
            overall_diff_total += diff;
        }
        float avg_diff_delt = diff_delt_total / (limit-1);
        float avg_diff = diff_total / (limit-1);
        cout << "Average Delta: \t\t" << avg_diff_delt << "\t: file "<< bedBfiles[i] << endl;
        cout << "Average Num: \t" << avg_diff << "\t\t: file " << bedBfiles[i] << endl << endl;


        delete bedB;
    }


    sort(combinationB.begin(), combinationB.end(), compareInterval);

    float combo_diff = 0;
    float combo_delta_diff = 0;
    float combo_diff_avg = 0;
    float combo_diff_delt_avg = 0;
    for (int i = 0; i < combinationB.size() -1 ; i++) {
        combo_delta_diff += combinationB[i].end - combinationB[i+1].start;
        combo_diff += (combinationB[i].start + combinationB[i].end) / 2;
    }
    combo_diff_delt_avg = combo_delta_diff / (combinationB.size()-1);
    combo_diff_avg = combo_diff / (combinationB.size()-1);

    cout << "Combo Average Delta: \t" << combo_diff_delt_avg << endl;
    cout << "Combo Average Number:\t" << combo_diff_avg << endl << endl;

    float overall_avg_diff_delt = overall_diff_delt_total / n_total;
    float overall_avg_diff = overall_diff_total / n_total;
    cout << endl << endl;
    cout << "Overall Average Delta:\t" << overall_avg_diff_delt << endl;
    cout << "Overall Average Number:\t" << overall_avg_diff << endl;
*/
    return 0;
}

