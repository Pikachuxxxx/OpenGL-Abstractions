#pragma once

#include "Light.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include <optional>

namespace GISurfels {

    class PointLight : public Light
    {
    public:
        /* Point light fall off in the scene */
        struct Attenuation
        {
            float constant = 1.f;
            float linear = 0.0f;
            float quadratic = 0.0f;

        };

    public:
        Attenuation                 atten;

        PointLight(glm::vec3 position, Color color, float radius, float nearClip, float area, Attenuation atten)
            : Light(position, color, area), m_Radius(radius), m_NearPlane(nearClip) { }

        float getNearPlane() const { return m_NearPlane; }
        float getFarPlane() const { return m_Radius; }
        float gerRadius() const { return m_Radius; }

        glm::mat4 getProjectionMatrix() { return glm::perspective(glm::radians(90.0f), 1.0f, m_NearPlane, m_Radius); }
        glm::mat4 getInverseProjectionMatrix() { return glm::inverse(getProjectionMatrix()); }

        //std::array<glm::mat4, 6> getViewProjectionsFromLightPOV() const
        //{
        //   
        //}

        void setRadius(float radius) { m_Radius = radius; }
        void setNearPlane(float near) { m_NearPlane = near; }

    private:
        float m_Radius = 1.0f;
        float m_NearPlane = 1.0f;

    };
}