#ifndef delta
#define delta

#include <vector>

using namespace std;



class Delta {

public:
    int* varint_deltastarts;
    int* varint_ends;
    vector<int> deltastarts;
    vector<int> deltaends;
    vector<int> initial_vals;
    
    Delta();

    int compute_deltas_d2(int * Bstarts, int * Bends, int Blength);
    int compress_varint();
    int find_match(int * A);
};

#endif