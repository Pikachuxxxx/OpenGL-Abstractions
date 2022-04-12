#pragma once

#include <glm/glm.hpp>

#include "Scene/Geometry/Transformation.hpp"

namespace GISurfels {

     /**
     1 position
     1 normal
     1 texture coordinate
     1 light map uv
     */
    struct Vertex1P1N2UV {
        glm::vec4 position;
        glm::vec3 textureCoords;
        glm::vec2 lightmapCoords;
        glm::vec3 normal;

        Vertex1P1N2UV(const glm::vec4& position, const glm::vec3& texCoords, const glm::vec2& lightmapCoords, const glm::vec3& normal)
            : position(position), textureCoords(texCoords), lightmapCoords(lightmapCoords), normal(normal) { }
    };

    /**
     1 position
     1 normal
     2 uv channels (tex coords + lightmap coords)
     1 tangent vector
     1 bitangent vector
     */
    struct Vertex1P1N2UV1T1BT : Vertex1P1N2UV {
        glm::vec3 tangent;
        glm::vec3 bitangent;

        using Vertex1P1N2UV::Vertex1P1N2UV;

        Vertex1P1N2UV1T1BT(const glm::vec4& position, const glm::vec3& texcoords, const glm::vec2& lightmapCoords, const glm::vec3& normal, const glm::vec3& tangent, const glm::vec3& bitangent)
            : Vertex1P1N2UV(position, texcoords, lightmapCoords, normal), tangent(tangent), bitangent(bitangent) { }

        Vertex1P1N2UV1T1BT(const glm::vec4& position)
            : Vertex1P1N2UV(position, glm::vec3(0.0), glm::vec2(0.0), glm::vec3(0.0)), tangent(glm::vec3(0.0)), bitangent(glm::vec3(0.0)) { }

        Vertex1P1N2UV1T1BT transformedBy(const Transformation& t)
        {
            glm::mat4 modelMatrix = t.modelMatrix();
            glm::mat4 normalMatrix = glm::transpose(glm::inverse(modelMatrix));

            return Vertex1P1N2UV1T1BT(modelMatrix * position,
                textureCoords,
                lightmapCoords,
                normalMatrix * glm::vec4(normal, 1.0),
                normalMatrix * glm::vec4(tangent, 1.0),
                normalMatrix * glm::vec4(bitangent, 1.0));
        }
    };
}