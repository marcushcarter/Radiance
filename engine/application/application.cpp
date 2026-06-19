#include <application/application.h>
#include <glad/glad.h>
#include <imgui.h>
#include <windows.h>
#include <shlobj.h>
#include <filesystem>
#include "IconsFontAwesome6.h"
#include <platform/embedded/embedded_asset.h>

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

        OpenGL::Framebuffer::Unbind();
        glClearColor(0,0,0,1);
        glClear(GL_COLOR_BUFFER_BIT);

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
    window.SetIcon("LOGOS_FAVICON_PNG");

    imgui.Create(window.GetNative());
    
    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = iniPath.c_str();

    {
        std::vector<uint8_t> jbData = EmbeddedAsset::LoadBinary("FONTS_JETBRAINS_MONO_REGULAR_TTF");
        if (!jbData.empty()) {
            void* jbCopy = IM_ALLOC(jbData.size());
            memcpy(jbCopy, jbData.data(), jbData.size());
            ImFontConfig jbCfg;
            jbCfg.FontDataOwnedByAtlas = true;
            io.Fonts->AddFontFromMemoryTTF(jbCopy, (int)jbData.size(), 14.0f, &jbCfg);
        }

        std::vector<uint8_t> faData = EmbeddedAsset::LoadBinary("FONTS_FA_SOLID_900_OTF");
        if (!faData.empty()) {
            void* faCopy = IM_ALLOC(faData.size());
            memcpy(faCopy, faData.data(), faData.size());
            static const ImWchar faRanges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
            ImFontConfig faCfg;
            faCfg.MergeMode = true;
            faCfg.PixelSnapH = true;
            faCfg.FontDataOwnedByAtlas = true;
            io.Fonts->AddFontFromMemoryTTF(faCopy, (int)faData.size(), 14.0f, &faCfg, faRanges);
        }

        io.Fonts->Build();
    }

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
        ImGuiWindowFlags_NoNavFocus;

    ImGuiViewport* viewport = ImGui::GetMainViewport();
    // ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y + 20));
    // ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, viewport->Size.y - 20));
    ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y + m_menuBarHeight));
    ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, viewport->Size.y - m_menuBarHeight));
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
    
    DrawMenuBar();
    DrawViewport();
    DrawSceneInspector();
    DrawRenderSettings();

    imgui.Render();
}

void Application::DrawViewport()
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::Begin(ICON_FA_EYE " Viewport");

    m_viewportPos = ImGui::GetWindowPos();

    ImVec2 size = ImGui::GetContentRegionAvail();
    if (size.x > 0 && size.y > 0) {
        uint32_t w = (uint32_t)size.x;
        uint32_t h = (uint32_t)size.y;
        if (w != renderer.viewportWidth || h != renderer.viewportHeight)
            renderer.RequestResize(w, h);

        ImVec2 pos = ImGui::GetCursorScreenPos();
        ImDrawList* dl = ImGui::GetWindowDrawList();
        ImGui::Image((ImTextureID)(uint64_t)renderer.image.Get(), size, ImVec2(0, 1), ImVec2(1, 0));
    }

    ImGui::End();
    ImGui::PopStyleVar();
}

void Application::DrawMenuBar()
{
    const float barHeight = 48.0f;
    ImGuiViewport* vp = ImGui::GetMainViewport();

    ImGui::SetNextWindowPos(vp->Pos);
    ImGui::SetNextWindowSize(ImVec2(vp->Size.x, barHeight));
    ImGui::SetNextWindowViewport(vp->ID);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(12.0f, 0.0f));

    ImGui::Begin("##menubar", nullptr,
        ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoDocking |
        ImGuiWindowFlags_NoBringToFrontOnFocus);

    ImGui::PopStyleVar(3);

    float rowY = (barHeight - ImGui::GetFrameHeight()) * 0.5f;
    ImGui::SetCursorPos(ImVec2(12.0f, rowY));

    // File menu (as a button + popup, not a native menu bar)
    if (ImGui::Button("File")) ImGui::OpenPopup("FilePopup");
    if (ImGui::BeginPopup("FilePopup")) {
        if (ImGui::MenuItem("New")) {}
        if (ImGui::MenuItem("Open...")) {}
        if (ImGui::MenuItem("Save", "Ctrl+S")) {}
        if (ImGui::MenuItem("Save As...")) {}
        ImGui::Separator();
        if (ImGui::MenuItem("Exit")) Close();
        ImGui::EndPopup();
    }

    ImGui::SameLine();
    if (ImGui::Button("View")) ImGui::OpenPopup("ViewPopup");
    if (ImGui::BeginPopup("ViewPopup")) {
        if (ImGui::MenuItem("Reset Layout")) {}
        ImGui::EndPopup();
    }

    bool pathtracing = renderer.viewportMode == Renderer::ViewportMode::Pathtrace;
    ImVec2 btnSize = ImVec2(72.0f, ImGui::GetFrameHeight());
    float centerX = (vp->Size.x - btnSize.x) * 0.5f;

    ImGui::SameLine(centerX);
    if (ImGui::Button(pathtracing ? ICON_FA_PAUSE : ICON_FA_PLAY, btnSize)) {
        if (pathtracing) renderer.ExitPathtrace();
        else renderer.EnterPathtrace(scene);
    }

    const char* rightText = pathtracing ? "rendering..." : "ready";
    ImVec2 textSize = ImGui::CalcTextSize(rightText);
    ImGui::SameLine(vp->Size.x - textSize.x - 24.0f);
    ImGui::SetCursorPosY(rowY + (ImGui::GetFrameHeight() - textSize.y) * 0.5f);
    ImGui::TextDisabled("%s", rightText);

    ImGui::End();
}

void Application::DrawSceneInspector()
{
    ImGui::Begin("Scene Heirarchy");
    ImGui::End();
    
    ImGui::Begin("Inspector");
    ImGui::End();
    
    ImGui::Begin("Render Settings");
    ImGui::End();
    
    ImGui::Begin("Profiler");
    ImGui::End();
}

void Application::DrawRenderSettings()
{

}
}