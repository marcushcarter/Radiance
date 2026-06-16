#pragma once

struct GLFWwindow;

namespace Radiance
{
struct ImGuiLayer
{
    void Create(GLFWwindow* window);
    void Destroy();

    void NewFrame();
    void Render();
    void RecordDraw();
};
}