#include "inverseexclusive.h"
#include "utils/galois_stuff.h"
#include <numeric>

InverseExclusive::InverseExclusive(const Solution& sol):
  Formulation(sol)
{
  generateRowColVariables();
  generatePrimeRowColVariables();
  generateCellVariables();
  generatePrimeCellVariables();
  generatePriceVariables();
  generateUniquenessConstraints();
  generatePrimeRowColConstraints();
  generateCellConstraints();
  generatePrimeCellConstraints();
  generatePriceConstraints();
  generateInverseConstraints();
}

void InverseExclusive::generateRowColVariables()
{
  row.resize(m_currentSolution.rows);
  for (auto& el: row)
    for (int p = 0; p < m_currentSolution.maxInt; ++p)
      el.push_back(m_solver().addVar(0, 1, 0, GRB_BINARY));
  col.resize(m_currentSolution.cols);
  for (auto& el: col)
    for (int p = 0; p < m_currentSolution.maxInt; ++p)
      el.push_back(m_solver().addVar(0, 1, 0, GRB_BINARY));
}

void InverseExclusive::generatePrimeRowColVariables()
{
  primeRow.resize(m_currentSolution.rows);
  for (auto& el: primeRow)
    for (int p = 0; p < m_currentSolution.maxInt; ++p)
      el.push_back(m_solver().addVar(0, 1, 0, GRB_BINARY));
  primeCol.resize(m_currentSolution.cols);
  for (auto& el: primeCol)
    for (int p = 0; p < m_currentSolution.maxInt; ++p)
      el.push_back(m_solver().addVar(0, 1, 0, GRB_BINARY));
}

void InverseExclusive::generateCellVariables()
{
  cell.resize(m_currentSolution.cols, VarMatrix(m_currentSolution.rows));
  for (auto& row: cell)
    for (auto& el: row)
      for (int p = 0; p < m_currentSolution.maxInt; ++p)
        el.push_back(m_solver().addVar(0, 1, 0, GRB_BINARY));
}

void InverseExclusive::generatePrimeCellVariables()
{
  primeCell.resize(m_currentSolution.cols, VarMatrix(m_currentSolution.rows));
  for (auto& row: primeCell)
    for (auto& el: row)
      for (int p = 0; p < m_currentSolution.maxInt; ++p)
        el.push_back(m_solver().addVar(0, 1, 0, GRB_BINARY));
}

void InverseExclusive::generatePriceVariables()
{
  price.resize(m_currentSolution.cols);
  for (auto& el: price)
    for (int i = 0; i < m_currentSolution.rows; ++i)
      el.push_back(m_solver().addVar(
        0, std::numeric_limits<double>::max(),
        1, GRB_CONTINUOUS
      ));
}

void InverseExclusive::generateUniquenessConstraints()
{
  // Every x must be assigned
  for (auto& el: row) {
    m_solver().addConstr(
      std::accumulate(el.begin(), el.end(), GRBLinExpr(0.0)),
      GRB_EQUAL, 1.0
    );
  }
  // Every y must be assigned
  for (auto& el: col) {
    m_solver().addConstr(
      std::accumulate(el.begin(), el.end(), GRBLinExpr(0.0)),
      GRB_EQUAL, 1.0
    );
  }
  // A finite field element can be assigned to at most one x or y
  for (int p = 0; p < m_currentSolution.maxInt; ++p) {
    auto sum = GRBLinExpr(0.0);
    for (auto& el: row) sum += el[p];
    for (auto& el: col) sum += el[p];
    m_solver().addConstr(sum, GRB_LESS_EQUAL, 1.0);
  }
}

void InverseExclusive::generatePrimeRowColConstraints()
{
  // Every prime x must be assigned
  for (auto& el: primeRow) {
    m_solver().addConstr(
      std::accumulate(el.begin()+1, el.end(), GRBLinExpr(0.0)),
      GRB_EQUAL, 1.0
    );
  }
  // Every prime y must be assigned
  for (auto& el: primeCol) {
    m_solver().addConstr(
      std::accumulate(el.begin()+1, el.end(), GRBLinExpr(0.0)),
      GRB_EQUAL, 1.0
    );
  }
}

