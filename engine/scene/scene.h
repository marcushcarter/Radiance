#pragma once
#include <glm/glm.hpp>

using namespace glm;

namespace Radiance
{
struct Vertex
{
    vec3 pos;
    vec3 norm;
    vec2 uv;
    vec3 tan;
};

struct Mesh
{
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    int materialIndex = -1;
};

struct Sphere
{
    vec3 center;
    float radius;
    int materialIndex = -1;
};

struct Material
{
    vec3 albedo = vec3(1.0f);
    float roughness = 0.5f;

    vec3 emission = vec3(0.0f);
    float metallic = 0.0f;

    float ior = 1.5f;
    int type = 0; // 0=lambertian, 1=metal, 2=dialectric, 3=emissive
    int albedoTex = -1;
    int normalTex = -1;

    int roughnessTex = -1;
    int metallicTex = -1;
    int emissionTex = -1;
    float _pad = 0.0f;
};

struct Triangle
{
    vec4 v0, v1, v2;
    vec4 n0, n1, n2; 
    
    vec2 uv0, uv1, uv2, _pad0;

    int materialIndex = -1;
    // float _pad1 = 0.0f;
};

struct Camera
{
    vec3 position = vec3(0.0f, 0.0f, 5.0f);
    vec3 target = vec3(0.0f);
    vec3 up = vec3(0.0f, 1.0f, 0.0f);

    float fov = 60.0f;
    float nearPlane = 0.1f;
    float farPlane = 1000.0f;

    float aperture = 0.0f;
    float focusDistance = 10.0f;
};

struct Scene
{
    std::vector<Material> materials;
    std::vector<Mesh> meshes;
    std::vector<Sphere> spheres;
    Camera camera;
};
}