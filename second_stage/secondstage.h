#ifndef SECONDSTAGE_H
#define SECONDSTAGE_H
#include "utils/solution.h"
#include <memory>

class SecondStage
{
protected:
  Solution m_currentSolution;

public:
  enum class SecondStageType {None, Descent, Dynamic, IP};

  SecondStage(const Solution& sol);
  virtual void solve() {}
  Solution solution() const;

  static SecondStageType TypeFromParameters(const Parameters& P);
  static std::unique_ptr<SecondStage> create(SecondStageType T, const Solution& sol);
};

#endif // SECONDSTAGE_H
