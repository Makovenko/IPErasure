#ifndef GALOIS_STUFF_H
#define GALOIS_STUFF_H

int calculate_number_of_ones(int* bitmatrix, int r, int c, int w);
int galois_sum(int i, int j);
int galois_multiply(int i, int j, int w);
int galois_suminverse(int i, int j, int w);
int galois_prodinverse(int i, int j, int w);
int galois_suminverse_extended(int i, int j, int a, int t, int w);
// This routine supports w of up to sizeof(int), which is, generally, 32 bits.
int get_cij_product(int i, int j, int w);
int get_cij(int i, int j, int w);
int get_cij_extended(int i, int j, int a, int t, int w);

#endif
