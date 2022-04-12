#include "Scene.h"

namespace GISurfels {

    Scene::Scene(const std::string& sceneName)
        : m_SceneName(sceneName), m_DirectionalLight(glm::vec3(0.0, -1.0, 0.0), Color(1.0, 1.0), 1.0), m_PointLights(MAX_POINT_LIGHTS), m_MeshInstances(MAX_MESH_INSTANCES) { }
}
