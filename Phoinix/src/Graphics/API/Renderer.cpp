#include "Renderer.h"

namespace Phoinix
{
   Renderer*(*Renderer::CreateFunc)(Window*) = nullptr;
   // Renderer* Renderer::s_Instance = nullptr;
}