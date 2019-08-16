#include "formulation/exclusive.h"
#include "utils/galois_stuff.h"
#include <numeric>

Exclusive::Exclusive(const Solution &sol):
  Formulation(sol)
{
  std::cerr<<"First Stage: Exclusive initialized"<<std::endl;
  generateXVariables();
  generateDeltaVariables();

  generateUniquenessConstraints();
  generateDeltaConstraints();
}

void Exclusive::generateXVariables() {
  int n = m_currentSolution.rows + m_currentSolution.cols;
  m_X.resize(n);
  for (int i = 0; i < n; ++i)
    for (int p = 0; p < maxInt(); ++p)
      m_X[i].push_back(m_solver().addVar(0, 1, 0, GRB_BINARY));
}

void Exclusive::generateDeltaVariables() {
  int n = m_currentSolution.rows + m_currentSolution.cols;
  m_Delta.resize(n, Var3Tensor(maxInt(), VarMatrix(n)));
  for (int i = 0; i < m_currentSolution.cols; ++i)
    for (int p = 0; p < maxInt(); ++p)
      for (int j = 0; j < m_currentSolution.rows; ++j)
        for (int d = 0; d < maxInt(); ++d)
          m_Delta[i][p][j].push_back(
            m_solver().addVar(
              0, 1, get_cij(p, d, m_currentSolution.bits),
              GRB_BINARY
            )
          );
}

void Exclusive::generateUniquenessConstraints() {
  int n = m_currentSolution.rows + m_currentSolution.cols;
  for (int i = 0; i < n; ++i)
    m_solver().addConstr(
      std::accumulate(m_X[i].begin(), m_X[i].end(), GRBLinExpr()),
      GRB_EQUAL, 1
    );
  for (int p = 0; p < maxInt(); ++p) {
    GRBLinExpr sum;
    for (int i = 0; i < n; ++i)
      sum += m_X[i][p];
    m_solver().addConstr(sum, GRB_LESS_EQUAL, 1);
  }
}

void Exclusive::generateDeltaConstraints() {
  for (int i = 0; i < m_currentSolution.cols; ++i)
    for (int p = 0; p < maxInt(); ++p)
      for (int j = 0; j < m_currentSolution.rows; ++j)
        for (int d = 0; d < maxInt(); ++d)
          m_solver().addConstr(
            m_Delta[i][p][j][d], GRB_GREATER_EQUAL,
            m_X[i][p] + m_X[m_currentSolution.cols + j][d] - 1
          );
}

Solution Exclusive::nextSolution()
{
  static int currentSolution = 0;
  int solutions = m_solver().get(GRB_IntParam_SolutionNumber);
  if (currentSolution == solutions)
    currentSolution = 0;

  m_solver().set(GRB_IntParam_SolutionNumber, currentSolution);
  Solution sol = m_currentSolution;

  for (int i = 0; i < m_currentSolution.cols; ++i)
    for (int p = 0; p < maxInt(); ++p)
      if (m_X[i][p].get(GRB_DoubleAttr_Xn) > 0.5) {
        sol.encodersCol[i] = p;
        break;
      }

  for (int j = 0; j < m_currentSolution.rows; ++j)
    for (int d = 0; d < maxInt(); ++d)
      if (m_X[m_currentSolution.cols + j][d].get(GRB_DoubleAttr_Xn) > 0.5) {
        sol.encodersRow[j] = d;
        break;
      }

  ++currentSolution;
  return sol;
}
