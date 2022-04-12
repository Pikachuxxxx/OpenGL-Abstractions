#pragma once

#include <glm/glm.hpp>

namespace GISurfels {

    struct Color {
    public:
        enum class Space {
            Linear, sRGB, YCoCg
        };

    public:
        Color() = default;

        Color(float red, float green, float blue, float alpha, Space space = Space::Linear)
            : mR(red), mG(green), mB(blue), mA(alpha), mSpace(space) {}

        Color(float red, float green, float blue, Space space = Space::Linear)
            :  Color(red, green, blue, 1.0, space) {}

        Color(float white, float alpha, Space space = Space::Linear)
             : Color(white, white, white, alpha, space) {}

        Color(float white, Space space = Space::Linear)
            : Color(white, white, white, 1.0, space) {}

        static const Color &White()
        {
            static Color color(1.0f, 1.0f, 1.0f, 1.0f);
            return color;
        }

        static const Color &Black()
        {
            static Color color(0.0f, 1.0f);
            return color;
        }

        static const Color &Gray()
        {
            static Color color(0.3f, 1.0f);
            return color;
        }

        static const Color &Red()
        {
            static Color color(1.0f, 0.0f, 0.0f, 1.0f);
            return color;
        }

        static const Color &Green()
        {
            static Color color(0.0f, 1.0f, 0.0f, 1.0f);
            return color;
        }

        static const Color &Blue()
        {
            static Color color(0.0f, 0.0f, 1.0f, 1.0f);
            return color;
        }

        float r() const
        {
            return mR;
        }

        float g() const
        {
            return mG;
        }

        float b() const
        {
            return mB;
        }

        float a() const
        {
            return mA;
        }

        glm::vec3 rgb() const
        {
            return glm::vec3(mR, mG, mB);
        }

        glm::vec4 rgba() const
        {
            return glm::vec4(mR, mG, mB, mA);
        }

        Space space() const { return mSpace; }

        Color convertedTo(Space space) const
        {
            switch (mSpace) {
                case Color::Space::sRGB: {
                    float power = 2.2;
                    return {std::pow(mR, power), std::pow(mG, power), std::pow(mB, power), mA};
                }

                case Color::Space::YCoCg: {
                    float t = mR - mB;
                    float g = mR + mB;
                    float b = t - mG;
                    float r = t + mG;
                    return {r, g, b, mA};
                }

                case Color::Space::Linear: {
                    return *this;
                }
            }
        }

        Color scaled(float scale) const
        {
            return { mR * scale, mG * scale, mB * scale, mA };
        }

    private:
        float mR = 0.0;
        float mG = 0.0;
        float mB = 0.0;
        float mA = 1.0;

        Space mSpace;

        Color toLinear() const
        {
            Color linear = toLinear();

            switch (mSpace) {
                case Space::Linear: {
                    return linear;
                }

                case Space::sRGB: {
                    float power = 1.0 / 2.2;
                    return {std::pow(linear.mR, power), std::pow(linear.mG, power), std::pow(linear.mB, power), mA, Space::sRGB};
                }

                case Space::YCoCg: {
                    float Co = (linear.mR - linear.mB) / 2.0;
                    float t = linear.mB + Co;
                    float Cg = (linear.mG - t) / 2.0;
                    float Y = t + Cg;
                    return {Y, Co, Cg, mA, Space::YCoCg};
                }
            }
        }
    };

}
