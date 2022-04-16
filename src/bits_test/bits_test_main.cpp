/*****************************************************************************
bits_test_intersections_main.cpp
(c) 2012 - Ryan M. Layer
Hall Laboratory
Quinlan Laboratory
Department of Computer Science
Department of Biochemistry and Molecular Genetics
Department of Public Health Sciences and Center for Public Health Genomics,
University of Virginia
rl6sf@virginia.edu

Licenced under the GNU General Public License 2.0 license.
******************************************************************************/
#include "interval.h"
#include "version.h"
#include "bits_test.h"
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <stdlib.h>

using namespace std;

// define our program name
#define PROGRAM_NAME "bits_test"


// define our parameter checking macro
#define PARAMETER_CHECK(param, paramLen, actualLen) (strncmp(argv[i], param, min(actualLen, paramLen))== 0) && (actualLen == paramLen)

// function declarations
void ShowHelp(void);

int main(int argc, char* argv[]) {

    // our configuration variables
    bool showHelp = false;

    // input files
    string bedAFile;
    vector<string> bedBFiles;
    string genomeFile;

	unsigned int N;

    bool haveBedA = false;
    bool haveBedB = false;
    bool haveGenome = false;
    bool haveN = false;
    
    // check to see if we should print out some help
    if(argc <= 1) showHelp = true;

    for(int i = 1; i < argc; i++) {
        int parameterLength = (int)strlen(argv[i]);

        if((PARAMETER_CHECK("-h", 2, parameterLength)) ||
        (PARAMETER_CHECK("--help", 5, parameterLength))) {
            showHelp = true;
        }
    }

    if(showHelp) ShowHelp();

    // do some parsing (all of these parameters require 2 strings)
    int i;
    for(i = 1; i < argc; i++) {

        int parameterLength = (int)strlen(argv[i]);

        if(PARAMETER_CHECK("-a", 2, parameterLength)) {
            if ((i+1) < argc) {
                haveBedA = true;
                bedAFile = argv[i + 1];
                i++;
            }
        }
        else if(PARAMETER_CHECK("-b", 2, parameterLength)) {
            if ((i+1) < argc) {
                haveBedB = true;
                int j = 0;
                for (j = i+1; j < argc; j++) {
                    if (argv[j][0] != '-') {
                        bedBFiles.push_back(argv[j]);
                    } else {
                        break;
                    }
                }
                i = j-1;
            }
        }
        else if(PARAMETER_CHECK("-g", 2, parameterLength)) {
            if ((i+1) < argc) {
                haveGenome = true;
                genomeFile = argv[i + 1];
                i++;
            }
        }
        else if(PARAMETER_CHECK("-n", 2, parameterLength)) {
            if ((i+1) < argc) {
                haveN = true;
                N = atoi(argv[i + 1]);
                i++;
            }
        }
        else {
            cerr << endl << "*****ERROR: Unrecognized parameter: " << 
				argv[i] << " *****" << endl << endl;
            showHelp = true;
        }
    }

    // make sure we have both input files
    if (!haveBedA || !haveBedB || !haveGenome || !haveN) {
        cerr << endl << "*****" << endl << 
			"*****ERROR: Need -a -b -g and -n. " << endl << "*****" << endl;
        showHelp = true;
    }

    if (!showHelp) {

        for (size_t i = 0; i < bedBFiles.size(); i++) {
            cout << endl;
            cout << bedAFile << "|||| against ||||" << bedBFiles[i] << endl;
            BitsTest *bc = new BitsTest(bedAFile, bedBFiles[i], genomeFile, N);
            cout << endl;
            delete bc;
        }
        return 0;
    }
    else {
        ShowHelp();
    }
}

void ShowHelp(void) {

    cerr << endl << "Program: " << PROGRAM_NAME << " (v" << VERSION << ")" << endl;

    cerr << "Author: Kieran Zylstra (kieran.zylstra@colorado.edu)" << endl;

    cerr << "Summary: Report overlaps between two feature files." << endl << endl;

    cerr << "Usage: " << PROGRAM_NAME << " [OPTIONS] -a <bed/gff/vcf> -b <bed/gff/vcf> -g <bed/gff/vcf>" << endl << endl;

    cerr << "Options: " << endl;

    cerr << "\t-a\t" << "The A input file." << endl << endl;
    cerr << "\t-b\t" << "The B input files seperated by spaces." << endl << endl;
    cerr << "\t-n\t" << "The number of iterations." << endl << endl;
    cerr << "\t-g\t" << "The genome/universe input file." << endl << endl;

    // end the program here
    exit(1);

}

