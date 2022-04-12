#include "OBJMeshLoader.h"

#include <fstream>
#include <iostream>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

namespace GISurfels {

    void OBJMeshLoader::loadMesh(const std::string& filePath, const std::string& name, std::vector<SubMesh>& subMeshes, AxisAlignedBox3D& boundingBox)
    {
        m_MeshName = name;
        m_MeshPath = filePath;

        m_SubMeshes = &subMeshes;
        m_BoundingBox = &boundingBox;

        // Init bounding box with reverse limits (IDK why we even do this)
        m_BoundingBox->min = glm::vec3(std::numeric_limits<float>::max());
        m_BoundingBox->max = glm::vec3(-std::numeric_limits<float>::max());


        // Use callback functions to load the mesh, this makes parsing look neat and easy
        tinyobj::callback_t cb;
        cb.vertex_cb    = vertexCallback;
        cb.normal_cb    = normalCallback;
        cb.texcoord_cb  = texcoordCallback;
        cb.index_cb     = indexCallback;
        cb.group_cb     = groupCallback;
        cb.object_cb    = objectCallback;
        cb.usemtl_cb    = materialCallback;

        std::string err;
        std::ifstream ifs(m_MeshPath.c_str());

        if (ifs.fail()) {
            std::cerr << "[GI SURFELS] OBJ model file not found!" << std::endl;
            return;
        }

        bool ret = tinyobj::LoadObjWithCallback(ifs, cb, this, nullptr, &err);

        if (!err.empty()) {
            std::cerr << err << std::endl;
        }

        if (!ret) {
            std::cerr << "[GI SURFELS] Failed to parse .obj" << std::endl;
            return;
        }
    }

    void OBJMeshLoader::vertexCallback(void* userData, float x, float y, float z, float w)
    {
        OBJMeshLoader* thisPtr = reinterpret_cast<OBJMeshLoader*>(userData);

        // Update the bounding boxes
        thisPtr->m_BoundingBox->min.x = std::min(thisPtr->m_BoundingBox->min.x, x);
        thisPtr->m_BoundingBox->min.y = std::min(thisPtr->m_BoundingBox->min.y, y);
        thisPtr->m_BoundingBox->min.z = std::min(thisPtr->m_BoundingBox->min.z, z);
                                                           
        thisPtr->m_BoundingBox->max.x = std::max(thisPtr->m_BoundingBox->max.x, x);
        thisPtr->m_BoundingBox->max.y = std::max(thisPtr->m_BoundingBox->max.y, y);
        thisPtr->m_BoundingBox->max.z = std::max(thisPtr->m_BoundingBox->max.z, z);

        thisPtr->m_Vertices.emplace_back(x, y, z, w);
    }

    void OBJMeshLoader::normalCallback(void* userData, float x, float y, float z)
    {
        OBJMeshLoader* thisPtr = reinterpret_cast<OBJMeshLoader*>(userData);
        thisPtr->m_Normals.emplace_back(x, y, z);
    }

    void OBJMeshLoader::texcoordCallback(void* userData, float x, float y, float z)
    {
        OBJMeshLoader* thisPtr = reinterpret_cast<OBJMeshLoader*>(userData);
        thisPtr->m_TexCoords.emplace_back(x, y, z);
    }

    void OBJMeshLoader::indexCallback(void* userData, tinyobj::index_t* indices, int numIndices)
    {
        tinyobj::index_t* idxs = reinterpret_cast<tinyobj::index_t*>(indices);
        OBJMeshLoader* thisPtr = reinterpret_cast<OBJMeshLoader*>(userData);

        // Triangulate
        tinyobj::index_t i0 = idxs[0];// f[0];
        tinyobj::index_t i1 = { -1, -1, -1 };
        tinyobj::index_t i2 = idxs[1]; //f[1];

        for (size_t k = 2; k < numIndices; k++) {
            i1 = i2;
            i2 = idxs[k];

            std::array<tinyobj::index_t, 3> indices = { i0, i1, i2 };
            thisPtr->processTriangle(indices);
        }
    }

    void OBJMeshLoader::groupCallback(void* userData, const char** names, int numNames)
    {
        OBJMeshLoader* thisPtr = reinterpret_cast<OBJMeshLoader*>(userData);

        if (thisPtr->wasEmptyGroupOrObjectDetected()) { return; };

        if (!thisPtr->m_SubMeshes->empty()) {
            thisPtr->finalizeSubMeshNormals(thisPtr->m_SubMeshes->back());
        }

        thisPtr->m_SubMeshes->emplace_back();
        SubMesh* lastSubMesh = &thisPtr->m_SubMeshes->back();
        if (numNames) {
            lastSubMesh->m_Name = std::string(names[numNames - 1]);
        }

    }

    void OBJMeshLoader::objectCallback(void* userData, const char* name)
    {
        OBJMeshLoader* thisPtr = reinterpret_cast<OBJMeshLoader*>(userData);

        if (thisPtr->wasEmptyGroupOrObjectDetected()) { return; };

        if (!thisPtr->m_SubMeshes->empty()) {
            thisPtr->finalizeSubMeshNormals(thisPtr->m_SubMeshes->back());
        }

        thisPtr->m_SubMeshes->emplace_back();
        SubMesh* lastSubMesh = &thisPtr->m_SubMeshes->back();
        if (name) {
            lastSubMesh->m_Name = (std::string(name));
        }

    }

