#ifndef GUROBI_STUFF_H
#define GUROBI_STUFF_H
#include "gurobi_c++.h"

using VarVector  = std::vector<GRBVar>;
using VarMatrix  = std::vector<VarVector>;
using Var3Tensor = std::vector<VarMatrix>;
using Var4Tensor = std::vector<Var3Tensor>;
using ConstrList = std::vector<GRBConstr>;

class GRBEnv;
class GRBModel;

struct Gurobi {
  GRBEnv* env;
  GRBModel* model;

  Gurobi();
  ~Gurobi();
  GRBModel& operator()();

  void optimize();
};

#endif
