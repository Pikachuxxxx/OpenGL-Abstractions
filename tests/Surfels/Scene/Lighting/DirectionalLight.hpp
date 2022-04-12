#pragma once

#include "Light.hpp"

#include <glm/gtc/matrix_transform.hpp>

namespace GISurfels {

    class DirectionalLight : public Light
    {
    public:     
        DirectionalLight(glm::vec3 direction, const Color& color, float area)
        : Light(glm::zero<glm::vec3>(), color, area), m_Direction(direction) {}

        glm::mat4 getViewMatrix()
        {
            glm::vec3 up(0.0f, 1.0f, 0.0f);
            glm::vec3 front(0.0f, 0.0f, 1.0f);

            // When we point on the same dir as up we exchange the Up and Front vectors to avoid gimbal lock by never letting point 2 vectors in the same direction
            float dot = glm::dot(m_Direction, up);
            glm::vec3 reference = (1.0f - fabs(dot)) < 0.01 ? front : up;

            return glm::lookAt(glm::zero<glm::vec3>(), m_Direction, reference);
        }

        glm::vec3 getDirection() { return m_Direction; }
        void setDirection(glm::vec3 dir) { m_Direction = dir; }

    private:
        glm::vec3 m_Direction;
    };

}
