#pragma once

#include "Algorithms/PackedLUT.h"
#include "Math/AxisAlignedBox3D.h"
#include "SubMesh.h"

#include "Utility/OBJMeshLoader.h"

#include <string>

#define MAX_MESHES_CAPACITY 500

namespace GISurfels {

    /* A mesh that has enough information so that we can easily generate surfels onto it */
    class SurfelMesh
    {
    public:
        SurfelMesh(const std::string& filePath, const std::string& name);

        std::string                 m_Name;
        Transformation              m_BaseTransformation;
        AxisAlignedBox3D            m_BoundingBox;
        PackedLookupTable<SubMesh>  m_SubMeshes;
    };
}

