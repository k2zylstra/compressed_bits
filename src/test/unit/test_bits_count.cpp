#include <gtest/gtest.h>
#include "interval.h"
#include "bedFile.h"
#include "genomeFile.h"
#include "file_read.h"
#include "lineFileUtilities.h"

#define BEDAFILE "HSMMtube_Skeletal_Muscle_Myotubes_Derived_from_HSMM_Active_TSS.bed"
#define BEDBFILE "HSMMtube_Skeletal_Muscle_Myotubes_Derived_from_HSMM_Enhancers.bed"
#define GENOMEFILE "human.hg38.genome"
#define N 5

class TestBitsCount : public ::testing::Test {
protected:
    string _bedAFile;
    string _bedBFile;
    string _genomeFile;

    BedFile *_bedA, *_bedB;
    GenomeFile *_genome;
    
    map<string,CHRPOS> _offsets;

    unsigned int _N;

    unsigned int O;
    double mean;
    double sd;
    double p;

    void SetUp() override {
        _bedAFile = BEDAFILE;
        _bedBFile = BEDBFILE;
        _genomeFile = GENOMEFILE;
        _N = 50;

        _bedA = new BedFile(_bedAFile);
        _bedB = new BedFile(_bedBFile);
        _genome = new GenomeFile(_genomeFile);
    }

    void TestOverlaps() {
        struct interval *A, *B;
	    unsigned int A_size, B_size;
        //vector<struct interval> A, B;
	
	    read_and_map_files_to_interval_arrays_skip_vector(_genome,
													    &_offsets,
													    _bedA,
													    _bedB,
													    &A,
													    &A_size,
													    &B,
													    &B_size);


	    CHRPOS max_offset = 0;
	    map<string,CHRPOS>::const_iterator itr;
	    for (itr = _offsets.begin(); itr != _offsets.end(); ++itr) {
                    if (max_offset < itr->second)
		        max_offset = itr->second;
            }

        test_intersections_bsearch_seq(A,
								    A_size,
								    B,
								    B_size,
								    _N,
								    max_offset,
								    &O,
								    &mean,
								    &sd,
								    &p);
	    //printf("O:%u\tE:%f\tsd:%f\tp:%f\n", O, mean, sd, p);
    }

};

TEST_F(TestBitsCount, determine_seq) {
    //TestOverlaps();
    EXPECT_EQ(O, 0);
    EXPECT_EQ(sd, 0);
    EXPECT_EQ(mean, 0);
    EXPECT_EQ(p, 0);
    TestOverlaps();
    cout << endl << "O: " << O << "\tmean: " << mean << "\tsd: " << sd << "\tp: " << p << endl << endl;
    EXPECT_EQ(O, 18);
    EXPECT_EQ(p, 1);
}