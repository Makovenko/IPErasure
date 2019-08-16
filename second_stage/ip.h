#ifndef IP_H
#define IP_H
#include "second_stage/secondstage.h"
#include "utils/gurobi_stuff.h"

class IP: public SecondStage
{
private:
  Gurobi m_solver;
  VarMatrix primeCol, primeRow, price;

  // Variables
  void generatePrimeVariables();
  void generatePriceVariables();
  // Constraints
  void generateSingleAssignmentConstraints();
  void generatePriceConstraints();

public:
  IP(const Solution& sol);

  void solve();
};

#endif // IP_H
