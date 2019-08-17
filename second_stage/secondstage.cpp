#include "secondstage.h"
#include "second_stage/descent.h"
#include "second_stage/dynamic.h"
#include "second_stage/ip.h"
#include <limits>

void SecondStage::setBound(int bound)
{
  m_bound = bound;
}

SecondStage::SecondStage(const Solution &sol):
  m_currentSolution(sol)
{
  m_bound = std::numeric_limits<int>::max();
}

Solution SecondStage::solution() const
{
  return m_currentSolution;
}

SecondStage::SecondStageType SecondStage::TypeFromParameters(const Parameters &P)
{
  switch (P.secondstage) {
    case 0:
      return SecondStageType::None;
    case 1:
      return SecondStageType::Descent;
    case 2:
      return SecondStageType::Dynamic;
    case 3:
      return SecondStageType::IP;
  }
  return SecondStageType::None;
}

std::unique_ptr<SecondStage> SecondStage::create(SecondStage::SecondStageType T, const Solution &sol)
{
  switch (T) {
    case SecondStageType::None:
      return std::make_unique<SecondStage>(sol);
    case SecondStageType::Descent:
      return std::make_unique<Descent>(sol);
    case SecondStageType::Dynamic:
      return std::make_unique<Dynamic>(sol);
    case SecondStageType::IP:
      return std::make_unique<IP>(sol);
  }
  return std::make_unique<SecondStage>(sol);
}
