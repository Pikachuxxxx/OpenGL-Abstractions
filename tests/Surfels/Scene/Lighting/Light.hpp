#pragma once

#include "Core/Color.hpp"

namespace GISurfels {

    /* Any light in the scene will be derived from this (Directional or Point) */
    class Light
    {
    public:

        // The type of the light
        enum class Type
        {
            Direcitional = 0,
            Point = 1
        };

    public:
        Light() = default;
        Light(glm::vec3 position, const Color& color, float area)
            : m_Position(position), m_Color(color), m_Area(area) {}

        glm::vec3 getPosition() { return m_Position; }
        Color getColor() { return m_Color; }
        float getArea() { return m_Area; }
        bool isEnabled() { return m_IsEnabled; }

    protected:
        glm::vec3   m_Position;
        Color       m_Color;
        float       m_Area;
        bool        m_IsEnabled;

    };

}
