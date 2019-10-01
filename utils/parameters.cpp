#include "parameters.h"

void Parameters::parseResult(const cxxopts::ParseResult& result)
{
  if (result.count("help")) {
    printHelp();
    exit(0);
  }
  auto required = std::vector<std::string> {
    "bits", "redundant", "original", "formulation"
  };
  for (auto parameter: required)
    if (!result.count(parameter)) {
      std::cerr<<"Missing option ‘"<<parameter<<"’"<<std::endl;
      printHelp();
      exit(0);
    }
  w = result["bits"].as<int>();
  c = result["original"].as<int>();
  r = result["redundant"].as<int>();
  firststage = result["formulation"].as<int>();
  secondstage = result["secondstage"].as<int>();
  timelimit = result["timelimit"].as<int>();
  L = result["nofsol"].as<int>();
  delta = result["delta"].as<int>();

  if (c+r > (1<<w)) {
    std::cerr<<"Not enough distinct finite field elements (original + redundant > 2^bits)."<<std::endl;
    exit(0);
  }
}

void Parameters::printHelp()
{
  std::cout << options.help({"General", "Problem", "Solving"}) << std::endl;
}

Parameters::Parameters(int argc, char** argv):
  options("./run", "TODO: Add a description")
{
  options.add_options("General")
    ("h,help", "Print help")
  ;
  options.add_options("Problem")
    ("w,bits", "Number of bits in a byte.", cxxopts::value<int>())
    ("r,redundant", "Number of columns in encoding matrix (i.e, number of bytes added to the message).", cxxopts::value<int>())
    ("k,original", "Number of rows in enconding matrix (i.e, number of bytes in original message).", cxxopts::value<int>())
  ;
  options.add_options("Solving")
    ("f,formulation", "Type of algorithm (first stage) to solve.", cxxopts::value<int>())
    ("s,secondstage", "Type of second stage to solve.", cxxopts::value<int>()->default_value("0"))
    ("t,timelimit", "First-stage solver time limit.", cxxopts::value<int>()->default_value("600"))
    ("L,nofsol", "Number of solutions to solve the second stage for.", cxxopts::value<int>()->default_value("1"))
    ("d,delta", "delta value for Branch and Bound algorithm", cxxopts::value<int>()->default_value("0"))
  ;
  try {
    parseResult(options.parse(argc, argv));
  }
  catch (cxxopts::OptionException e) {
    std::cout<<e.what()<<std::endl;
    printHelp();
    exit(0);
  }
}
