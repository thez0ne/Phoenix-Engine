#pragma once
#include "pch.h"

#include "Buffer/VertexBuffer.h"
#include "Buffer/IndexBuffer.h"

namespace Phoinix
{
   class Mesh
   {
   public:
      // Mesh();
      Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);
      Mesh(const Mesh& mesh);

      virtual ~Mesh();
   private:
      // VertexBuffer m_VertexBuffer;
      // IndexBuffer m_IndexBuffer;
      // TODO add material

      std::vector<Vertex> m_Vertices;
      std::vector<uint32_t> m_Indices;
   };
}