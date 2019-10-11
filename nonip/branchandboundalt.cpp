#include "branchandboundalt.h"
#include <queue>
#include <algorithm>
#include <csignal>
#include "utils/galois_stuff.h"

using triplet = std::pair<int, std::pair<int, int>>;

int BranchAndBoundAlt::solveEncoder(int col) {
  if (col == m_incumbentSolution.rows) {
    findOptimalColumn(col);
    m_currentSolution = m_incumbentSolution;
    m_bestCost = m_incumbentSolution.cost();
    std::cerr<<"New best solution: "<<m_bestCost<<std::endl;
    std::cerr<<m_currentSolution<<std::endl;
    return m_bestCost;
  }

  for (int e = m_incumbentSolution.encodersRow[col-1]+1; e < m_incumbentSolution.maxInt; ++e) {
    m_incumbentSolution.encodersRow[col] = e;
    m_present[e] = true;
    solveMultiplier(col);
    m_present[e] = false;
  }

  return m_bestCost;
}

int BranchAndBoundAlt::solveMultiplier(int col) {
  std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>, std::greater<std::pair<int, int>> > mHeap;

  for (int m = 1; m < m_incumbentSolution.maxInt; ++m) {
    m_incumbentSolution.multipliersRow[col] = m;
    int cost = findOptimalColumn(col+1);
    mHeap.push(std::make_pair(cost, m));
  }

  int colsLeft = m_incumbentSolution.rows - col-1;
  int bound = findBound(colsLeft, m_incumbentSolution.rows, m_incumbentSolution.bits);
  int best = mHeap.top().first;

  while (!mHeap.empty()) {
    std::pair<int, int> top = mHeap.top();
    if (col == 1) {
      std::cerr<<"Now considering the second multiplier to be "<<top.second<<std::endl;
    }
    if (top.first + bound >= m_bestCost) return m_bestCost;
    m_incumbentSolution.multipliersRow[col] = top.second;
    solveEncoder(col+1);
    mHeap.pop();
  }
  return m_bestCost;
}

int BranchAndBoundAlt::solve()
{

  std::cerr<<"Running Branch and Bound Algorithm (Alternative) with delta "<<m_delta<<std::endl;
  m_incumbentSolution.encodersRow[0] = 0;
  m_present[0] = true;
  m_incumbentSolution.multipliersRow[0] = 1;
  if (this->m_incumbentSolution.rows == 1) {
    findOptimalColumn(1);
    m_currentSolution = m_incumbentSolution;
    m_bestCost = m_currentSolution.cost();
    return m_bestCost;
  }
  m_incumbentSolution.encodersRow[1] = 1;
  m_present[1] = true;
  return solveMultiplier(1);
}

Solution BranchAndBoundAlt::solution() const
{
  return m_currentSolution;
}

