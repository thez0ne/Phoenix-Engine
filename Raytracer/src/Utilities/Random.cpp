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

  glm::vec3 Random::RandomVector(double min, double max)
  {
    return glm::vec3(
      RandomDoubleRange(min, max), RandomDoubleRange(min, max), RandomDoubleRange(min, max));
  }

  glm::vec3 Random::RandomUnitVector()
  {
    while (true)
    {
      auto v = RandomVector(-1, 1);
      if (v.length() * v.length() >= 1)
      {
        continue;
      }
      return v;
    }
  }
}