#pragma once

#include <random>

namespace Raytracing
{
  class Random
  {
  public:
    static void RandomDouble(); // TODO
    static double RandomDoubleRange(double min, double max);
  };
}