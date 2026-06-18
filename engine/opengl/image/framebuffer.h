#pragma once
#include <glad/glad.h>
#include <cstdint>
#include <vector>

namespace Radiance::OpenGL
{
struct Framebuffer
{
    uint32_t framebuffer = 0;

    uint64_t sampledHandle = 0;
    uint64_t storageHandle = 0;

    uint32_t width = 0;
    uint32_t height = 0;
    GLenum format = GL_RGBA32F;
    
    ~Framebuffer();

    bool Create(const std::vector<uint32_t>& colorAttachments, uint32_t depthAttachment = 0);
    void Destroy();

    void Bind() const;
    static void Unbind();

    uint32_t Get() const { return framebuffer; }
};
}