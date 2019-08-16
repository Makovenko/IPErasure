#include <gurobi_c++.h>
#include "gurobi_stuff.h"

Gurobi::Gurobi() {
  env = new GRBEnv();
  model = new GRBModel(*env);
}

Gurobi::~Gurobi() {
  delete model;
  delete env;
}

GRBModel& Gurobi::operator()() {
  return *model;
}

void Gurobi::optimize() {
  try {
    model->optimize();
  } catch (GRBException exception) {
    std::cerr<<exception.getMessage()<<std::endl;
    exit(0);
  }
}
