#pragma once
#include "pch.h"

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>

namespace Phoinix
{
    struct Vertex
    {
        glm::vec2 pos;
        glm::vec3 colour;

        static VkVertexInputBindingDescription GetBindingDescription()
        {
            VkVertexInputBindingDescription bindingDescription{};
            bindingDescription.binding = 0;
            bindingDescription.stride = sizeof(Vertex);
            // NOTE: VK_VERTEX_INPUT_RATE_INSTANCE used if we doing instanced rendering
            bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
            return bindingDescription;
        }

        static std::array<VkVertexInputAttributeDescription, 2> GetAttributeDescription()
        {
            std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};

            // Vertex Position
            attributeDescriptions[0].binding = 0;
            attributeDescriptions[0].location = 0;
            /* NOTE Formats
                float   -> VK_FORMAT_R32_SFLOAT
                vec2    -> VK_FORMAT_R32G32_SFLOAT
                vec3    -> VK_FORMAT_R32G32B32_SFLOAT
                vec4    -> VK_FORMAT_R32G32B32A32_SFLOAT
                ivec2   -> VK_FORMAT_R32G32_SINT
                uvec4   -> VK_FORMAT_R32G32B32A32_UINT
                double  -> VK_FORMAT_R64_SFLOAT
                ...
            */
            attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
            attributeDescriptions[0].offset = offsetof(Vertex, pos);

            // Vertex Colour
            attributeDescriptions[1].binding = 0;
            attributeDescriptions[1].location = 1;
            attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[1].offset = offsetof(Vertex, colour);

            return attributeDescriptions;
        }
    };
}