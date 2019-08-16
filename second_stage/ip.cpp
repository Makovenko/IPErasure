#include "ip.h"
#include "utils/galois_stuff.h"
#include <limits>
#include <numeric>

IP::IP(const Solution &sol):
  SecondStage(sol)
{
  std::cerr<<"Second stage: Integer Programming initialized"<<std::endl;
  generatePrimeVariables();
  generatePriceVariables();
  generateSingleAssignmentConstraints();
  generatePriceConstraints();
}

void IP::solve()
{
  m_solver().optimize();
  for (int r = 0; r < m_currentSolution.rows; ++r)
    for (int p = 0; p < m_currentSolution.maxInt; ++p)
      if (primeRow[r][p].get(GRB_DoubleAttr_X) > 0.5)
        m_currentSolution.multipliersRow[r] = p;
  for (int c = 0; c < m_currentSolution.cols; ++c)
    for (int p = 0; p < m_currentSolution.maxInt; ++p)
      if (primeCol[c][p].get(GRB_DoubleAttr_X) > 0.5)
        m_currentSolution.multipliersCol[c] = p;
}

void IP::generatePrimeVariables()
{
  primeRow.resize(m_currentSolution.rows);
  for (int r = 0; r < m_currentSolution.rows; ++r) {
    for (int p = 0; p < m_currentSolution.maxInt; ++p) {
      primeRow[r].push_back(m_solver().addVar(0, p==0?0:1, 0, GRB_BINARY));
    }
    primeRow[r][m_currentSolution.multipliersRow[r]].set(GRB_DoubleAttr_Start, 1);
  }

  primeCol.resize(m_currentSolution.cols);
  for (int c = 0; c < m_currentSolution.cols; ++c) {
    for (int p = 0; p < m_currentSolution.maxInt; ++p) {
      primeCol[c].push_back(m_solver().addVar(0, p==0?0:1, 0, GRB_BINARY));
    }
    primeCol[c][m_currentSolution.multipliersCol[c]].set(GRB_DoubleAttr_Start, 1);
  }
}

void IP::generatePriceVariables()
{
  price.resize(m_currentSolution.rows);
  for (int r = 0; r < m_currentSolution.rows; ++r) {
    for (int c = 0; c < m_currentSolution.cols; ++c) {
      price[r].push_back(m_solver().addVar(
        0, std::numeric_limits<double>::max(), 1.0, GRB_CONTINUOUS
      ));
    }
  }
}

void IP::generateSingleAssignmentConstraints()
{
  for (int r = 0; r < m_currentSolution.rows; ++r) {
    m_solver().addConstr(
      std::accumulate(primeRow[r].begin(), primeRow[r].end(), GRBLinExpr(0.0)),
      GRB_EQUAL, 1
    );
  }
  for (int c = 0; c < m_currentSolution.cols; ++c) {
    m_solver().addConstr(
     std::accumulate(primeCol[c].begin(), primeCol[c].end(), GRBLinExpr(0.0)),
     GRB_EQUAL, 1
    );
  }
}

void IP::generatePriceConstraints()
{
  for (int r = 0; r < m_currentSolution.rows; ++r) {
    for (int c = 0; c < m_currentSolution.cols; ++c) {
      for (int p = 1; p < m_currentSolution.maxInt; ++p) {
        for (int t = 1; t < m_currentSolution.maxInt; ++t) {
          int cost = get_cij_extended(
            m_currentSolution.encodersRow[r], m_currentSolution.encodersCol[c],
            p, t, m_currentSolution.bits
          );
          m_solver().addConstr(
           price[r][c], GRB_GREATER_EQUAL,
           cost*(primeRow[r][p] + primeCol[c][t] - 1)
          );
        }
      }
    }
  }
}
