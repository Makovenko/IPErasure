#ifndef INVERSEEXCLUSIVE_H
#define INVERSEEXCLUSIVE_H
#include "formulation.h"

class InverseExclusive: public Formulation
{
private:
  VarMatrix row, col;
  VarMatrix primeRow, primeCol;
  Var3Tensor cell;
  Var3Tensor primeCell;
  VarMatrix price;

  void generateRowColVariables();
  void generatePrimeRowColVariables();
  void generateCellVariables();
  void generatePrimeCellVariables();
  void generatePriceVariables();

  void generateUniquenessConstraints();
  void generatePrimeRowColConstraints();
  void generateCellConstraints();
  void generatePrimeCellConstraints();
  void generatePriceConstraints();

  void generateInverseConstraints();
public:
  InverseExclusive(const Solution& sol);
  Solution nextSolution();
};

#endif // INVERSEEXCLUSIVE_H
