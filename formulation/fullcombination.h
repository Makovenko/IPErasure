#ifndef FULLCOMBINATION_H
#define FULLCOMBINATION_H
#include "formulation/formulation.h"

class FullCombination: public Formulation
{
  VarMatrix m_row, m_col;
  VarMatrix m_Pi;

  void generateRowColVariables();
  void generatePiVariables();

  void generatePermutationConstraints();
  void generateUniquenessConstraints();
  void generatePriceConstraints();

public:
  FullCombination(const Solution& sol);
  Solution nextSolution();
};

#endif // FULLCOMBINATION_H
