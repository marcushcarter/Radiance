#pragma once
#include <glad/glad.h>
#include <cstdint>

namespace Radiance::OpenGL
{
struct UniformBuffer
{
    uint32_t buffer = 0;
    size_t size = 0;

    ~UniformBuffer();

    bool Create(size_t sizeBytes);
    void Destroy();

    void Update(const void* data, size_t sizeBytes = 0);
    void Bind(uint32_t unit = 0) const;

    uint32_t Get() const { return buffer; }
};
}