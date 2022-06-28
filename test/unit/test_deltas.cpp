#include <stdlib.h>
#include <iostream>
#include <string>
#include <vector>
#include "file_read.h"
#include "delta.h"

using namespace std;

bool compareInterval(struct interval i1, struct interval i2) {
    return (i1.start < i2.start);
}

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

int test_deltas(vector<struct interval> * combinationB) {
    
    int remainder = combinationB->size() % 4;
    for (int i = 0; i < remainder; i++) {
        struct interval * si = new struct interval;
        si->start = 0;
        si->end = 0;
        si->order = 0;
        combinationB->push_back(*si);
    }

    unsigned int Bc = combinationB->size();
    unsigned int * Bstarts = (unsigned int *)malloc(combinationB->size() * sizeof(int));
    unsigned int * Bends = (unsigned int *)malloc(combinationB->size() * sizeof(int));
    
    for (int i = 0; i < combinationB->size(); i++) {
        Bstarts[i] = (*combinationB)[i].start;
        Bends[i] = (*combinationB)[i].end;
    }

    std::sort(Bstarts, Bstarts+Bc);
    std::sort(Bends, Bends+Bc);

    for (int i = 0; i < Bc -1; i++) {
        if (Bstarts[i] > Bstarts[i+1] || Bends[i] > Bends[i+1]) {
            std::cout << "B not sorted" << endl;
            std::cout << "i: " << i << endl;
            return 0;
        }
    }


    std::cout << "===== Beginning Delta Test =====" << endl;

    
    

    std::cout << "creating Delta object:" << endl;
    Delta * D = new Delta(Bstarts, Bends, Bc);

    std::cout << "Printing delta values:" << endl;
    for (int i = 0; i < 4; i++) {
        std::cout << "Position " << i << ": " << D->delta_starts[i] << endl;
    }
    std::cout << endl;

    std::cout << "Bprime start: " << D->bprim_starts << endl;;
    std::cout << "Bprim end: " <<  D->bprim_ends << endl;

    std::cout << "===== Completed delta test =====" << endl;
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

