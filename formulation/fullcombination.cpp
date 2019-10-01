#include "fullcombination.h"
#include "utils/galois_stuff.h"
#include <numeric>

FullCombination::FullCombination(const Solution& sol):
  Formulation(sol)
{
  generateRowColVariables();
  generatePiVariables();

  generatePermutationConstraints();
  generateUniquenessConstraints();
  generatePriceConstraints();
}

Solution FullCombination::nextSolution()
{
  static int currentSolution = 0;
  int solutions = m_solver().get(GRB_IntParam_SolutionNumber);
  if (currentSolution == solutions)
    currentSolution = 0;

  m_solver().set(GRB_IntParam_SolutionNumber, currentSolution);
  int rcnt = 0, kcnt = 0;
  Solution sol = m_currentSolution;
  for (int p = 0; p < m_currentSolution.maxInt; ++p) {
    for (int l = 0; l < m_currentSolution.maxInt; ++l) {
      if (m_row[p][l].get(GRB_DoubleAttr_Xn) > 0.5) {
        sol.encodersRow[rcnt] = p;
        sol.multipliersRow[rcnt++] = l;
      }
      if (m_col[p][l].get(GRB_DoubleAttr_Xn) > 0.5) {
        sol.encodersCol[kcnt] = p;
        sol.multipliersCol[kcnt++] = l;
      }
    }
  }
  ++currentSolution;

  return sol;
}

void FullCombination::generateRowColVariables()
{
  m_row.resize(m_currentSolution.maxInt);
  for (auto& el: m_row)
    for (int p = 0; p < m_currentSolution.maxInt; ++p)
      el.push_back(m_solver().addVar(0, 1, 0, GRB_BINARY));
  m_col.resize(m_currentSolution.maxInt);
  for (auto& el: m_col)
    for (int p = 0; p < m_currentSolution.maxInt; ++p)
      el.push_back(m_solver().addVar(0, 1, 0, GRB_BINARY));
}

void FullCombination::generatePiVariables() {
  m_Pi.resize(m_currentSolution.maxInt);
  for (auto& el: m_Pi)
    for (int q = 0; q < m_currentSolution.maxInt; ++q)
      el.push_back(m_solver().addVar(
        0.0, std::numeric_limits<double>::max(),
        1.0, GRB_CONTINUOUS
      ));
}

void FullCombination::generatePermutationConstraints()
{
  GRBLinExpr sum_row(0.0);
  for (auto& el: m_row)
    sum_row += std::accumulate(el.begin()+1, el.end(), GRBLinExpr(0.0));
  GRBLinExpr sum_col(0.0);
  for (auto& el: m_col)
    sum_col += std::accumulate(el.begin()+1, el.end(), GRBLinExpr(0.0));
  m_solver().addConstr(sum_row, GRB_EQUAL, m_currentSolution.rows);
  m_solver().addConstr(sum_col, GRB_EQUAL, m_currentSolution.cols);
}

void FullCombination::generateUniquenessConstraints()
{
  for (int p = 0; p < m_currentSolution.maxInt; ++p) {
    GRBLinExpr sum(0.0);
    sum += std::accumulate(m_row[p].begin(), m_row[p].end(), GRBLinExpr(0.0));
    sum += std::accumulate(m_col[p].begin(), m_col[p].end(), GRBLinExpr(0.0));
    m_solver().addConstr(sum, GRB_LESS_EQUAL, 1.0);
  }
}

void FullCombination::generatePriceConstraints()
{
  GRBLinExpr sum(0.0);
  for (int l = 0; l < m_currentSolution.maxInt; ++l) {
    for (int q = 0; q < m_currentSolution.maxInt; ++q) {
      sum += m_Pi[l][q];
      for (int p = 0; p < m_currentSolution.maxInt; ++p) {
        for (int d = 0; d < m_currentSolution.maxInt; ++d) {
          m_solver().addConstr(
            m_Pi[l][q], GRB_GREATER_EQUAL,
            get_cij_extended(l, q, p, d, m_currentSolution.bits)*(m_row[l][p] + m_col[q][d] - 1)
          );
        }
      }
    }
  }
  m_solver().addConstr(
    sum, GRB_GREATER_EQUAL,
    m_currentSolution.bits*m_currentSolution.cols*m_currentSolution.rows
  );
}
