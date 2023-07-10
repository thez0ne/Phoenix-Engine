#include "Random.h"

namespace Raytracing
{
  double Random::RandomDoubleRange(double min, double max)
  {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dist(min, max);
    return dist(gen);
  }
}