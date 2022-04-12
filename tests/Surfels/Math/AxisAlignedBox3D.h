#pragma once

#include "Surfels/Scene/Geometry/Transformation.hpp"

#include <glm/glm.hpp>
#include <glm/mat4x4.hpp>

#include <array>

namespace GISurfels {

    // FOrward declaration
    struct Triangle3D;

    class AxisAlignedBox3D
    {
    public:
        glm::vec3 min = glm::zero<glm::vec3>();
        glm::vec3 max = glm::zero<glm::vec3>();

    public:
        AxisAlignedBox3D() = default;
        AxisAlignedBox3D(glm::vec3 min, glm::vec3 max);

        static const AxisAlignedBox3D& Zero();
        static const AxisAlignedBox3D& Unit();
        static const AxisAlignedBox3D& NDC();
        static const AxisAlignedBox3D& MaximumReversed();
        //static const AxisAlignedBox3D& MaximumReversed();

        /* Returns the length of the diagonal */
        float diagonal();
        /* Returns the box as orthographic projection matrix */
        glm::mat4 asFrustum();
        //glm::mat4 localSpaceMatrix() const;
        std::array<glm::vec4, 8> cornerPoints() const;
        float smallestDimensionLength() const;
        float largestDimensionLength() const;
        glm::vec3 center() const;
        bool contains(const glm::vec3 &point) const;
        bool contains(const Triangle3D &triangle) const;
        bool contains(const AxisAlignedBox3D &box) const;

        /**
         Splits into 8 sub-boxes of equal size
         The order is:
          Bottom-Front-Left
          Bottom-Front-Right
          Top-Front-Left
          Top-Front-Right
          Bottom-Back-Left
          Bottom-Back-Right
          Top-Back-Left
          Top-Back-Right
         @return array of sub-boxes
         */
        std::array<AxisAlignedBox3D, 8> octet() const;
        AxisAlignedBox3D transformedBy(const Transformation &t) const;
        AxisAlignedBox3D transformedBy(const glm::mat4 &m) const;
    };
}

