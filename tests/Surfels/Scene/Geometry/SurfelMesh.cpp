#include "SurfelMesh.h"


namespace GISurfels {

    SurfelMesh::SurfelMesh(const std::string& filePath, const std::string& name) 
        : m_SubMeshes(MAX_MESHES_CAPACITY), m_Name(name)
    {
        auto meshLoader = new OBJMeshLoader;

        std::vector<SubMesh> subMeshes;

        meshLoader->loadMesh(filePath, name, subMeshes, m_BoundingBox);

        for (auto& subMesh : subMeshes) {
            m_SubMeshes.emplace(std::move(subMesh));
        } 
        // IDK why I'm scaling this
        float scaleDown = m_BoundingBox.diagonal() * 1.44;
        m_BaseTransformation.scale = glm::vec3(1.0 / scaleDown);
    }
}