    void OBJMeshLoader::materialCallback(void* userData, const char* name, int material_id)
    {
        OBJMeshLoader* thisPtr = reinterpret_cast<OBJMeshLoader*>(userData);

        auto& lastSubMesh = thisPtr->m_SubMeshes->back();
        if (name) {
            lastSubMesh.m_MaterialName = (name);
        }

    }

    static int32_t fixIndex(int32_t idx, int32_t n)
    {
        if (idx > 0) return idx - 1;
        if (idx == 0) return 0;
        return n + idx;  // negative value = relative
    }

    void OBJMeshLoader::processTriangle(const std::array<tinyobj::index_t, 3>& indices)
    {
        std::array<int32_t, 3> positionIndices;
        std::array<int32_t, 3> texCoordIndices;
        bool shouldBuildTangent = false;
        bool shouldCalculateNormal = false;

        const int32_t faceVertexCount = 3;

        SubMesh& lastSubMesh = m_SubMeshes->back();

        for (int32_t i = 0; i < faceVertexCount; i++) {
            int32_t fixedVIdx = fixIndex(indices[i].vertex_index, static_cast<int32_t>(m_Vertices.size()));
            int32_t fixedNIdx = fixIndex(indices[i].normal_index, static_cast<int32_t>(m_Normals.size()));
            int32_t fixedTIdx = fixIndex(indices[i].texcoord_index, static_cast<int32_t>(m_TexCoords.size()));

            bool isTexCoordPresent = indices[i].texcoord_index != 0;
            bool isNormalPresent = indices[i].normal_index != 0;

            lastSubMesh.addVertex(Vertex1P1N2UV1T1BT(m_Vertices[fixedVIdx],
                isTexCoordPresent ? m_TexCoords[fixedTIdx] : glm::vec3(),
                isTexCoordPresent ? m_TexCoords[fixedTIdx] : glm::vec2(),
                isNormalPresent ? m_Normals[fixedNIdx] : glm::vec3()));

            shouldCalculateNormal = !isNormalPresent;
            shouldBuildTangent = isTexCoordPresent;

            positionIndices[i] = fixedVIdx;
            texCoordIndices[i] = fixedTIdx;
        }

        if (shouldCalculateNormal) {
            calculateNormal(positionIndices);
        }

        if (shouldBuildTangent) {
            buildTangentSpace(positionIndices, texCoordIndices);
        }
    }

    void OBJMeshLoader::calculateNormal(const std::array<int32_t, 3>& positionIndices)
    {
        glm::vec3 edge1 = m_Vertices[positionIndices[1]] - m_Vertices[positionIndices[0]];
        glm::vec3 edge2 = m_Vertices[positionIndices[2]] - m_Vertices[positionIndices[0]];
        glm::vec3 surfaceNormal = glm::cross(edge1, edge2);

        const int32_t faceVertexCount = 3;

        SubMesh& lastSubMesh = m_SubMeshes->back();

        for (int32_t i = 0; i < faceVertexCount; i++) {
            auto& normalData = m_ManualNormals[positionIndices[i]];
            normalData.first += surfaceNormal;
            // Indicate that [submeshVertexIndex] vertex should receive averaged normal afterwards (normalData.first)
            int32_t submeshVertexIndex = static_cast<int32_t>(lastSubMesh.m_Vertices.size()) - (faceVertexCount - i);
            normalData.second.emplace_back(submeshVertexIndex);
        }
    }

    void OBJMeshLoader::buildTangentSpace(const std::array<int32_t, 3>& positionIndices, const std::array<int32_t, 3>& texCoordIndices)
    {
        glm::vec3 edge1 = m_Vertices[positionIndices[1]] - m_Vertices[positionIndices[0]];
        glm::vec3 edge2 = m_Vertices[positionIndices[2]] - m_Vertices[positionIndices[0]];
        glm::vec2 deltaUV1 = m_TexCoords[texCoordIndices[1]] - m_TexCoords[texCoordIndices[0]];
        glm::vec2 deltaUV2 = m_TexCoords[texCoordIndices[2]] - m_TexCoords[texCoordIndices[0]];

        float f = 1.f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

        glm::vec3 tangent;
        tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
        tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
        tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

        glm::vec3 bitangent;
        bitangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
        bitangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
        bitangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);

        // Update tangent vectors for latest vertices
        auto size = m_SubMeshes->back().m_Vertices.size();
        for (int32_t i = 1; i <= 3; i++) {
            auto& vertex = m_SubMeshes->back().m_Vertices[size - i];
            vertex.tangent = tangent;
            vertex.bitangent = bitangent;
        }
    }

    void OBJMeshLoader::finalizeSubMeshNormals(SubMesh& subMesh)
    {
        // Check if manual normal calculation took place in triangle callback
        if (m_ManualNormals.size()) {
            for (auto& keyObjectPair : m_ManualNormals) {
                glm::vec3 normal = glm::normalize(keyObjectPair.second.first);
                std::vector<int32_t>& vertexIndices = keyObjectPair.second.second;
                for (auto submeshVertexIdx : vertexIndices) {
                    subMesh.m_Vertices[submeshVertexIdx].normal = normal;
                }
            }
            m_ManualNormals.clear();
        }
    }

    bool OBJMeshLoader::wasEmptyGroupOrObjectDetected()
    {
        if (m_SubMeshes->size() < 1) { return false; };
        auto lastSubmeshIdx = m_SubMeshes->size() - 1;
        return m_SubMeshes->at(lastSubmeshIdx).m_Vertices.size() == 0;
    }
}
