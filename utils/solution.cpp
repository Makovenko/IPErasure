#include "solution.h"
#include "galois_stuff.h"

Solution::Solution(const Parameters &P): rows(P.r), cols(P.c), bits(P.w), maxInt(1<<bits) {
  multipliersRow.resize(rows, 1);
  multipliersCol.resize(cols, 1);
  encodersRow.resize(rows, 1);
  encodersCol.resize(cols, 1);
  for (int i = 0; i < rows; ++i) encodersRow[i] = i;
  for (int i = 0; i < cols; ++i) encodersCol[i] = rows+i;
}

Solution::Solution(const int k, const int r, const int w):
    rows(r), cols(k), bits(w), maxInt(1<<bits)
{
  multipliersRow.resize(rows, 1);
  multipliersCol.resize(cols, 1);
  encodersRow.resize(rows, 1);
  encodersCol.resize(cols, 1);
  for (int i = 0; i < rows; ++i) encodersRow[i] = i;
  for (int i = 0; i < cols; ++i) encodersCol[i] = rows+i;
}

Solution &Solution::operator=(const Solution &other)
{
  if (rows != other.rows || cols != other.cols || bits != other.bits) {
    return *this;
  }
  encodersRow = other.encodersRow;
  encodersCol = other.encodersCol;
  multipliersRow = other.multipliersRow;
  multipliersCol = other.multipliersCol;
  return *this;
}

bool Solution::isUsedInRow(int value, int toCol)
{
  if (!toCol) toCol = cols;
  return std::find(encodersRow.begin(), encodersRow.begin()+toCol, value) != (encodersRow.begin() + toCol);
}

int Solution::matrix(const int row, const int col) const {
  return galois_suminverse_extended(encodersRow[row], encodersCol[col], multipliersRow[row], multipliersCol[col], bits);
}

int Solution::cost(const int row, const int col) const {
  return get_cij_extended(encodersRow[row], encodersCol[col], multipliersRow[row], multipliersCol[col], bits);
}

int Solution::cost() const {
  int ans = 0;
  for (int r = 0; r < rows; ++r)
    for (int c = 0; c < cols; ++c)
      ans += cost(r, c);
  return ans;
}

std::ostream &operator<<(std::ostream &out, const Solution &sol)
{
  out<<"Row: ";
  for (int r = 0; r < sol.rows-1; ++r)
    out<<"("<<sol.encodersRow[r]<<", "<<sol.multipliersRow[r]<<"), ";
  out<<"("<<sol.encodersRow[sol.rows-1]<<", "<<sol.multipliersRow[sol.rows-1]<<")";
  out<<std::endl<<"Col: ";
  for (int c = 0; c < sol.cols-1; ++c)
    out<<"("<<sol.encodersCol[c]<<", "<<sol.multipliersCol[c]<<"), ";
  out<<"("<<sol.encodersCol[sol.cols-1]<<", "<<sol.multipliersCol[sol.cols-1]<<")";
  return out;
}
