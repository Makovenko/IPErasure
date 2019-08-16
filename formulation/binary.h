#ifndef MODEL_BINARY_H
#define MODEL_BINARY_H

#include <vector>
#include "utils/gurobi_stuff.h"
#include "formulation.h"

class Binary: public Formulation {
private:
  VarMatrix m_X, m_Pi;
  Var3Tensor m_R;

  inline int bin(int p, int t) const;

  void generateXVariables();
  void generatePiVariables();
  void generateRVariables();

  void generateXORConstraints();
  void generateUniquenessConstraints();
  void generatePriceConstraints();

public:
  Binary(const Solution& sol);
  Solution nextSolution();
  std::vector<Solution> allSolutions();
};

#endif
