#pragma once
#include <glad/glad.h>
#include <cstdint>

namespace Radiance::OpenGL
{
struct Image
{
    uint32_t image = 0;
    uint64_t sampledHandle = 0;
    uint64_t storageHandle = 0;

    uint32_t width = 0;
    uint32_t height = 0;
    GLenum format = GL_RGBA32F;
    
    ~Image();

    bool Create(uint32_t w, uint32_t h, GLenum fmt);
    bool LoadRgbaF32(uint32_t w, uint32_t h, float* data);

    void Resize(uint32_t w, uint32_t h);
    void Destroy();

    void BindSampled(uint32_t unit) const;
    void BindStorage(uint32_t unit, GLenum access = GL_READ_WRITE) const;

    uint32_t Get() const { return image; }
    uint64_t GetSampledHandle() const { return sampledHandle; }
    uint64_t GetStorageHandle() const { return storageHandle; }
};
}