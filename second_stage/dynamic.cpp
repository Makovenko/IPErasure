#include "dynamic.h"
#include "utils/galois_stuff.h"
#include <limits>

Dynamic::Dynamic(const Solution& sol):
  SecondStage(sol)
{
  std::cerr<<"Second stage: Dynamic initialized"<<std::endl;
}

void Dynamic::solve()
{
  Solution incumbent(m_currentSolution);
  int best = incumbent.cost();
  int iteration = 0;
  solve(0, incumbent, best, iteration);
  if (iteration > iterLimit)
    std::cerr<<"Second stage terminated on iteration limit"<<std::endl;
}

void Dynamic::solve(int column, Solution& incumbent, int &best, int &iteration)
{
  ++iteration;
  if (column == m_currentSolution.rows) {
    best = incumbent.cost();
    std::cerr<<"New best solution found with value "<<best<<std::endl;
    m_currentSolution = incumbent;
    return;
  }
  if (iteration > iterLimit) return;
  for (int m = 1; m < (column==0?2:m_currentSolution.maxInt); ++m) {
    int total = 0;
    for (int row = 0; row < m_currentSolution.cols; ++row) {
      int bestinrow = std::numeric_limits<int>::max();
      for (int n = 1; n < m_currentSolution.maxInt; ++n) {
        int current = 0;
        for (int i = 0; i < column; ++i)
          current += get_cij_extended(
            incumbent.encodersRow[i], incumbent.encodersCol[row],
            incumbent.multipliersRow[i], n, incumbent.bits
          );
        current += get_cij_extended(
          incumbent.encodersRow[column], incumbent.encodersCol[row],
          m, n, m_currentSolution.bits
        );
        if (current < bestinrow) {
          bestinrow = current;
          incumbent.multipliersCol[row] = n;
        }
      }
      total += bestinrow;
      if (total + (m_currentSolution.rows-1-column)*m_currentSolution.cols*m_currentSolution.bits >= best) {
        break;
      }
    }
    if (total + (m_currentSolution.rows-1-column)*m_currentSolution.cols*m_currentSolution.bits < best) {
      incumbent.multipliersRow[column] = m;
      solve(column+1, incumbent, best, iteration);
    }
  }
}
