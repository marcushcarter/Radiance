#pragma once
#include <cstdint>
#include <opengl/opengl.h>
#include <glm/glm.hpp>

namespace Radiance
{
struct Scene;

struct CameraData
{
    glm::vec3 camPos;     float tanHalfFov;
    glm::vec3 camForward; float _pad0;
    glm::vec3 camRight;   float _pad1;
    glm::vec3 camUp;      float _pad2;
    glm::vec2 resolution;
    uint32_t  frame;
    uint32_t  sampleCount;
};

struct GPUSphere
{
    glm::vec3 center; float radius;
    glm::vec3 albedo; float fuzz;
    int matType; float ior; glm::vec2 _pad;
};

struct GPUTriangle
{
    glm::vec3 v0; float _p0;
    glm::vec3 v1; float _p1;
    glm::vec3 v2; float _p2;
    glm::vec3 albedo; float fuzz;
    int matType; float ior; glm::vec2 _pad;
};

struct Renderer
{
    uint32_t viewportWidth, viewportHeight;
    bool pendingResize = false;

    uint32_t sampleCount = 0;
    enum class ViewportMode { Clay, Material, Pathtrace } viewportMode = ViewportMode::Clay;

    OpenGL::Image image;
    OpenGL::Image envMap;
    OpenGL::Framebuffer framebuffer;

    OpenGL::ShaderProgram pathtracingShader;

    OpenGL::UniformBuffer cameraUBO;
    OpenGL::StorageBuffer sphereSSBO;
    OpenGL::StorageBuffer triangleSSBO;
    uint32_t frameCount = 0;

    void Start(uint32_t w, uint32_t h);
    void Shutdown();

    void UploadBVH(const Scene& scene);
    void ResetAccumulation();
    void RequestResize(uint32_t w, uint32_t h);
    void Resize();

    void RasterizeScene(const Scene& scene);
    void PathtraceTick(const Scene& scene);
    
    void EnterPathtrace(const Scene& scene) {
        UploadBVH(scene);
        ResetAccumulation();
        viewportMode = ViewportMode::Pathtrace;
        sampleCount = 0;
    }

    void ExitPathtrace() { viewportMode = ViewportMode::Clay; }
};
}