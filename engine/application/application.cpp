#include <application/application.h>
#include <glad/glad.h>
#include <imgui.h>

#include <windows.h>
#include <shlobj.h>
#include <filesystem>

namespace Radiance
{
void Application::Run()
{
    Setup();
    
    while (!window.ShouldClose() && running) {
        window.PollEvents();

        BuildEditorUI();

        switch (renderer.viewportMode) {
            case Renderer::ViewportMode::Clay: renderer.RasterizeScene(scene); break;
            case Renderer::ViewportMode::Pathtrace: renderer.PathtraceTick(scene); break;
        }
        renderer.BlitToScreen();

        imgui.RecordDraw();
        window.SwapBuffers();
    }

    Shutdown();
}

void Application::Close() { running = false; }

void Application::Setup()
{
    PWSTR roaming = nullptr;
    SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, nullptr, &roaming);
    std::filesystem::path configDir = std::filesystem::path(roaming) / "Ballistic/Radiance";
    CoTaskMemFree(roaming);

    std::filesystem::create_directories(configDir);
    iniPath = (configDir / "imgui.ini").string();

    window.Create("Radiance", 1280, 800);
    window.SetTitlebarColor(0.15f, 0.15f,0.15f);
    window.onFramebufferResize = [this](uint32_t w, uint32_t h) {
        renderer.RequestResize(w, h);
    };

    imgui.Create(window.GetNative());

    ImGui::GetIO().IniFilename = iniPath.c_str();

    renderer.Start(window.width, window.height);
}

void Application::Shutdown()
{
    renderer.Shutdown();
    window.Destroy();
}

void Application::BuildEditorUI()
{
    imgui.NewFrame();
    
    ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBackground;

    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y + 20));
    ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, viewport->Size.y - 20));
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

    ImGui::Begin("##dockspace", nullptr, flags);
    ImGui::PopStyleVar(3);
    ImGuiID id = ImGui::GetID("MainDockspace");
    ImGui::DockSpace(id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);
    ImGui::End();

    static float uiAlpha = 1.0f;
    bool pathtracing = renderer.viewportMode == Renderer::ViewportMode::Pathtrace;
    float targetAlpha = pathtracing ? 0.0f : 1.0f;
    uiAlpha += (targetAlpha - uiAlpha) * min(ImGui::GetIO().DeltaTime * 8.0f, 1.0f);
    
    DrawMenuBar(uiAlpha);
    DrawPathtracingToggle(ImGui::GetIO().DeltaTime);

    imgui.Render();
}

void Application::DrawMenuBar(float uiAlpha)
{
    bool pathtracing = renderer.viewportMode == Renderer::ViewportMode::Pathtrace;
    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, uiAlpha);
    if (pathtracing) ImGui::BeginDisabled();

    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("New")) {}
            if (ImGui::MenuItem("Open...")) {}
            if (ImGui::MenuItem("Save", "Ctrl+S")) {}
            if (ImGui::MenuItem("Save As...")) {}
            ImGui::Separator();
            if (ImGui::MenuItem("Exit")) Close();
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("View")) {
            if (ImGui::MenuItem("Reset Layout")) {}
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
    
    if (pathtracing) ImGui::EndDisabled();
    ImGui::PopStyleVar();
}

void Application::DrawPathtracingToggle(float dt)
{
    static float t = 0.0f;
    static float colorT = 0.0f;
    
    bool pathtracing = renderer.viewportMode == Renderer::ViewportMode::Pathtrace;
    float target = pathtracing ? 1.0f : 0.0f;
    t += (target - t) * min(dt * 10.0f, 1.0f);
    if (pathtracing) colorT += dt * 0.4f;

    float hue = 0.65f + 0.1f * sinf(colorT * 3.14159f * 2.0f);
    float sat = 0.75f, val = 0.85f;

    ImVec4 onColor = ImColor::HSV(hue, sat, val);
    ImVec4 offColor = ImVec4(0.32f, 0.32f, 0.32f, 1.0f);
    ImVec4 bgColor = ImVec4(
        offColor.x + (onColor.x - offColor.x) * t,
        offColor.y + (onColor.y - offColor.y) * t,
        offColor.z + (onColor.z - offColor.z) * t,
        1.0f
    );

    ImGui::SetNextWindowPos(ImVec2(16, 16 + ImGui::GetFrameHeight()), ImGuiCond_Always);
    ImGui::SetNextWindowBgAlpha(0.0f);
    ImGui::SetNextWindowSize(ImVec2(200, 48), ImGuiCond_Always);
    ImGui::Begin("##toggle_overlay", nullptr,
        ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDocking);

    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImVec2 pos = ImGui::GetCursorScreenPos();

    const float W = 56.0f, H = 28.0f, R = H * 0.5f;

    ImU32 bg = ImColor(bgColor);
    dl->AddRectFilled(ImVec2(pos.x + R, pos.y), ImVec2(pos.x + W - R, pos.y + H), bg);
    dl->AddCircleFilled(ImVec2(pos.x + R, pos.y + R), R, bg);
    dl->AddCircleFilled(ImVec2(pos.x + W - R, pos.y + R), R, bg);

    if (t > 0.01f) {
        ImU32 glowColor = ImColor(onColor.x, onColor.y, onColor.z, t * 0.35f);
        dl->AddCircleFilled(ImVec2(pos.x + R, pos.y + R), R + 3.0f, glowColor);
        dl->AddCircleFilled(ImVec2(pos.x + W - R, pos.y + R), R + 3.0f, glowColor);
        dl->AddRectFilled(ImVec2(pos.x + R, pos.y - 3.0f), ImVec2(pos.x + W - R, pos.y + H + 3.0f), glowColor);
    }

    float knobX = pos.x + R + t * (W - H);
    dl->AddCircleFilled(ImVec2(knobX, pos.y + R), R - 3.0f, IM_COL32(230, 230, 230, 255));

    ImGui::InvisibleButton("##toggle", ImVec2(W, H));
    if (ImGui::IsItemClicked()) {
        if (pathtracing) renderer.ExitPathtrace();
        else renderer.EnterPathtrace(scene);
    }

    if (t > 0.3f) {
        ImGui::SameLine(0, 8);
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 6);
        ImGui::TextColored(ImVec4(onColor.x, onColor.y, onColor.z, t), "SPP %u", renderer.sampleCount);
    }

    ImGui::End();
}
}