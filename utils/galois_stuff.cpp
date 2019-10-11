#include <galois.h>
#include <jerasure.h>
#include <iostream>
#include <vector>
#include "galois_stuff.h"

int calculate_number_of_ones(int* bitmatrix, int r, int c, int w) {
  int sum = 0;
  for (int i = 0; i < w*r; ++i) {
    for (int j = 0; j < w*c; ++j) {
      sum += bitmatrix[i*w*c + j];
    }
  }
  return sum;
}

int galois_sum(int i, int j) {
  galois_region_xor(
    reinterpret_cast<char*>(&i),
    reinterpret_cast<char*>(&j),
    sizeof(i)
  );
  return j;
}

int galois_multiply(int i, int j, int w) {
  return galois_single_multiply(i, j, w);
}

int galois_suminverse(int i, int j, int w) {
  j = galois_sum(i, j);
  j = galois_inverse(j, w);
  return j;
}

int galois_prodinverse(int i, int j, int w) {
  j = galois_multiply(i, j, w);
  j = galois_inverse(j, w);
  return j;
}

int get_cij_product(int i, int j, int w) {
  static std::vector<int> ans(1<<w, -1);
  j = galois_prodinverse(i, j, w);
  if (ans[j] == -1) {
    int *bitmatrix = jerasure_matrix_to_bitmatrix(1, 1, w, &j);
    int result = calculate_number_of_ones(bitmatrix, 1, 1, w);
    delete[] bitmatrix;
    ans[j] = result;
  }
  return ans[j];
}

inline int galois_quick_multiply(int i, int j, int w) {
  static std::vector<int> ans(1<<(2*w), -1);
  if (ans[(i<<w)+j] == -1)
    ans[(i<<w)+j] = galois_single_multiply(i, j, w);
  return ans[(i<<w)+j];
}

inline int galois_quick_inverse(int i, int w) {
  static std::vector<int> ans(1<<w, -1);
  if (ans[i] == -1)
    ans[i] = galois_inverse(i, w);
  return ans[i];
}

int galois_suminverse_extended(int i, int j, int a, int t, int w) {
  galois_region_xor(
    reinterpret_cast<char*>(&i),
    reinterpret_cast<char*>(&j),
    sizeof(i)
  );
//  j = galois_single_multiply(j, a, w);
//  j = galois_single_multiply(j, t, w);
//  j = galois_inverse(j, w);
//  return j;
  j = galois_quick_multiply(j, a, w);
  j = galois_quick_multiply(j, t, w);
  j = galois_quick_inverse(j, w);
  return j;
}

int get_cij(int i, int j, int w) {
  static std::vector<int> ans(1<<w, -1);
  if (i == j)
    return 0;
  j = galois_suminverse(i, j, w);
  if (ans[j] == -1) {
    int *bitmatrix = jerasure_matrix_to_bitmatrix(1, 1, w, &j);
    int result = calculate_number_of_ones(bitmatrix, 1, 1, w);
    delete[] bitmatrix;
    ans[j] = result;
  }
  return ans[j];
}

int get_cij_extended(int i, int j, int a, int t, int w) {
  static std::vector<int> ans(1<<w, -1);
  if (i == j || a == 0 || t == 0)
    return 0;
  j = galois_suminverse_extended(i, j, a, t, w);
  if (ans[j] == -1) {
    int *bitmatrix = jerasure_matrix_to_bitmatrix(1, 1, w, &j);
    int result = calculate_number_of_ones(bitmatrix, 1, 1, w);
    delete[] bitmatrix;
    ans[j] = result;
  }
  return ans[j];
}
