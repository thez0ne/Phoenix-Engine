#include "Mesh.h"

// TODO

namespace Phoenix
{
  Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices) :
    m_Vertices(vertices), m_Indices(indices)
  {
    // m_VertexBuffer;
  }

  // Mesh::Mesh(const Mesh& mesh) :
  //    m_VertexBuffer(mesh.m_VertexBuffer), m_IndexBuffer(mesh.m_IndexBuffer)
  // {}
}
