#include <application/application.h>

void Application::Run()
{
    window.Create("Radiance", 1280, 800);
    window.Show();
    
    while (!window.ShouldClose() && running) {
        window.PollEvents();
    }

    window.Destroy();
}