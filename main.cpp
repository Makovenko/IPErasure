// STD/STL includes
#include <iostream>
#include <numeric>
#include "formulation/formulation.h"
#include "second_stage/secondstage.h"
#include "utils/parameters.h"
#include "nonip/branchandboundalt.h"
#include <csignal>

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

BranchAndBoundAlt *signalptr;
void signalHandlerNonIP( int signum ) {
  std::cout<<std::endl;
  if (signalptr) {
    std::cout<<"Cost:"<<std::endl;
    std::cout<<signalptr->solution().cost()<<std::endl;
    std::cout<<"Assignment:"<<std::endl;
    std::cout<<signalptr->solution();
  }
  // cleanup and close up stuff here
  // terminate program
  exit(signum);
}
std::vector<Solution> solveNonIP(const Parameters& P) {
  void (*prev_handler_int)(int) = signal(SIGINT, signalHandlerNonIP);
  void (*prev_handler_term)(int) = signal(SIGTERM, signalHandlerNonIP);
  Solution solution(P);
  BranchAndBoundAlt algo(solution, P.delta);
  signalptr = &algo;
  algo.solve();
  return std::vector<Solution>{algo.solution()};
  signalptr = nullptr;
  signal(SIGINT, prev_handler_int);
  signal(SIGTERM, prev_handler_term);
}

void generateBounds(int w) {
  int bounds[16][6];
  for (int k = 0; k < 16; ++k) {
    for (int r = 0; r < 4 && r<=k && r+k+2 <= (1<<w); ++r) {
      Solution initial(k+1, r+1, w);
      BranchAndBoundAlt algo(initial);
      bounds[k][r] = algo.solve();
    }
  }
  for (int k = 0; k < 16; ++k) {
    std::cerr<<"k = "<<k+1<<": ";
    for (int r = 0; r < 4 && r <= k && r+k+2 <= (1<<w); ++r) {
      std::cerr<<bounds[k][r]<<" ";
    }
    std::cerr<<std::endl;
  }
}

int main(int argc, char** argv) {
  //generateBounds(4);
  Parameters P(argc, argv);
  std::vector<Solution> solutions;
  if (Formulation::TypeFromParameters(P) == Formulation::FormulationType::None) {
    solutions = solveNonIP(P);
    std::cout<<"Cost:"<<std::endl;
    std::cout<<solutions.front().cost()<<std::endl;
    std::cout<<"Assignment:"<<std::endl;
    std::cout<<solutions.front()<<std::endl;
    std::cout<<"Optimal"<<std::endl;
  }
  else {
    solutions = first_stage(P);
    Solution best(P);
    for (auto& sol: solutions) {
      auto ssol = second_stage(sol, P, best.cost());
      if (ssol.cost() < best.cost()) {
        best = ssol;
        std::cerr<<"New best solution value: "<<best.cost()<<std::endl;
      }
    }
    std::cout<<"Found solution with cost "<<best.cost()<<std::endl;
    std::cout<<best<<std::endl;
  }
  return 0;
}
