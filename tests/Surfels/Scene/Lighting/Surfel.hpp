#pragma once

#include "Core/Color.hpp"

namespace GISurfels {

    struct Surfel
    {
        glm::vec3 position;
        glm::vec3 normal;
        Color albedo;
        float area = 0;

        Surfel() = default;
        Surfel(const glm::vec3& position, const glm::vec3& normal, const Color& albedo, float area)
            : position(position), normal(normal), albedo(albedo), area(area) {}
        Surfel(const glm::vec3& position, const glm::vec3& normal) 
            : Surfel(position, normal, Color(1.0, 1.0), 0.0) {}
    };
}