#pragma once
#include <glad/glad.h>
#include <cstdint>
#include <string>
#include <unordered_map>

namespace Radiance::OpenGL
{
struct ShaderProgram
{
    uint32_t program = 0;
    std::unordered_map<std::string, GLint> uniformCache;

    ~ShaderProgram();

    void Load(const std::string& vert = "", const std::string& frag = "", const std::string& comp = "");
    void Destroy();

    void Bind() const;
    static void Unbind();

    void Dispatch(uint32_t gx, uint32_t gy = 1, uint32_t gz = 1);

    uint32_t GetUniformLocation(const std::string& name);

    void SetInt(const std::string& name, int v);
    void SetFloat(const std::string& name, float v);
    void SetVec2(const std::string& name, const float* v);
    void SetVec3(const std::string& name, const float* v);
    void SetMat4(const std::string& name, const float* m);
    void SetTextureHandle(const std::string& name, uint64_t handle);

    uint32_t Get() const { return program; }  
};
}