void InverseExclusive::generateCellConstraints()
{
  for (int p = 0; p < m_currentSolution.maxInt; ++p) {
    for (int d = 0; d < m_currentSolution.maxInt; ++d) {
      int sum = galois_sum(p, d);
      for (int i = 0; i < m_currentSolution.cols; ++i) {
        for (int j = 1; j < m_currentSolution.rows; ++j) {
          m_solver().addConstr(
            cell[i][j][sum], GRB_GREATER_EQUAL, col[i][p]+row[j][d] - 1
          );
        }
      }
    }
  }
}

void InverseExclusive::generatePrimeCellConstraints()
{
  for (int p = 0; p < m_currentSolution.maxInt; ++p) {
    for (int d = 0; d < m_currentSolution.maxInt; ++d) {
      int product = galois_multiply(p, d, m_currentSolution.bits);
      for (int i = 0; i < m_currentSolution.cols; ++i) {
        for (int j = 1; j < m_currentSolution.rows; ++j) {
          m_solver().addConstr(
            primeCell[i][j][product], GRB_GREATER_EQUAL,
            primeCol[i][p] + primeRow[j][d] - 1
          );
        }
      }
    }
  }
}

void InverseExclusive::generatePriceConstraints()
{
  for (int p = 0; p < m_currentSolution.maxInt; ++p) {
    for (int d = 0; d < m_currentSolution.maxInt; ++d) {
      int c = get_cij_product(p, d, m_currentSolution.bits);
      for (int i = 0; i < m_currentSolution.cols; ++i) {
        for (int j = 1; j < m_currentSolution.rows; ++j) {
          m_solver().addConstr(
            price[i][j], GRB_GREATER_EQUAL,
            (primeCell[i][j][p] + cell[i][j][d] - 1)*c
          );
        }
      }
    }
  }
}

void InverseExclusive::generateInverseConstraints()
{
  std::cerr<<"Generating inverse constraints..."<<std::endl;
  m_solver().addConstr(primeRow[0][1], GRB_EQUAL, 1.0);

  for (int p = 0; p < m_currentSolution.maxInt; ++p) {
    for (int d = 0; d < m_currentSolution.maxInt; ++d) {
      int inverse = galois_suminverse(p, d, m_currentSolution.bits);
      if (inverse <= 0) continue;
      for (int c = 0; c < m_currentSolution.cols; ++c) {
        m_solver().addConstr(
          primeCol[c][inverse], GRB_GREATER_EQUAL, col[c][p] + row[0][d] - 1
        );
      }
    }
  }
  std::cerr<<"Done"<<std::endl;
}

Solution InverseExclusive::nextSolution()
{
  static int currentSolution = 0;
  int solutions = m_solver().get(GRB_IntParam_SolutionNumber);
  if (currentSolution == solutions)
    currentSolution = 0;

  m_solver().set(GRB_IntParam_SolutionNumber, currentSolution);
  Solution sol = m_currentSolution;
  std::cerr<<sol<<std::endl;
  for (int i = 0; i < m_currentSolution.rows; ++i)
    for (int p = 0; p < m_currentSolution.maxInt; ++p)
      if (row[i][p].get(GRB_DoubleAttr_Xn) > 0.5)
        sol.encodersRow[i] = p;
  for (int i = 0; i < m_currentSolution.cols; ++i)
    for (int p = 0; p < m_currentSolution.maxInt; ++p)
      if (col[i][p].get(GRB_DoubleAttr_Xn) > 0.5)
        sol.encodersCol[i] = p;
  for (int i = 0; i < m_currentSolution.rows; ++i)
    for (int p = 1; p < m_currentSolution.maxInt; ++p)
      if (primeRow[i][p].get(GRB_DoubleAttr_Xn) > 0.5)
        sol.multipliersRow[i] = p;
  for (int i = 0; i < m_currentSolution.cols; ++i)
    for (int p = 1; p < m_currentSolution.maxInt; ++p)
      if (primeCol[i][p].get(GRB_DoubleAttr_Xn) > 0.5)
        sol.multipliersCol[i] = p;
  ++currentSolution;
  return sol;
}
