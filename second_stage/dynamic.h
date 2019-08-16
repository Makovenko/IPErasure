#ifndef DYNAMIC_H
#define DYNAMIC_H
#include "second_stage/secondstage.h"

class Dynamic: public SecondStage
{
private:
  void solve(int column, Solution &incumbent, int& best, int& iteration);
  int iterLimit = 1000000;
public:
  Dynamic(const Solution& sol);

  void solve();
};

#endif // DYNAMIC_H
