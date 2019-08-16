#include "formulation.h"
#include "formulation/binary.h"
#include "formulation/exclusive.h"
#include "formulation/combination.h"
#include "formulation/fullexclusive.h"

Formulation::Formulation(const Solution &sol):
  m_currentSolution(sol)
{

}

int Formulation::maxInt() const
{
  return (1<<m_currentSolution.bits);
}

Formulation::FormulationType Formulation::TypeFromParameters(const Parameters &P)
{
  switch (P.firststage) {
    case 1:
      return FormulationType::Exclusive;
    case 2:
      return FormulationType::Binary;
    case 3:
      return FormulationType::Combination;
    case 4:
      return FormulationType::FullExclusive;
  }
  return FormulationType::Combination;
}

void Formulation::setSearchMode(Formulation::SearchMode mode)
{
  if (mode == SearchMode::Single)
    m_solver().set(GRB_IntParam_PoolSearchMode, 0);
  else
    m_solver().set(GRB_IntParam_PoolSearchMode, 2);
}

void Formulation::setNumberOfSolutions(int solutions)
{
  m_solver().set(GRB_IntParam_PoolSolutions, solutions);
}

void Formulation::setTimeLimit(int seconds)
{
  if (seconds)
    m_solver().getEnv().set(GRB_DoubleParam_TimeLimit, seconds);
  else
    m_solver().getEnv().set(GRB_DoubleParam_TimeLimit, GRB_INFINITY);
}

void Formulation::solve() {
  m_solver().optimize();
}

std::vector<Solution> Formulation::allSolutions()
{
  std::vector<Solution> result;
  int solutions = m_solver().get(GRB_IntAttr_SolCount);
  for (int i = 0; i < solutions; ++i) {
    result.push_back(nextSolution());
  }
  return result;
}

std::unique_ptr<Formulation> Formulation::create(Formulation::FormulationType T, const Solution &S)
{
  switch (T) {
    case FormulationType::Exclusive:
      return std::make_unique<Exclusive>(S);
    case FormulationType::Binary:
      return std::make_unique<Binary>(S);
    case FormulationType::Combination:
      return std::make_unique<Combination>(S);
    case FormulationType::FullExclusive:
      return std::make_unique<FullExclusive>(S);
  }
  return std::make_unique<Combination>(S);
}
