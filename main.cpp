// STD/STL includes
#include <iostream>
#include <numeric>
#include "formulation/formulation.h"
#include "second_stage/secondstage.h"
#include "utils/parameters.h"

std::vector<Solution> first_stage(const Parameters& P) {
  std::cerr<<"Running the first stage."<<std::endl;
  Solution initial(P);
  try {
    auto solver = Formulation::create(Formulation::TypeFromParameters(P), initial);
    solver->setSearchMode(Formulation::SearchMode::Multiple);
    solver->setNumberOfSolutions(P.L);
    solver->setTimeLimit(P.timelimit);
    if (P.L == 1)
      solver->setSearchMode(Formulation::SearchMode::Single);
    solver->solve();
    return solver->allSolutions();
  } catch (GRBException exc) {
    std::cout<<exc.getMessage()<<std::endl;
    return std::vector<Solution>();
  }
}

Solution second_stage(Solution sol, const Parameters &P, int best) {
  std::cerr<<"Running the second stage."<<std::endl;
  std::cerr<<"Objective value before second stage: "<<sol.cost()<<std::endl;
  auto solver = SecondStage::create(SecondStage::TypeFromParameters(P), sol);
  solver->setBound(best);
  solver->solve();
  return solver->solution();
}

int main(int argc, char** argv) {
  Parameters P(argc, argv);
  Solution best(P);
  auto fsol = first_stage(P);
  std::cerr<<fsol[0]<<std::endl;
  for (auto& sol: fsol) {
    auto ssol = second_stage(sol, P, best.cost());
    if (ssol.cost() < best.cost()) {
      best = ssol;
      std::cerr<<"New best solution value: "<<best.cost()<<std::endl;
    }
  }
  std::cout<<"Best solution value: "<<best.cost()<<std::endl;
  std::cout<<best<<std::endl;
  return 0;
}
