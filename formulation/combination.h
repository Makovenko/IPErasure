#ifndef MODEL_COMBINATION_H
#define MODEL_COMBINATION_H

#include <vector>
#include "formulation.h"
#include "utils/solution.h"

class Combination: public Formulation{
private:
  VarMatrix m_X, m_Pi;

  void generateXVariables();
  void generatePiVariables();

  void generatePermutationConstraints();
  void generatePriceConstraints();

public:
  Combination(const Solution& sol);

  Solution nextSolution() override;
};

#endif
