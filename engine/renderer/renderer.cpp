#include <renderer/renderer.h>
#include <scene/scene.h>
#include <glad/glad.h>

namespace Radiance
{
void Renderer::Start(uint32_t w, uint32_t h)
{
    viewportWidth = w;
    viewportHeight = h;
}

void Renderer::Shutdown()
{
    sampleCount = 0;
    // glClearTexImage(renderTex, 0, GL_RGBA, GL_FLOAT, nullptr);
}

void Renderer::UploadBVH(const Scene& scene)
{

}

void Renderer::ResetAccumulation()
{

}

void Renderer::RequestResize(uint32_t w, uint32_t h)
{
    viewportWidth = w;
    viewportHeight = h;
    pendingResize = true;
}

void Renderer::Resize()
{
    pendingResize = false;
}

void Renderer::RasterizeScene(const Scene& scene)
{
    (void)scene;

    if (pendingResize) Resize();
}

void Renderer::PathtraceTick(const Scene& scene)
{
    (void)scene;

    if (pendingResize) {
        Resize();
        ResetAccumulation();
    }

    sampleCount++;
}

void Renderer::BlitToScreen()
{
    glClearColor(1,1,1,1);
    glClear(GL_COLOR_BUFFER_BIT);
}
}