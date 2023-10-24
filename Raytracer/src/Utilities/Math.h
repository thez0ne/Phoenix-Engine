#pragma once

namespace Raytracing
{
  namespace Math
  {
    float remap(float input, float start, float end, float newStart, float newEnd)
    {
      float output = newStart + ((newEnd - newStart) / (end - start)) * (input - start);
      return output;
    }
  }
}
