#ifndef MODEL_EXCLUSIVE_H
#define MODEL_EXCLUSIVE_H

#include <vector>
#include "utils/gurobi_stuff.h"
#include "formulation/formulation.h"

class Exclusive: public Formulation {
private:
  VarMatrix m_X;
  Var4Tensor m_Delta;

  void generateXVariables();
  void generateDeltaVariables();

  void generateUniquenessConstraints();
  void generateDeltaConstraints();

public:
  Exclusive(const Solution& sol);
  Solution nextSolution() override;
};

#endif
