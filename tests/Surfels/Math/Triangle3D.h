#pragma once

#include "Triangle.hpp"

namespace GISurfels {

    struct AxisAlignedBox3D;

    struct Triangle3D : public Triangle<glm::vec3>
    {
        // Exposes the default constructor to current implementation by tellling the template type
        using Triangle<glm::vec3>::Triangle;

        float area() const override;

        AxisAlignedBox3D boundingBox() const;
        glm::vec3 normal() const;
        std::array<Triangle3D, 4> split() const;
    };
}
