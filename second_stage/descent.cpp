#include "descent.h"
#include "utils/galois_stuff.h"
#include <limits>

Descent::Descent(const Solution &sol):
  SecondStage(sol)
{
  std::cerr<<"Second stage: Descent initialized"<<std::endl;
}

void Descent::solve()
{
  optimizeDown();
}

void Descent::optimizeDown()
{
  int oldValue = m_currentSolution.cost();
  int newValue = 0;
  for (int i = 0; i < m_currentSolution.cols; ++i) {
    int best = std::numeric_limits<int>::max();
    for (int a = 1; a < m_currentSolution.maxInt; ++a) {
      int sum = 0;
      for (int c = 0; c < m_currentSolution.rows; ++c)
        sum += get_cij_extended(
          m_currentSolution.encodersCol[i], m_currentSolution.encodersRow[c],
          a, m_currentSolution.multipliersRow[c], m_currentSolution.bits
        );
      if (best > sum) {
        m_currentSolution.multipliersCol[i] = a;
        best = sum;
      }
    }
    newValue += best;
  }
  std::cout<<"Optimized the matrix downwards. New value is "<<newValue<<", while old was "<<oldValue<<". ";
  if (newValue < oldValue) {
    std::cout<<"Going to optimize rightwards."<<std::endl;
    optimizeRight();
  }
}

void Descent::optimizeRight() {
  int oldValue = m_currentSolution.cost();
  int newValue = 0;
  for (int i = 0; i < m_currentSolution.rows; ++i) {
    int best = std::numeric_limits<int>::max();
    for (int a = 1; a < m_currentSolution.bits; ++a) {
      int sum = 0;
      for (int r = 0; r < m_currentSolution.cols; ++r)
        sum += get_cij_extended(
          m_currentSolution.encodersRow[i], m_currentSolution.encodersCol[r],
          a, m_currentSolution.multipliersCol[r], m_currentSolution.bits
        );
      if (best > sum) {
        m_currentSolution.multipliersRow[i] = a;
        best = sum;
      }
    }
    newValue += best;
  }
  std::cout<<"Optimized the matrix rightwards. New value is "<<newValue<<", while old was "<<oldValue<<". ";
  if (newValue < oldValue) {
    std::cout<<"Going to optimize downwards."<<std::endl;
    optimizeDown();
  }
}
