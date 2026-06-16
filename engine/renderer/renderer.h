#pragma once
#include <cstdint>
#include <opengl/opengl.h>

namespace Radiance
{
struct Scene;

struct Renderer
{
    uint32_t viewportWidth, viewportHeight;
    bool pendingResize = false;

    uint32_t sampleCount = 0;
    enum class ViewportMode { Clay, Material, Pathtrace } viewportMode;

    void Start(uint32_t w, uint32_t h);
    void Shutdown();

    void UploadBVH(const Scene& scene);
    void ResetAccumulation();
    void RequestResize(uint32_t w, uint32_t h);
    void Resize();

    void RasterizeScene(const Scene& scene);
    void PathtraceTick(const Scene& scene);
    void BlitToScreen();
    
    void EnterPathtrace(const Scene& scene) {
        UploadBVH(scene);
        ResetAccumulation();
        viewportMode = ViewportMode::Pathtrace;
        sampleCount = 0;
    }

    void ExitPathtrace() { viewportMode = ViewportMode::Clay; }
};
}