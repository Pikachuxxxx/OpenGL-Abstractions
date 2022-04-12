#include "MeshInstance.h"

namespace GISurfels {

    MeshInstance::MeshInstance(ID id, SurfelMesh& mesh)
        : m_MeshID(id)
    {
        m_Transformation = mesh.m_BaseTransformation;
        m_ModelMatrix = m_Transformation.modelMatrix();
    }

    AxisAlignedBox3D MeshInstance::boundingBox(const SurfelMesh& mesh) const
    {
        return mesh.m_BoundingBox.transformedBy(m_Transformation);
    }

    std::optional<MaterialReference> MeshInstance::materialReferenceForSubMeshID(ID subMeshID) const
    {
        if (m_SubMeshMaterialMap.find(subMeshID) == m_SubMeshMaterialMap.end()) {
            return std::nullopt;
        }
        return m_SubMeshMaterialMap.at(subMeshID);
    }
}
