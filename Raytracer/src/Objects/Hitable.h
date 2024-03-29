#pragma once

#include <optional>
#include <string>
#include <vector>

#include "HitInformation.h"
#include "../Ray.h"

namespace Raytracing
{
  class Hitable
  {
  public:
    // TODO do i need a min and max t like rt in weekend?
    virtual std::optional<HitInformation> Hit(const Ray& ray) = 0;
    virtual void RenderOptions(int id, const std::vector<std::string>& matList) = 0;
  };
}
