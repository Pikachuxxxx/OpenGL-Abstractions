#pragma once

#include "Scene/Geometry/SurfelMesh.h"
#include "Scene/Geometry/SubMesh.h"

#include "Math/AxisAlignedBox3D.h"

#include <tiny_obj_loader.h>

#include <unordered_map>
#include <array>

namespace GISurfels {

    class OBJMeshLoader
    {
    public:
        OBJMeshLoader() = default;

        void loadMesh(const std::string& filePath, const std::string& name, std::vector<SubMesh>& subMeshes, AxisAlignedBox3D& boundingBox);

    private:
        static void vertexCallback(void* userData, float x, float y, float z, float w);
        static void normalCallback(void* userData, float x, float y, float z);
        static void texcoordCallback(void* userData, float x, float y, float z);
        static void indexCallback(void* userData, tinyobj::index_t* indices, int numIndices);
        static void groupCallback(void* userData, const char** names, int numNames);
        static void objectCallback(void* userData, const char* name);
        static void materialCallback(void* userData, const char* name, int material_id);

        void processTriangle(const std::array<tinyobj::index_t, 3>& indices);

        void calculateNormal(const std::array<int32_t, 3>& positionIndices);
        void buildTangentSpace(const std::array<int32_t, 3>& positionIndices, const std::array<int32_t, 3>& texCoordIndices);

        void finalizeSubMeshNormals(SubMesh& subMesh);

        bool wasEmptyGroupOrObjectDetected();

    private:
        // Objects of such type contain averaged normal for a vertex and indices of vertices
        // in current submesh which are sharing this normal
        using SmoothNormalData = std::pair<glm::vec3, std::vector<int32_t>>;

        std::string                                     m_MeshName;
        std::string                                     m_MeshPath;
        std::vector<glm::vec4>                          m_Vertices;
        std::vector<glm::vec3>                          m_Normals;
        std::vector<glm::vec3>                          m_TexCoords;
        std::unordered_map<int32_t, SmoothNormalData>   m_ManualNormals;
        std::vector<SubMesh>*                           m_SubMeshes;
        AxisAlignedBox3D*                               m_BoundingBox;
    };
}

