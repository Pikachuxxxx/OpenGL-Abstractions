#include "SubMesh.h"

#include "Math/Triangle3D.h"

namespace GISurfels {

    void SubMesh::addVertex(const Vertex1P1N2UV1T1BT& vertex)
    {
        // This might turn out wrong!
        m_BoundingBox.min = glm::min(glm::vec3(vertex.position), m_BoundingBox.min);
        m_BoundingBox.max = glm::max(glm::vec3(vertex.position), m_BoundingBox.max);
        m_Vertices.push_back(vertex);

        if ((m_Vertices.size() % 3) == 0) {
            size_t count = m_Vertices.size();
            auto& v0 = m_Vertices[count - 3];
            auto& v1 = m_Vertices[count - 2];
            auto& v2 = m_Vertices[count - 1];

            Triangle3D triangle(v0.position, v1.position, v2.position);
            m_Area += triangle.area();
        }
    }
}