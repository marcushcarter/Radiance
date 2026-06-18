#pragma once
#include <platform/window/window.h>
#include <platform/imgui/imgui_layer.h>
#include <renderer/renderer.h>
#include <scene/scene.h>
#include <string>
#include <imgui.h>

namespace Radiance
{
struct Application
{
    Window window;
    ImGuiLayer imgui;
    Renderer renderer;
    Scene scene;
    bool running = true;

    std::string iniPath;
    ImVec2 m_viewportPos = ImVec2(0, 0);

    void Run();
    void Close();

    void Setup();
    void Shutdown();
    
    void BuildEditorUI();
    void DrawViewport();
    void DrawMenuBar();
    void DrawSceneInspector();
    void DrawRenderSettings();


float m_menuBarHeight = 48.0f;
};
}