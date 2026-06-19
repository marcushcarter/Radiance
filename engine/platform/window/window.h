#pragma once
#include <cstdint>
#include <functional>

struct GLFWwindow;

namespace Radiance
{
struct Window
{
    GLFWwindow* glfwWindow = nullptr;
    uint32_t width = 1280, height = 800;
    bool fullscreen = false;
    bool vsync = false;

    uint32_t windowedWidth = 1280, windowedHeight = 800;
    int windowedX = 0, windowedY = 0;

    std::function<void(uint32_t, uint32_t)> onFramebufferResize;

    bool Create(const char* title, uint32_t width, uint32_t height);
    void Destroy();

    bool ShouldClose() const;
    void PollEvents() const;
    void SwapBuffers() const;

    void Show() const;
    void Hide() const;

    void SetFullscreen(bool enabled);
    void ToggleFullscreen() { SetFullscreen(!fullscreen); }
    
    void SetVsync(bool enabled);
    void ToggleVsync() { SetVsync(!vsync); }

    void SetTitle(const char* title);
    void SetTitlebarColor(float r, float g, float b);

    void SetIcon(const char* resourceName);

    GLFWwindow* GetNative() { return glfwWindow; }

private:
    inline static Window* instance;
};
}