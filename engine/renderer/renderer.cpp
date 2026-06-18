#include <renderer/renderer.h>
#include <scene/scene.h>
#include <glad/glad.h>
#include <platform/embedded/embedded_asset.h>
#include <stb_image.h>
#include <iostream>

namespace Radiance
{
static CameraData BuildCameraData(const Camera& cam, uint32_t w, uint32_t h, uint32_t frame, uint32_t sampleCount)
{
    glm::vec3 forward = glm::normalize(cam.target - cam.position);
    glm::vec3 right = glm::normalize(glm::cross(forward, cam.up));
    glm::vec3 up = glm::cross(right, forward);

    CameraData data;
    data.camPos = cam.position;
    data.tanHalfFov = tanf(glm::radians(cam.fov) * 0.5f);
    data.camForward = forward;
    data.camRight = right;
    data.camUp = up;
    data.resolution = glm::vec2((float)w, (float)h);
    data.frame = frame;
    data.sampleCount = sampleCount;
    return data;
}

void Renderer::Start(uint32_t w, uint32_t h)
{
    viewportWidth = w;
    viewportHeight = h;

    image.Create(viewportWidth, viewportHeight, GL_RGBA32F);
    framebuffer.Create({ image.Get() });

    {
        auto hdrBytes = EmbeddedAsset::LoadBinary("SKYBOX_KLOOFENDAL_48D_PARTLY_CLOUDY_PURESKY_2K_HDR");
        int ew, eh, ec;
        float* hdrData = stbi_loadf_from_memory(hdrBytes.data(), (int)hdrBytes.size(), &ew, &eh, &ec, 4);
        if (hdrData) {
            envMap.LoadRgbaF32((uint32_t)ew, (uint32_t)eh, hdrData);
            stbi_image_free(hdrData);
        }
    }

    pathtracingShader.Load("", "", EmbeddedAsset::LoadText("SHADERS_RAYTRACE_COMP"));
    
    cameraUBO.Create(sizeof(CameraData));
}

void Renderer::Shutdown()
{
    pathtracingShader.Destroy();

    framebuffer.Destroy();
    image.Destroy();

    sampleCount = 0;
}

void Renderer::UploadBVH(const Scene& scene)
{
    std::vector<GPUSphere> spheres = {
        { {-2.25f, 0.0f, 0.0f}, 0.5f, {1.0f, 1.0f, 1.0f}, 0.0f, 2, 1.5f, {0,0} }, // glass
        { {-0.75f, 0.0f, 0.0f}, 0.5f, {1.0f, 1.0f, 1.0f}, 0.0f, 0, 1.0f, {0,0} }, // diffuse white
        { { 0.75f, 0.0f, 0.0f}, 0.5f, {1.0f, 1.0f, 1.0f}, 0.0f, 1, 1.0f, {0,0} }, // metal
        { { 2.25f, 0.0f, 0.0f}, 0.5f, {0.5f, 0.5f, 1.0f}, 0.3f, 1, 1.0f, {0,0} }, // fuzzy metal
    };

    float y = -0.5f, hh = 5.0f;
    glm::vec3 c0(-hh, y, -hh);
    glm::vec3 c1( hh, y, -hh);
    glm::vec3 c2( hh, y,  hh);
    glm::vec3 c3(-hh, y,  hh);
    glm::vec3 ground(0.7f, 0.7f, 0.7f);

    std::vector<GPUTriangle> triangles = {
        { c0, 0.0f, c1, 0.0f, c2, 0.0f, ground, 0.0f, 0, 1.0f, {0,0} },
        { c0, 0.0f, c2, 0.0f, c3, 0.0f, ground, 0.0f, 0, 1.0f, {0,0} },
    };

    sphereSSBO.FromVector(spheres, 0);
    triangleSSBO.FromVector(triangles, 1);
}

void Renderer::ResetAccumulation()
{
    sampleCount = 0;
}

void Renderer::RequestResize(uint32_t w, uint32_t h)
{
    viewportWidth = w;
    viewportHeight = h;
    pendingResize = true;
}

void Renderer::Resize()
{
    image.Resize(viewportWidth, viewportHeight);
    framebuffer.Create({ image.Get() });

    pendingResize = false;
}

void Renderer::RasterizeScene(const Scene& scene)
{
    if (pendingResize) Resize();

    CameraData camData = BuildCameraData(scene.camera, viewportWidth, viewportHeight, frameCount, sampleCount);
    cameraUBO.Update(&camData);

    framebuffer.Bind();
    glClearColor(0,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::PathtraceTick(const Scene& scene)
{
    if (pendingResize) {
        Resize();
        ResetAccumulation();
    }

    CameraData camData = BuildCameraData(scene.camera, viewportWidth, viewportHeight, frameCount, sampleCount);
    cameraUBO.Update(&camData);

    uint32_t gx = (viewportWidth + 7) / 8;
    uint32_t gy = (viewportHeight + 7) / 8;

    pathtracingShader.Bind();
    image.BindStorage(0, GL_READ_WRITE);
    envMap.BindSampled(1);
    cameraUBO.Bind(0);
    sphereSSBO.Bind(0);
    triangleSSBO.Bind(1);
    pathtracingShader.Dispatch(gx, gy);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    sampleCount++;
    frameCount++;
}
}