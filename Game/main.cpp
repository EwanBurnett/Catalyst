
#include "Framework.h"
#include <string>

#include "Graphics/Window.h"
#include "Graphics/Backends/DX11_GFX.h"
#include "Core/Input.h"
#include "IO/Importer.h"

using namespace Engine;

int WINAPI WinMain(
    _In_ HINSTANCE inst,
    _In_ HINSTANCE prevInst,     //Unused 
    _In_ LPSTR cmd,
    _In_ int showCmd
)
{
    VideoMode mode = { .width = 1280, .height = 720 };
    mode.adapterIndex = 0;

    Window window(L"Sponza Scene", inst, mode);
    window.SetIcon(L"Resources/Catalyst_Icon.ico");
    window.Show(showCmd);

    DX11_GFX gfx;
    gfx.Init(window, { .xResolution = 1280, .yResolution = 720 });

    Blinn* b = new Blinn;
    b->Diffuse = { 0x0f, 0xff, 0xf0, 0xff };
    b->Ambient = { 0xAA, 0xAA, 0xAA, 0xAA };

    Model model;
    Importer::LoadFromFile(model, "Resources\\Sponza.Asset");

    MeshRenderer renderer;
    renderer.material = b;
    renderer.shader = EShaderType::Blinn;

    model.renderers.push_back(renderer);

    Camera cam;
    cam.Position = { 0.0f, 0.0f, -2.0f };

    Time time;
    time.Reset();
    Input::Init(&time);

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
            time.Tick();
            float dt = time.DeltaTime();
            Input::Advance();

            static float r = 0;
            r += 1.0f / 120000.0f;

            static float fixedAcc = 0;
            fixedAcc += dt;
            while (fixedAcc > (1.0f )) {

                Log("dt: %d\n", (int)(1.0f / dt));
                fixedAcc -= (1.0f );
            }

            gfx.Clear(sin(r) * 0xD9, cos(r) * 0xAA, 0xAD, 0xFF);
            { 
                
                if (Input::Mouse::MouseMoved()) {
                    auto mouseDelta = Input::Mouse::DeltaPosition();
                    cam.Look(mouseDelta.x, mouseDelta.y, 100.0f * dt);
                }
                Engine::Vector3f dir = { 0.0f, 0.0f, 0.0f };
                if (Input::Keyboard::KeyDown(Input::Keys::KB_KEY_W))
                {
                    dir = dir + Engine::Vector3f{ 0.0f, 0.0f, 1.0f };
                }
                if (Input::Keyboard::KeyDown(Input::Keys::KB_KEY_S))
                {
                    dir = dir + Engine::Vector3f{ 0.0f, 0.0f, -1.0f };
                }
                if (Input::Keyboard::KeyDown(Input::Keys::KB_KEY_A))
                {
                    dir = dir + Engine::Vector3f{ -1, 0, 0 };
                }
                if (Input::Keyboard::KeyDown(Input::Keys::KB_KEY_D))
                {
                    dir = dir + Engine::Vector3f{ 1, 0, 0 };
                }
                if (Input::Keyboard::KeyDown(Input::Keys::KB_KEY_Q))
                {
                    dir = dir + Engine::Vector3f{ 0, -1, 0 };
                }
                if (Input::Keyboard::KeyDown(Input::Keys::KB_KEY_E))
                {
                    dir = dir + Engine::Vector3f{ 0, 1, 0 };
                }

                cam.Walk(dir, 25.0f * dt);
                cam.ComputeViewProjection();
            }
            if (Input::Keyboard::KeyPressed(Input::Keys::KB_KEY_ESC))
            {
                PostQuitMessage(0x04);
            }
            for (auto i = 0; i < model.meshes.size(); i++) {
                gfx.Draw(model.worldMatrix, model.meshes.at(i), model.renderers.at(i), cam);
            }
            gfx.Present();
        }
    }
}