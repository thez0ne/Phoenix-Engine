#pragma once

#include <random>
#include <glm/vec3.hpp>

namespace Raytracing
{
  class Random
  {
  public:
    static void RandomDouble(); // TODO
    static double RandomDoubleRange(double min, double max);
    static glm::vec3 RandomVector(double min, double max);
    static glm::vec3 RandomUnitVector();
  };
}