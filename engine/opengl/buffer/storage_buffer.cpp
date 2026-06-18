// storage_buffer.cpp
#include <opengl/buffer/storage_buffer.h>

namespace Radiance::OpenGL
{
StorageBuffer::~StorageBuffer() { Destroy(); }

// bool StorageBuffer::Create(void* data, size_t sizeBytes)
// {
//     Destroy();

//         binding = bindingPoint;
//         size    = data.size() * sizeof(T);

//         glCreateBuffers(1, &buffer);
//         glNamedBufferData(buffer, (GLsizeiptr)size, data.data(), GL_STATIC_DRAW);
//         glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, buffer);

//         return true;

// }

void StorageBuffer::Destroy()
{
    if (buffer) {
        glDeleteBuffers(1, &buffer);
        buffer = 0;
    }
    size = 0;
}

void StorageBuffer::Bind(uint32_t unit) const
{
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, unit, buffer);
}
}