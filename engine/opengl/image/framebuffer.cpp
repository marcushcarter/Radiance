#include <opengl/image/framebuffer.h>

namespace Radiance::OpenGL
{
Framebuffer::~Framebuffer() { Destroy(); }

bool Framebuffer::Create(const std::vector<uint32_t>& colorAttachments, uint32_t depthAttachment)
{
    Destroy();

    glCreateFramebuffers(1, &framebuffer);

    std::vector<GLenum> drawBuffers;
    drawBuffers.reserve(colorAttachments.size());

    for (size_t i = 0; i < colorAttachments.size(); i++) {
        GLenum attachment = GL_COLOR_ATTACHMENT0 + (GLenum)i;
        glNamedFramebufferTexture(framebuffer, attachment, colorAttachments[i], 0);
        drawBuffers.push_back(attachment);
    }

    if (!drawBuffers.empty()) {
        glNamedFramebufferDrawBuffers(framebuffer, (GLsizei)drawBuffers.size(), drawBuffers.data());
    }

    if (depthAttachment) {
        glNamedFramebufferTexture(framebuffer, GL_DEPTH_ATTACHMENT, depthAttachment, 0);
    }

    return glCheckNamedFramebufferStatus(framebuffer, GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
}

void Framebuffer::Destroy()
{    
    if (framebuffer) {
        glDeleteFramebuffers(1, &framebuffer);
        framebuffer = 0;
    }
}

void Framebuffer::Bind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
}

void Framebuffer::Unbind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
}