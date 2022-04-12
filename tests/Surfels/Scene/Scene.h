#pragma once

#include "Algorithms/PackedLUT.h"

#include "Core/Color.hpp"

#include "Lighting/DirectionalLight.hpp"
#include "Lighting/PointLight.hpp"

#include "Lighting/Surfel.hpp"
#include "Lighting/SurfelCluster.hpp"
#include "Lighting/SurfelClusterProjection.hpp"

#include "Scene/Geometry/MeshInstance.h"

#include <string>

#define MAX_POINT_LIGHTS 5
#define MAX_MESH_INSTANCES 10

namespace GISurfels {

    /* Scene class that hold the information about what to render and everything */
    class Scene
    {
    public:
        // Types definitions
        // IDK wtf this means yet
        using SubMeshInstancePair = std::pair<ID, ID>;

    public:
        Scene(const std::string& sceneName);

        std::string                             m_SceneName;                        /* The name of the scene                                        */
        size_t                                  m_OcTreeDepth           = 5;        /* The depth of the Oct tree DS                                 */
        float                                   m_DiffuseProbeSpacing   = 1.0f;     /* The spacing between the automatically spaced diffuse probes  */
        float                                   m_SurfelSpacing         = 1.0f;     /* The minimum distance between any 2 surfels                   */
        float                                   m_StaticGeometryArea    = 0.0;      /* The total area of the static geometry Objects                */

        // Lighting information about the scene
        DirectionalLight                        m_DirectionalLight;
        PackedLookupTable<PointLight>           m_PointLights;
        PackedLookupTable<MeshInstance>         m_MeshInstances;

        std::vector<Surfel>                     m_Surfels;
        std::vector<SurfelCluster>              m_SurfelClusters;
        std::vector<SurfelClusterProjection>    m_SurfelClusterProjections;

        std::list<ID>                           m_StaticMeshInstanceIDs;
        std::list<ID>                           m_DynamicMeshInstanceIDs;

        AxisAlignedBox3D                        m_BoundingBox;  // ???? Not generated yet!
        AxisAlignedBox3D                        m_LightBakingVolume; //?????
    };


}
