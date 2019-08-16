#ifndef DESCENT_H
#define DESCENT_H

#include "secondstage.h"

class Descent: public SecondStage
{
private:
  void optimizeDown();
  void optimizeRight();

public:
  Descent(const Solution& sol);

  void solve();
  Solution solution() const;
};

#endif // DESCENT_H
