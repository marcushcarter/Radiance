#include <opengl/shader/shader_program.h>
#include <fstream>
#include <sstream>
#include <iostream>

namespace Radiance::OpenGL
{
ShaderProgram::~ShaderProgram() { Destroy(); }

uint32_t CompileStage(GLenum type, const std::string& src)
{
    uint32_t shader = glCreateShader(type);

    const char* cstr = src.c_str();
    glShaderSource(shader, 1, &cstr, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success) {
        char log[1024];
        glGetShaderInfoLog(shader, 1024, nullptr, log);
        std::cerr << "Shader compile error:\n" << log << "\n";
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

uint32_t LinkProgram(GLuint vs, GLuint fs, GLuint cs)
{
    uint32_t program = glCreateProgram();

    if (vs) glAttachShader(program, vs);
    if (fs) glAttachShader(program, fs);
    if (cs) glAttachShader(program, cs);
    glLinkProgram(program);

    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char log[1024];
        glGetProgramInfoLog(program, 1024, nullptr, log);
        std::cerr << "Program link error:\n" << log << "\n";
        return false;
    }

    if (vs) glDetachShader(program, vs);
    if (fs) glDetachShader(program, fs);
    if (cs) glDetachShader(program, cs);

    if (vs) glDeleteShader(vs);
    if (fs) glDeleteShader(fs);
    if (cs) glDeleteShader(cs);

    return program;
}

void ShaderProgram::Load(const std::string& vert, const std::string& frag, const std::string& comp)
{
    Destroy();

    GLuint vs = 0, fs = 0, cs = 0;
    if (!vert.empty()) vs = CompileStage(GL_VERTEX_SHADER, vert);
    if (!frag.empty()) fs = CompileStage(GL_FRAGMENT_SHADER, frag);
    if (!comp.empty()) cs = CompileStage(GL_COMPUTE_SHADER, comp);

    program = LinkProgram(vs, fs, cs);
    uniformCache.clear();
}

void ShaderProgram::Destroy()
{
    if (program) {
        glDeleteProgram(program);
        program = 0;
    }

    uniformCache.clear();
}

void ShaderProgram::Bind() const
{
    glUseProgram(program);
}

void ShaderProgram::Unbind()
{
    glUseProgram(0);
}

void ShaderProgram::Dispatch(uint32_t gx, uint32_t gy, uint32_t gz)
{
    glDispatchCompute(gx, gy, gz);
}

uint32_t ShaderProgram::GetUniformLocation(const std::string& name)
{
    auto it = uniformCache.find(name);
    if (it != uniformCache.end()) return it->second;
    GLint loc = glGetUniformLocation(program, name.c_str());
    uniformCache[name] = loc;
    return loc;    
}

void ShaderProgram::SetInt(const std::string& name, int v)
{
    glUniform1i(GetUniformLocation(name), v);
}

void ShaderProgram::SetFloat(const std::string& name, float v)
{
    glUniform1f(GetUniformLocation(name), v);
}

void ShaderProgram::SetVec2(const std::string& name, const float* v)
{
    glUniform2fv(GetUniformLocation(name), 1, v);
}

void ShaderProgram::SetVec3(const std::string& name, const float* v)
{
    glUniform3fv(GetUniformLocation(name), 1, v);
}

void ShaderProgram::SetMat4(const std::string& name, const float* m)
{
    glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, m);
}

void ShaderProgram::SetTextureHandle(const std::string& name, uint64_t handle)
{
    glUniformHandleui64ARB(GetUniformLocation(name), handle);
}
}