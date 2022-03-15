/*****************************************************************************
bits_count.cpp
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
#include "lineFileUtilities.h"
#include "bits_count.h"
#include "timer.h"
#include "file_read.h"


/*
Constructor
*/
BitsCount::BitsCount(string bedAFile, vector<string> bedBFiles, string genomeFile) {

    _bedAFile = bedAFile;
    _bedBFiles = bedBFiles;
    _genomeFile = genomeFile;
    
    // create new BED file objects for A and B
	_bedB_count = bedBFiles.size();	

	_bedA = new BedFile(bedAFile);
	for (int i = 0; i < _bedB_count; i++) {
		_bedBs.push_back(new BedFile(bedBFiles[i]));
	}
	//TODO#1 get rid of A as plural. Right now it was being set up to have a list of A files. Only want one. change it back
    //_genome = new BedFile(genomeFile);
    _genome = new GenomeFile(genomeFile);
    
    CountOverlaps();
}


/*
Destructor
*/
BitsCount::~BitsCount(void) {
}

void BitsCount::CountOverlaps() {
    struct interval *A;
	unsigned int *B_starts, *B_ends, *B_combine_starts, *B_combine_ends;
	unsigned int A_size, B_size;
	unsigned int curr_combine_index = 0;
	
	unsigned int total_bed_size = read_total_B_file_size(_bedBs);
	B_combine_starts = (unsigned int *) malloc(sizeof(struct interval) * total_bed_size);


	fill_offsets(_genome, &_offsets);
	read_and_map_Afiles_to_array_skip_vector(&_offsets,
						_bedA,
						&A,
						&A_size);
	
	for (int i = 0; i < _bedB_count; i++) {
		read_and_map_multiple_Bfiles_to_array_skip_vector(&_offsets,
						_bedBs,
						&B_starts,
						&B_ends,
						&B_size);
		combine_startend_arrays(&B_starts, &B_ends, &B_size, &B_combine_starts, &B_combine_ends, &curr_combine_index, total_bed_size);
	}//TODO the B_starts ends and size will be overwritten each time // combine them into one big B_start and ends


    uint32_t tot_overlaps = 
		count_intersections_bsearch_seq_mem( A,
											 A_size,
											 B_starts,
											 B_ends,
											 B_size);

	cout << tot_overlaps << endl;
}