int BranchAndBoundAlt::findBound(int r, int k, int w)
{
  static std::vector<std::vector<int>> bounds_for_4 = {
    {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0},
    {   0,   4,   8,  12,  16,  20,  24,  28,  32,  36,  40,  44,  48,  52,  56,  60},
    {   0,   0,  17,  26,  36,  46,  57,  68,  80,  92, 105, 118, 131, 144, 158,   0},
    {   0,   0,   0,  43,  58,  75,  92, 112, 130, 151, 171, 191, 213, 235,   0,   0}
  };
  static std::vector<std::vector<int>> bounds_for_5 = {
    {0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0},
    {0,   5,  10,  15,  20,  25,  30,  35,  40,  45,  50,  55,  60,  65,  70,  75,  80,  85,  90,  95, 100, 105, 110, 115, 120, 125, 130, 135, 140, 145, 150, 155},
    {0,   0,  21,  32,  44,  56,  69,  82,  96, 110, 125, 140, 156, 172, 188, 204, 221, 238, 255, 272, 289, 306, 324, 342, 360, 378, 397, 416, 435, 454, 474,   0},
    {0,   0,   0,  52,  71,  91, 112, 133, 155, 178, 203, 229, 255, 282, 309, 336, 364, 392, 420, 448, 476, 505, 534, 563, 592, 621, 650, 680, 710, 741,   0,   0}
  };
  static std::vector<std::vector<int>> bounds_for_6 = {
    {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0},
    {   0,   6,  12,  18,  24,  30,  36,  42,  48,  54,  60,  66,  72,  78,  84,  90,  96, 102, 108, 114, 120, 126, 132, 138, 144, 150, 156, 162, 168, 174, 180, 186, 192, 198, 204, 210, 216, 222, 228, 234, 240, 246, 252, 258, 264, 270, 276, 282, 288, 294, 300},
    {   0,   0,  25,  38,  52,  66,  81,  96, 112, 128, 145, 162, 180, 198, 217, 236, 255, 274, 293, 312, 331, 350, 370, 390, 410, 430, 451, 472, 493, 514, 536, 558, 580, 602, 624, 646, 668, 690, 713, 736, 759, 782, 805, 828, 851, 874, 898, 922, 946, 970, 995},
    {   0,   0,   0,  63,  86, 115, 144, 175, 205, 234, 263, 294, 324, 354, 384, 416, 448, 481, 514, 548, 581, 615, 649, 683, 718, 753, 788, 823, 859, 895, 931, 967,1003,1039,1076,1113,1150,1187,1225,1263,1301,1340,1378,1416,1454,1493,1532,1571,1610,1650,1690}
  };
  static std::vector<std::vector<int>> bounds_for_7 = {
    {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0},
    {   0,   7,  14,  21,  28,  35,  42,  49,  56,  63,  70,  77,  84,  91,  98, 105, 112, 119, 126, 133, 140, 147, 154, 161, 168, 175, 182, 189, 196, 203, 210, 217, 224, 231, 238, 245, 252, 259, 266, 273, 280, 287, 294, 301, 308, 315, 322, 329, 336, 343, 350},
    {   0,   0,  29,  44,  60,  76,  93, 110, 128, 146, 165, 184, 204, 224, 245, 266, 288, 310, 332, 354, 377, 400, 423, 446, 469, 492, 515, 538, 561, 584, 607, 630, 654, 678, 702, 726, 750, 774, 798, 822, 847, 872, 897, 922, 947, 972, 997,1022,1048,1074,1100},
    {   0,   0,   0,  74, 102, 134, 165, 198, 231, 265, 299, 334, 369, 405, 442, 479, 517, 555, 593, 633, 672, 713, 754, 794, 833, 875, 917, 959,1001,1044,1087,1129,1171,1214,1257,1300,1344,1387,1431,1474,1518,1562,1605,1648,1691,1735,1778,1821,1865,1909,1953}
  };
  static std::vector<std::vector<int>> bounds_for_8 = {
    {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0},
    {   0,   8,  16,  24,  32,  40,  48,  56,  64,  72,  80,  88,  96, 104, 112, 120, 128, 136, 144, 152, 160, 168, 176, 184, 192, 200, 208, 216, 224, 232, 240, 248, 256, 264, 272, 280, 288, 296, 304, 312, 320, 328, 336, 344, 352, 360, 368, 376, 384, 392, 400},
    {   0,   0,  35,  54,  76,  98, 123, 148, 174, 200, 227, 254, 281, 308, 335, 362, 390, 418, 446, 474, 502, 530, 558, 586, 614, 642, 670, 698, 727, 756, 785, 814, 843, 872, 901, 930, 960, 990,1020,1050,1080,1110,1140,1170,1201,1232,1263,1294,1325,1356,1387},
    {   0,   0,   0,  74, 102, 134, 165, 198, 231, 265, 299, 334, 369, 405, 442, 479, 517, 555, 593, 633, 672, 713, 754, 794, 833, 875, 917, 959,1001,1044,1087,1129,1171,1214,1257,1300,1344,1387,1431,1474,1518,1562,1605,1648,1691,1735,1778,1821,1865,1909,1953}
  };
  static std::vector<std::vector<std::vector<int>>*> bounds = {
    nullptr, nullptr, nullptr, nullptr, &bounds_for_4, &bounds_for_5, &bounds_for_6, &bounds_for_7, &bounds_for_8
  };
  std::vector<std::vector<int>>* bound = bounds[w];

  int value = 0;
  while (r >= 3) {
    value += bound->at(3)[k];
    r -= 3;
  }
  value += bound->at(r)[k];
  return value;
}

// Finds optimal column for a completed row of m_incumbentSolution
int BranchAndBoundAlt::findOptimalColumn(int toCol)
{
  std::priority_queue<triplet, std::vector<triplet>> bestRows;
  for (int i = (this->m_incumbentSolution.rows >1)?2:1; i < m_incumbentSolution.maxInt; ++i) {
    // Check if i was used in row assignment.
    if (m_present[i]) continue;
    // Find the best multiplier for encoder i
    int bestCost = std::numeric_limits<int>::max();
    std::pair<int, int> bestRow = std::make_pair(1, i);
    for (int m = 1; m < m_incumbentSolution.maxInt; ++m) {
      int cost = 0;
      for (int col = 0; col < toCol; ++col) {
        cost += get_cij_extended(
          m_incumbentSolution.encodersRow[col], i,
          m_incumbentSolution.multipliersRow[col], m,
          m_incumbentSolution.bits
        );
      }
      if (cost < bestCost) {
        bestRow.first = m;
        bestCost = cost;
      }
    }
    bestRows.push(std::make_pair(bestCost, bestRow));
    // Keep queue limited to size k
    if (static_cast<int>(bestRows.size()) > m_currentSolution.cols) bestRows.pop();
  }

  int cost = 0;
  for (int row = 0; row < m_currentSolution.cols; ++row) {
    auto crow = bestRows.top();
    m_incumbentSolution.multipliersCol[row] = crow.second.first;
    m_incumbentSolution.encodersCol[row] = crow.second.second;
    cost += crow.first;
    bestRows.pop();
  }
  return cost;
}

BranchAndBoundAlt::BranchAndBoundAlt(const Solution &initial, int delta):
  m_bestCost(std::numeric_limits<int>::max()), m_delta(delta),
  m_currentSolution(initial), m_incumbentSolution(initial)
{
  m_bestCost = initial.cost();
  m_present.resize(m_incumbentSolution.maxInt, false);
}
