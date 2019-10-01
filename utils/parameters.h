#ifndef PARAMETERS_H
#define PARAMETERS_H
#include "cxxopts.hpp"

class Parameters
{
  cxxopts::Options options;
  void parseResult(const cxxopts::ParseResult& result);
  void printHelp();

public:
  int r, c, w, L, firststage, secondstage, timelimit;
  int delta;
  Parameters(int argc, char** argv);
};

#endif // PARAMETERS_H
