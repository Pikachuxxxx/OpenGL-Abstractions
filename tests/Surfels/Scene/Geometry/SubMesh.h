#pragma once

#include "Math/AxisAlignedBox3D.h"
#include "Vertices.h"

#include <string>
#include <vector>

namespace GISurfels { 

    class SubMesh
    {
    public:
        SubMesh() = default;

        void addVertex(const Vertex1P1N2UV1T1BT& vertex);

        std::string                     m_Name;
        std::string                     m_MaterialName;
        std::vector<Vertex1P1N2UV1T1BT> m_Vertices;
        AxisAlignedBox3D                m_BoundingBox = AxisAlignedBox3D::MaximumReversed();
        float                           m_Area = 0.0f;
    };
}

