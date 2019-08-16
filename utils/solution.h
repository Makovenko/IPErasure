#ifndef SOLUTION_H
#define SOLUTION_H

#include <vector>
#include <iostream>
#include "utils/parameters.h"

struct Solution {
  std::vector<int> multipliersRow, multipliersCol;
  std::vector<int> encodersRow, encodersCol;

  const int rows;
  const int cols;
  const int bits;
  const int maxInt;

  Solution(const Parameters& P);
  Solution& operator=(const Solution& other);

  int matrix(const int row, const int col) const;
  int cost(const int row, const int col) const;
  int cost() const;
};

std::ostream& operator<<(std::ostream& out, const Solution& sol);

#endif
