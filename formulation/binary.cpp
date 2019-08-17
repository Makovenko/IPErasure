#include "binary.h"
#include "utils/galois_stuff.h"
#include "utils/gurobi_stuff.h"
#include <limits>
#include <numeric>
#include <iostream>

Binary::Binary(const Solution &sol):
  Formulation(sol)
{
  std::cerr<<"First Stage: Binary initialized"<<std::endl;
  generateXVariables();
  generatePiVariables();
  generateRVariables();

  generateUniquenessConstraints();
  generateXORConstraints();
  generatePriceConstraints();
}

int Binary::bin(int p, int t) const
{
  return (p>>t & 1);
}

void Binary::generateXVariables() {
  int n = m_currentSolution.rows + m_currentSolution.cols;
  m_X.resize(n);
  for (int l = 0; l < n; ++l)
    for (int t = 0; t < m_currentSolution.bits; ++t)
      m_X[l].push_back(m_solver().addVar(0, 1, 0, GRB_BINARY));
}

void Binary::generatePiVariables() {
  m_Pi.resize(m_currentSolution.cols);
  for (int i = 0; i < m_currentSolution.cols; ++i)
    for (int j = 0; j < m_currentSolution.rows; ++j)
      m_Pi[i].push_back(m_solver().addVar(
        0.0, std::numeric_limits<double>::max(),
        1.0, GRB_CONTINUOUS
      ));
}

void Binary::generateRVariables() {
  int n = m_currentSolution.rows + m_currentSolution.cols;
  m_R.resize(n, VarMatrix(n));
  for (int l = 0; l < n; ++l)
    for (int q = 0; q < n; ++q)
      for (int t = 0; t < m_currentSolution.bits; ++t)
        m_R[l][q].push_back(m_solver().addVar(0, 1, 0, GRB_BINARY));
}

void Binary::generateXORConstraints() {
  int n = m_currentSolution.rows + m_currentSolution.cols;
  for (int l = 0; l < n; ++l)
    for (int q = 0; q < n; ++q) {
      if (l == q) continue;
      for (int t = 0; t < m_currentSolution.bits; ++t) {
        m_solver().addConstr(
          m_R[l][q][t], GRB_LESS_EQUAL, m_X[l][t] + m_X[q][t]
        );
        m_solver().addConstr(
          m_R[l][q][t], GRB_LESS_EQUAL, 2 - m_X[l][t] - m_X[q][t]
        );
        m_solver().addConstr(
          m_R[l][q][t], GRB_GREATER_EQUAL, m_X[l][t] - m_X[q][t]
        );
        m_solver().addConstr(
          m_R[l][q][t], GRB_GREATER_EQUAL, -m_X[l][t] + m_X[q][t]
        );
      }
    }
}

void Binary::generateUniquenessConstraints() {
  int n = m_currentSolution.rows + m_currentSolution.cols;
  for (int l = 0; l < n; ++l) {
    for (int q = 0; q < n; ++q) {
      if (l == q) continue;
      m_solver().addConstr(
        std::accumulate(m_R[l][q].begin(), m_R[l][q].end(), GRBLinExpr(0.0)),
        GRB_GREATER_EQUAL, 1
      );
    }
  }
}

void Binary::generatePriceConstraints() {
  for (int i = 0; i < m_currentSolution.cols; ++i) {
    for (int j = 0; j < m_currentSolution.rows; ++j) {
      for (int p = 0; p < m_currentSolution.maxInt; ++p) {
        for (int d = 0; d < m_currentSolution.maxInt; ++d) {
          GRBLinExpr sum;
          for (int t = 0; t < m_currentSolution.bits; ++t) {
            sum += (m_X[i][t] + bin(p, t) - 2*m_X[i][t]*bin(p, t)) + (
              m_X[m_currentSolution.cols+j][t] + bin(d, t)
              - 2*m_X[m_currentSolution.cols+j][t]*bin(d, t)
            );
          }
          sum = get_cij(p, d, m_currentSolution.bits)*(1 - sum);
          m_solver().addConstr(m_Pi[i][j], GRB_GREATER_EQUAL, sum);
        }
      }
    }
  }
}

Solution Binary::nextSolution() {
  static int currentSolution = 0;
  int solutions = m_solver().get(GRB_IntParam_SolutionNumber);
  if (currentSolution == solutions)
    currentSolution = 0;

  m_solver().set(GRB_IntParam_SolutionNumber, currentSolution);
  Solution sol = m_currentSolution;

  for (int i = 0; i < m_currentSolution.cols; ++i) {
    int value = 0;
    for (int t = 0; t < m_currentSolution.bits; ++t) {
      value = (value << 1) | (m_X[i][t].get(GRB_DoubleAttr_Xn) > 0.5?1:0);
    }
    sol.encodersCol[i] = value;
  }

  for (int j = 0; j < m_currentSolution.rows; ++j) {
    int value = 0;
    for (int t = 0; t < m_currentSolution.bits; ++t)
      value = (value << 1) |
        (m_X[j+m_currentSolution.cols][t].get(GRB_DoubleAttr_Xn) > 0.5?1:0);
    sol.encodersRow[j] = value;
  }

  ++currentSolution;

  return sol;
}
