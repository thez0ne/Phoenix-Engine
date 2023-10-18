#pragma once

#include <random>
#include <glm/vec3.hpp>

namespace Raytracing
{
  class Random
  {
  public:
    static void Init() { s_RandomEngine.seed(std::random_device()()); }

    static float RandomFloat();
    static float RandomFloat(float min, float max); // TODO
    static glm::vec3 RandomVector();
    static glm::vec3 RandomVector(double min, double max);
    static glm::vec3 RandomUnitVector();

  private:
    static std::mt19937 s_RandomEngine;
    static std::uniform_int_distribution<std::mt19937::result_type> s_Distribution;
  };
}