#include "Random.h"

namespace Raytracing
{
  std::mt19937 Random::s_RandomEngine;
  std::uniform_int_distribution<std::mt19937::result_type> Random::s_Distribution;

  float Random::RandomFloat()
  {
    return (float)s_Distribution(s_RandomEngine) / (float)std::numeric_limits<uint32_t>::max();
  }

  // float Random::RandomFloat(float min, float max)
  // {
  //   return (float)s_Distribution(s_RandomEngine) / (float)std::numeric_limits<uint32_t>::max();
  // }

  glm::vec3 Random::RandomVector()
  {
    return glm::vec3(RandomFloat(), RandomFloat(), RandomFloat());
  }

  glm::vec3 Random::RandomVector(double min, double max)
  {
    return glm::vec3(RandomFloat() * (max - min) + min,
                     RandomFloat() * (max - min) + min,
                     RandomFloat() * (max - min) + min);
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