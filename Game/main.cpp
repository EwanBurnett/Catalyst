
#include "Framework.h"
#include <string>

#include "Graphics/Window.h"
#include "Graphics/Backends/DX11_GFX.h"

using namespace Engine;

int WINAPI WinMain(
    _In_ HINSTANCE inst,
    _In_ HINSTANCE prevInst,     //Unused 
    _In_ LPSTR cmd,
    _In_ int showCmd
)
{
    VideoMode mode = { .width = 1280, .height = 720 };
    mode.adapterIndex = 1;

    Window window(L"Hello Cube", inst, mode);
    window.SetIcon(L"E:/Source/repos/Vega-Framework/Vega-Renderer/AppIcon.ico");
    window.Show(showCmd);

    DX11_GFX gfx;
    gfx.Init(window, { .xResolution = 1280, .yResolution = 720 });

    Primitives::Cube cube;
    Blinn* b = new Blinn;
    b->Diffuse = { 0x0f, 0xff, 0xf0, 0xff };
    b->Ambient = { 0xAA, 0xAA, 0xAA, 0xAA };

    MeshRenderer renderer;
    renderer.material = b;
    renderer.shader = EShaderType::Blinn;

    Model model;
    model.meshes.push_back(cube);
    model.renderers.push_back(renderer);

    Camera cam;
    cam.Position = { 0.0f, 0.0f, -2.0f };
    //cam.FoVDegrees = { 90 };

    //Application Loop
    MSG msg;
    ZeroMemory(&msg, sizeof(msg));

    while (msg.message != WM_QUIT) {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            static float r = 0;
            r += 1.0f / 120000.0f;

            model.ComputeWorld({}, { cos(r) * 360, 45.0f, sin(r) * 360 });
            gfx.Clear(sin(r) * 0xD9, cos(r) * 0xAA, 0xAD, 0xFF);
            cam.ComputeViewProjection();

            for (auto i = 0; i < model.meshes.size(); i++) {
                gfx.Draw(model.worldMatrix, model.meshes.at(i), model.renderers.at(i), cam);
            }
            gfx.Present();
        }
    }
}