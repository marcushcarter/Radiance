#include <opengl/image/image.h>

namespace Radiance::OpenGL
{
Image::~Image() { Destroy(); }

bool Image::Create(uint32_t w, uint32_t h, GLenum fmt)
{
    Destroy();

    width = w;
    height = h;
    format = fmt;

    glCreateTextures(GL_TEXTURE_2D, 1, &image);
    glTextureStorage2D(image, 1, format, static_cast<GLsizei>(width), static_cast<GLsizei>(height));
    glTextureParameteri(image, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(image, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTextureParameteri(image, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(image, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    sampledHandle = glGetTextureHandleARB(image);
    glMakeTextureHandleResidentARB(sampledHandle);

    storageHandle = glGetImageHandleARB(image, 0, GL_FALSE, 0, format);
    glMakeImageHandleResidentARB(storageHandle, GL_READ_WRITE);

    return true;
}

bool Image::LoadRgbaF32(uint32_t w, uint32_t h, float* data)
{
    Destroy();
    
    width = w;
    height = h;
    format = GL_RGBA32F;

    glCreateTextures(GL_TEXTURE_2D, 1, &image);
    glTextureStorage2D(image, 1, format, static_cast<GLsizei>(width), static_cast<GLsizei>(height));
    glTextureSubImage2D(image, 0, 0, 0, static_cast<GLsizei>(width), static_cast<GLsizei>(height), GL_RGBA, GL_FLOAT, data);
    glTextureParameteri(image, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(image, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTextureParameteri(image, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(image, GL_TEXTURE_WRAP_T, GL_REPEAT);

    sampledHandle = glGetTextureHandleARB(image);
    glMakeTextureHandleResidentARB(sampledHandle);

    storageHandle = glGetImageHandleARB(image, 0, GL_FALSE, 0, format);
    glMakeImageHandleResidentARB(storageHandle, GL_READ_WRITE);

    return true;    
}

void Image::Resize(uint32_t w, uint32_t h)
{
    if (width == w && height == h) return;
    Create(w, h, format);
}

void Image::Destroy()
{
    if (storageHandle) {
        glMakeImageHandleNonResidentARB(storageHandle);
        storageHandle = 0;
    }
    
    if (sampledHandle) {
        glMakeTextureHandleNonResidentARB(sampledHandle);
        sampledHandle = 0;
    }
    
    if (image) {
        glDeleteTextures(1, &image);
        image = 0;
    }

    width = 0;
    height = 0;
}

void Image::BindSampled(uint32_t unit) const
{
    glBindTextureUnit(unit, image);
}

void Image::BindStorage(uint32_t unit, GLenum access) const
{
    glBindImageTexture(unit, image, 0, GL_FALSE, 0, access, format);
}
}