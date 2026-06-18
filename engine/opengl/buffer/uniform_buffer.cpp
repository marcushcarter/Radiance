// uniform_buffer.cpp
#include <opengl/buffer/uniform_buffer.h>

namespace Radiance::OpenGL
{
UniformBuffer::~UniformBuffer() { Destroy(); }

bool UniformBuffer::Create(size_t sizeBytes)
{
    Destroy();

    size = sizeBytes;

    glCreateBuffers(1, &buffer);
    glNamedBufferStorage(buffer, (GLsizeiptr)size, nullptr, GL_DYNAMIC_STORAGE_BIT);

    return true;
}

void UniformBuffer::Destroy()
{
    if (buffer) {
        glDeleteBuffers(1, &buffer);
        buffer = 0;
    }
}

void UniformBuffer::Update(const void* data, size_t sizeBytes)
{
    if (!buffer) return;
    size_t updateSize = sizeBytes ? sizeBytes : size;
    glNamedBufferSubData(buffer, 0, (GLsizeiptr)updateSize, data);
}

void UniformBuffer::Bind(uint32_t unit) const
{
    glBindBufferBase(GL_UNIFORM_BUFFER, unit, buffer);
}
}