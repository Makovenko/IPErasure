#ifndef BRANCHANDBOUNDALT_H
#define BRANCHANDBOUNDALT_H
#include "utils/solution.h"

class BranchAndBoundAlt
{
private:
  int m_bestCost;
  int m_delta;
  Solution m_currentSolution;
  Solution m_incumbentSolution;

  int findOptimalColumn(int toCol);

  int solveEncoder(int col);
  int solveMultiplier(int col);
public:
  int solve();
  Solution solution() const;
  BranchAndBoundAlt(const Solution& initial, int delta = 0);
};

#endif // BRANCHANDBOUNDALT_H
