#include "Sphere.h"

namespace Raytracing
{
  Sphere::Sphere(const glm::vec3& position, float radius, glm::vec4 colour) :
    m_Position(position), m_Radius(radius), m_Colour(colour)
  {}

  std::optional<HitInformation> Sphere::Hit(const Ray& ray)
  {
    auto a = glm::dot(ray.dir, ray.dir);
    auto b = 2.0 * glm::dot(ray.origin - m_Position, ray.dir);
    auto c = glm::dot(ray.origin - m_Position, ray.origin - m_Position) - m_Radius * m_Radius;

    auto discriminant = b * b - 4.0f * a * c;

    if (discriminant < 0)
    {
      return std::nullopt;
    }

    // TODO figure out which hit actually is the closest point to camera
    float t1 = (-b - glm::sqrt(discriminant)) / (2.0f * a);
    float t2 = (-b + glm::sqrt(discriminant)) / (2.0f * a);

    // origin + dir * t -> coord of hit
    glm::vec3 h1 = ray.origin + ray.dir * t1;
    glm::vec3 h2 = ray.origin + ray.dir * t2;

    glm::vec3 normal = glm::normalize(h1 - m_Position);

    HitInformation result{};
    result.position = h1;
    result.normal = normal;
    result.colour = m_Colour;
    result.t = t1;
    result.hitDistance = glm::distance(h1, ray.origin);

    return result;
  }
}
