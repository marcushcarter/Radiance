// storage_buffer.h
#pragma once
#include <glad/glad.h>
#include <cstdint>
#include <vector>

namespace Radiance::OpenGL
{
struct StorageBuffer
{
    uint32_t buffer  = 0;
    uint32_t binding = 0;
    size_t size = 0;

    ~StorageBuffer();

    template<typename T>
    bool FromVector(const std::vector<T>& data, uint32_t bindingPoint)
    {
        Destroy();

        binding = bindingPoint;
        size = data.size() * sizeof(T);

        glCreateBuffers(1, &buffer);
        glNamedBufferData(buffer, (GLsizeiptr)size, data.data(), GL_STATIC_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, buffer);

        return true;
    }

    void Destroy();
    void Bind(uint32_t unit = 0) const;

    uint32_t Get() const { return buffer; }
};
}