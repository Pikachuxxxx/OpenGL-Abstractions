#include "Texture3D.h"

#include <glm/gtc/type_ptr.hpp>

Texture3D::Texture3D(void* data, int dimensions, int slotID /*= 0*/)
    : m_SlotID(slotID), m_Dimensions(dimensions)
{
    glGenTextures(1, &m_TID);
    glBindTexture(GL_TEXTURE_3D, m_TID);

    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexStorage3D(GL_TEXTURE_3D, log2(dimensions), GL_RGBA8, dimensions, dimensions, dimensions);
    glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, 0, dimensions, dimensions, dimensions, GL_RGBA, GL_FLOAT, data);
    glGenerateMipmap(GL_TEXTURE_3D);
    glBindTexture(GL_TEXTURE_3D, 0);
}

Texture3D::~Texture3D()
{

}

void Texture3D::Bind()
{
    glActiveTexture(GL_TEXTURE0 + m_SlotID);
    glBindTexture(GL_TEXTURE_3D, m_TID);
}

void Texture3D::Unbind()
{
    glBindTexture(GL_TEXTURE_3D, 0);
}

void Texture3D::Clear(glm::vec4 clearColor)
{
    glBindTexture(GL_TEXTURE_3D, m_TID);
    glClearTexImage(m_TID, 0, GL_RGBA, GL_FLOAT, glm::value_ptr(clearColor));
    glBindTexture(GL_TEXTURE_3D, 0);
}

void Texture3D::UpdateData(void* data)
{
    glBindTexture(GL_TEXTURE_3D, m_TID);
    glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, 0, m_Dimensions, m_Dimensions, m_Dimensions, GL_RGBA, GL_FLOAT, data);
    glBindTexture(GL_TEXTURE_3D, 0);
}

void Texture3D::fill_pixel(GLfloat* data, int x, int y, int z, int w, int h, int d)
{
    int floats = 4; // r+g+b+a
    int i = floats * (x + w * (y + h * z));

    assert(i < (w* h* d* floats));

    data[i + 0] = 1.0;
    data[i + 1] = 1.0;
    data[i + 2] = 1.0;
    data[i + 3] = 1.0;
}

void Texture3D::fill_corners(GLfloat* data, int w, int h, int d)
{
    assert(w == h && h == d);

    for (int x = 1; x < w; x++) {
        fill_pixel(data, x, 1, 1, w, h, d);
        fill_pixel(data, x, h - 1, 1, w, h, d);
        fill_pixel(data, x, 1, d - 1, w, h, d);
        fill_pixel(data, x, h - 1, d - 1, w, h, d);

        fill_pixel(data, 1, x, 1, w, h, d);
        fill_pixel(data, w - 1, x, 1, w, h, d);
        fill_pixel(data, w - 1, x, d - 1, w, h, d);
        fill_pixel(data, 1, x, d - 1, w, h, d);

        fill_pixel(data, 1, 1, x, w, h, d);
        fill_pixel(data, w - 1, 1, x, w, h, d);
        fill_pixel(data, w - 1, h - 1, x, w, h, d);
        fill_pixel(data, 1, h - 1, x, w, h, d);
    }
}
