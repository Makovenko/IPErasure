#ifndef FULLEXCLUSIVE_H
#define FULLEXCLUSIVE_H
#include "formulation/formulation.h"

class FullExclusive: public Formulation
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
public:
  FullExclusive(const Solution& sol);
  Solution nextSolution();
};

#endif // FULLEXCLUSIVE_H
