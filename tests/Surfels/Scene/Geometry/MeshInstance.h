#pragma once

#include "Algorithms/PackedLUT.h"

#include "SurfelMesh.h"

#include <unordered_map>
#include <optional>

namespace GISurfels {

    enum class MaterialType : uint8_t
    {
        CookTorrance = 0, Emissive = 1
    };

    using MaterialReference = std::pair<MaterialType, ID>;

    /* An instance of the mesh in the scene that has a material and encapsulates its sub meshes and their materials info as well */
    class MeshInstance
    {
    public:
        std::optional<MaterialReference>            materialReference;

    public:
        MeshInstance(ID id, SurfelMesh& mesh);

        AxisAlignedBox3D boundingBox(const SurfelMesh& mesh) const;
        std::optional<MaterialReference> materialReferenceForSubMeshID(ID subMeshID) const;

        ID                                          m_MeshID;
        bool                                        m_IsSelected    = false;
        bool                                        m_IsHighlighted = false;
        Transformation                              m_Transformation;
        glm::mat4                                   m_ModelMatrix;
        std::unordered_map<ID, MaterialReference>   m_SubMeshMaterialMap;       // ID is the submesh ID and material reference is the type of mat and the ID of the actual of the material with set values
    };
}

