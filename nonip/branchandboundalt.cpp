#include "branchandboundalt.h"
#include <queue>
#include <algorithm>
#include "utils/galois_stuff.h"

using triplet = std::pair<int, std::pair<int, int>>;

int BranchAndBoundAlt::solveEncoder(int col) {
  int cost = findOptimalColumn(col);
  int colsLeft = m_incumbentSolution.rows - col;
  int minPrice = m_incumbentSolution.bits + m_delta;
  int diff = colsLeft*minPrice*m_incumbentSolution.cols;
  if (cost + diff >= m_bestCost) return m_bestCost;

  if (col == m_incumbentSolution.rows) {
    m_currentSolution = m_incumbentSolution;
    m_bestCost = cost;
    std::cout<<"New best solution: "<<cost<<std::endl;
    std::cout<<m_currentSolution<<std::endl;
    return m_bestCost;
  }

  for (int e = m_incumbentSolution.encodersRow[col-1]+1; e < m_incumbentSolution.maxInt; ++e) {
    m_incumbentSolution.encodersRow[col] = e;
    solveMultiplier(col);
  }

  return m_bestCost;
}

int BranchAndBoundAlt::solveMultiplier(int col) {
  for (int m = 1; m < m_incumbentSolution.maxInt; ++m) {
    // To estimate the progress
    if (col == 1) {
      std::cout<<"Now considering the second multiplier to be "<<m<<std::endl;
    }
    m_incumbentSolution.multipliersRow[col] = m;
    solveEncoder(col+1);
  }
  return m_bestCost;
}

int BranchAndBoundAlt::solve()
{
  std::cout<<"Running Branch and Bound Algorithm (Alternative) with delta "<<m_delta<<std::endl;
  m_incumbentSolution.encodersRow[0] = 0;
  m_incumbentSolution.encodersRow[1] = 1;
  m_incumbentSolution.multipliersRow[0] = 1;
  return solveMultiplier(1);
}

Solution BranchAndBoundAlt::solution() const
{
  return m_currentSolution;
}

// Finds optimal column for a completed row of m_incumbentSolution
int BranchAndBoundAlt::findOptimalColumn(int toCol)
{
  std::priority_queue<triplet, std::vector<triplet>> bestRows;
  for (int i = 2; i < m_incumbentSolution.maxInt; ++i) {
    // Check if i was used in row assignment.
    if (m_incumbentSolution.isUsedInRow(i, toCol)) continue;
    // Find the best multiplier for encoder i
    int bestCost = std::numeric_limits<int>::max();
    std::pair<int, int> bestRow = std::make_pair(1, i);
    for (int m = 1; m < m_incumbentSolution.maxInt; ++m) {
      int cost = 0;
      for (int col = 0; col < toCol; ++col) {
        cost += get_cij_extended(
          m_incumbentSolution.encodersRow[col], i,
          m_incumbentSolution.multipliersRow[col], m,
          m_incumbentSolution.bits
        );
      }
      if (cost < bestCost) {
        bestRow.first = m;
        bestCost = cost;
      }
    }
    bestRows.push(std::make_pair(bestCost, bestRow));
    // Keep queue limited to size k
    if (static_cast<int>(bestRows.size()) > m_currentSolution.cols) bestRows.pop();
  }

  int cost = 0;
  for (int row = 0; row < m_currentSolution.cols; ++row) {
    auto crow = bestRows.top();
    m_incumbentSolution.multipliersCol[row] = crow.second.first;
    m_incumbentSolution.encodersCol[row] = crow.second.second;
    cost += crow.first;
    bestRows.pop();
  }
  return cost;
}

BranchAndBoundAlt::BranchAndBoundAlt(const Solution &initial, int delta):
  m_bestCost(std::numeric_limits<int>::max()), m_delta(delta),
  m_currentSolution(initial), m_incumbentSolution(initial)
{
  m_bestCost = initial.cost();
}
