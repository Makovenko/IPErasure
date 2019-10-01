#ifndef FORMULATION_H
#define FORMULATION_H

#include <vector>
#include <gurobi_c++.h>
#include <memory>
#include "utils/gurobi_stuff.h"
#include "utils/solution.h"

class Formulation
{
protected:
  Gurobi m_solver;
  Solution m_currentSolution;

  [[deprecated]] int maxInt() const;

public:
  enum class SearchMode {Single, Multiple};
  enum class FormulationType {
    Binary, Exclusive, Combination,
    FullExclusive, FullCombination,
    InverseExclusive, None
  };

  Formulation(const Solution& sol);
  void setSearchMode(SearchMode mode);
  void setNumberOfSolutions(int solutions);
  void setTimeLimit(int seconds);

  void solve();
  virtual Solution nextSolution() = 0;
  std::vector<Solution> allSolutions();

  static FormulationType TypeFromParameters(const Parameters& P);
  static std::unique_ptr<Formulation> create(FormulationType T, const Solution& S);
};

#endif // FORMULATION_H
