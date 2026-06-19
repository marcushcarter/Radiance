#include <platform/window/window.h>
#include <platform/embedded/embedded_asset.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <windows.h>
#include <dwmapi.h>
#include <iostream>
#include <stb_image.h>

namespace Radiance
{
bool Window::Create(const char* title, uint32_t w, uint32_t h)
{
    if (!glfwInit()) {
        return false;
    }

    this->width = w;
    this->height = h;

    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);

    glfwWindow = glfwCreateWindow(w, h, title, nullptr, nullptr);
    if (!glfwWindow) {
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(glfwWindow);
    glfwSwapInterval(0);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        return false;
    }

    glfwSetWindowUserPointer(glfwWindow, this);

    glfwSetFramebufferSizeCallback(glfwWindow, [](GLFWwindow* win, int w, int h) {
        auto* self = static_cast<Window*>(glfwGetWindowUserPointer(win));
        self->width  = (uint32_t)w;
        self->height = (uint32_t)h;
        if (self->onFramebufferResize) self->onFramebufferResize((uint32_t)w, (uint32_t)h);
    });

    instance = this;

    return true;
}

void Window::Destroy()
{    
    if (glfwWindow) {
        glfwDestroyWindow(glfwWindow);
        glfwTerminate();
        glfwWindow = nullptr;
    }
}

bool Window::ShouldClose() const { return glfwWindowShouldClose(glfwWindow); }

void Window::PollEvents() const { glfwPollEvents(); }

void Window::SwapBuffers() const { glfwSwapBuffers(glfwWindow); }

void Window::Show() const { glfwShowWindow(glfwWindow); }

void Window::Hide() const { glfwHideWindow(glfwWindow); }

void Window::SetFullscreen(bool enabled)
{
    if (fullscreen == enabled) return;
    fullscreen = enabled;

    if (enabled) {
        glfwGetWindowSize(glfwWindow, (int*)&windowedWidth, (int*)&windowedHeight);
        glfwGetWindowPos(glfwWindow, &windowedX, &windowedY);

        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        glfwSetWindowMonitor(glfwWindow, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
        width  = mode->width;
        height = mode->height;
    } else {
        glfwSetWindowMonitor(glfwWindow, nullptr, windowedX, windowedY, windowedWidth, windowedHeight, 0);
        width  = windowedWidth;
        height = windowedHeight;
    }
}

void Window::SetVsync(bool enabled)
{
    if (vsync == enabled) return;
    vsync = enabled;
    glfwSwapInterval(vsync ? 1 : 0);
}

void Window::SetTitle(const char* title) { if (glfwWindow) glfwSetWindowTitle(glfwWindow, title); }

void Window::SetTitlebarColor(float r, float g, float b)
{
    HWND hwnd = glfwGetWin32Window(glfwWindow);
    COLORREF color = RGB((int)(r * 255), (int)(g * 255), (int)(b * 255));
    DwmSetWindowAttribute(hwnd, DWMWA_CAPTION_COLOR, &color, sizeof(color));
}

void Window::SetIcon(const char* resourceName)
{
    auto bytes = EmbeddedAsset::LoadBinary(resourceName);
    if (bytes.empty()) return;

    int w, h, channels;
    unsigned char* pixels = stbi_load_from_memory(bytes.data(), (int)bytes.size(), &w, &h, &channels, 4);
    if (!pixels) return;

    GLFWimage icon;
    icon.width  = w;
    icon.height = h;
    icon.pixels = pixels;

    glfwSetWindowIcon(glfwWindow, 1, &icon);

    stbi_image_free(pixels);
}
}