#pragma once
#include <platform/window/window.h>

struct Application
{
    Window window;
    bool running = true;

    void Run();
};