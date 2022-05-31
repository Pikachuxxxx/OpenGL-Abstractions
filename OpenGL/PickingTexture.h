#pragma once

#include <glad/glad.h>

class PickingTexture
{
public:
    struct PixelInfo
    {
        unsigned int ObjectID = 0;
        unsigned int DrawID = 0;
        unsigned int PrimID = 0;
    };

public:
    PickingTexture(unsigned int width, unsigned int height);
    ~PickingTexture();

    void enableWriting();
    void disableWriting();

    PixelInfo readPixel(unsigned int x, unsigned int y);

//private:
    GLuint m_FBO;
    GLuint m_PickingTexture;
    GLuint m_DepthTexture;

};
