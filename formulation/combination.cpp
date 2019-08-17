#include "combination.h"
#include "utils/galois_stuff.h"
#include "utils/gurobi_stuff.h"
#include <limits>
#include <iostream>
#include <numeric>

Combination::Combination(const Solution &sol):
  Formulation(sol)
{
  std::cerr<<"First Stage: Combination initialized"<<std::endl;
  generateXVariables();
  generatePiVariables();
  generatePermutationConstraints();
  generatePriceConstraints();
}

void Combination::generateXVariables() {
  m_X.resize(m_currentSolution.maxInt);
  for (int l = 0; l < m_currentSolution.maxInt; ++l) {
    m_X[l].push_back(m_solver().addVar(0, 1, 0, GRB_BINARY));
    m_X[l].push_back(m_solver().addVar(0, 1, 0, GRB_BINARY));
  }
}

void Combination::generatePiVariables() {
  m_Pi.resize(m_currentSolution.maxInt);
  for (int l = 0; l < m_currentSolution.maxInt; ++l) {
    for (int q = 0; q < m_currentSolution.maxInt; ++q) {
      m_Pi[l].push_back(m_solver().addVar(
        0, std::numeric_limits<double>::max(),
        1.0, GRB_CONTINUOUS
      ));
    }
  }
}

void Combination::generatePermutationConstraints() {
  GRBLinExpr sum_r, sum_k;
  for (int l = 0; l < m_currentSolution.maxInt; ++l) {
    sum_r += m_X[l][0];
    sum_k += m_X[l][1];
    m_solver().addConstr(m_X[l][0]+m_X[l][1], GRB_LESS_EQUAL, 1);
  }
  m_solver().addConstr(sum_r, GRB_EQUAL, m_currentSolution.rows);
  m_solver().addConstr(sum_k, GRB_EQUAL, m_currentSolution.cols);
}

void Combination::generatePriceConstraints() {
  for (int l = 0; l < m_currentSolution.maxInt; ++l) {
    for (int q = 0; q < m_currentSolution.maxInt; ++q) {
      m_solver().addConstr(
        m_Pi[l][q], GRB_GREATER_EQUAL,
        get_cij(l, q, m_currentSolution.bits)*(m_X[l][0] + m_X[q][1] - 1)
      );
    }
  }
}

Solution Combination::nextSolution()
{
  static int currentSolution = 0;
  int solutions = m_solver().get(GRB_IntParam_SolutionNumber);
  if (currentSolution == solutions)
    currentSolution = 0;

  m_solver().set(GRB_IntParam_SolutionNumber, currentSolution);
  int rcnt = 0, kcnt = 0;
  Solution sol = m_currentSolution;
  for (int l = 0; l < m_currentSolution.maxInt; ++l) {
    if (m_X[l][0].get(GRB_DoubleAttr_Xn) > 0.5)
      sol.encodersRow[rcnt++] = l;
    if (m_X[l][1].get(GRB_DoubleAttr_Xn) > 0.5)
      sol.encodersCol[kcnt++] = l;
  }
  ++currentSolution;

  return sol;
}
