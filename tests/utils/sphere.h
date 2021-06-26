#pragma once


static std::vector<glm::vec3> sphereVertices;
static std::vector<glm::vec3> sphereNormals;
static std::vector<glm::vec2> sphereUVs;
static std::vector<unsigned int> sphereIndices;

void GenerateSphereSmooth(int radius, int latitudes, int longitudes)
{
    if(longitudes < 3)
        longitudes = 3;
    if(latitudes < 2)
        latitudes = 2;

    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> uv;
    std::vector<unsigned int> indices;

    float nx, ny, nz, lengthInv = 1.0f / radius;    // normal
    // Temporary vertex
    struct Vertex
    {
        float x, y, z, s, t; // Postion and Texcoords
    };

    float deltaLatitude = M_PI / latitudes;
    float deltaLongitude = 2 * M_PI / longitudes;
    float latitudeAngle;
    float longitudeAngle;

    // Compute all vertices first except normals
    for (int i = 0; i <= latitudes; ++i)
    {
        latitudeAngle = M_PI / 2 - i * deltaLatitude; /* Starting -pi/2 to pi/2 */
        float xy = radius * cosf(latitudeAngle);    /* r * cos(phi) */
        float z = radius * sinf(latitudeAngle);     /* r * sin(phi )*/

        /*
         * We add (latitudes + 1) vertices per longitude because of equator,
         * the North pole and South pole are not counted here, as they overlap.
         * The first and last vertices have same position and normal, but
         * different tex coords.
         */
        for (int j = 0; j <= longitudes; ++j)
        {
            longitudeAngle = j * deltaLongitude;

            Vertex vertex;
            vertex.x = xy * cosf(longitudeAngle);       /* x = r * cos(phi) * cos(theta)  */
            vertex.y = xy * sinf(longitudeAngle);       /* y = r * cos(phi) * sin(theta) */
            vertex.z = z;                               /* z = r * sin(phi) */
            vertex.s = (float)j/longitudes;             /* s */
            vertex.t = (float)i/latitudes;              /* t */
            vertices.push_back(glm::vec3(vertex.x, vertex.y, vertex.z));
            uv.push_back(glm::vec2(vertex.s, vertex.t));

            // normalized vertex normal
            nx = vertex.x * lengthInv;
            ny = vertex.y * lengthInv;
            nz = vertex.z * lengthInv;
            normals.push_back(glm::vec3(nx, ny, nz));
        }
    }

    /*
     *  Indices
     *  k1--k1+1
     *  |  / |
     *  | /  |
     *  k2--k2+1
     */
    unsigned int k1, k2;
    for(int i = 0; i < latitudes; ++i)
    {
        k1 = i * (longitudes + 1);
        k2 = k1 + longitudes + 1;
        // 2 Triangles per latitude block excluding the first and last longitudes blocks
        for(int j = 0; j < longitudes; ++j, ++k1, ++k2)
        {
            if (i != 0)
            {
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);
            }

            if (i != (latitudes - 1))
            {
                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }
        }
    }

    sphereVertices = vertices;
    sphereNormals = normals;
    sphereUVs = uv;
    sphereIndices = indices;
}
