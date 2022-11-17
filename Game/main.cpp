
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
    
    Model dragon;
    Importer::LoadFromFile(dragon, "Resources\\Stanford-Dragon.Asset");

    MeshRenderer renderer;
    renderer.material = b;
    renderer.shader = EShaderType::Blinn;

    //model.renderers.push_back(renderer);

    Camera cam;
    cam.Position = { 0.0f, 0.0f, -2.0f };

    bool bEnableCamera = true;
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

            //variable to add deltaTime to

            static float fixedAcc = 0;
            fixedAcc += dt;

            //Variable that holds how often to update, in seconds
            float timeStep = (1.0f / 0.5f);
            while (fixedAcc > (timeStep)) {

                Log("dt: %d\n", (int)(1.0f / dt));
                fixedAcc -= (timeStep);
            }

            gfx.Clear();
            {
                bool bShowOverlay = true;
                ImGui::SetNextWindowPos({ 0, 0 });
                ImGui::Begin("Status", &bShowOverlay, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoNav);
                ImGui::Text("Frame Rate: %dfps", (int)(1.0f / dt));
                ImGui::Checkbox("Enable Camera [Tab]", &bEnableCamera);
                ImGui::InputFloat3("Position", (float*)&cam.Position);
                ImGui::InputFloat3("Direction", (float*)&cam.Forward);
                ImGui::End();

                ImGui::Begin("Lights");
                ImGui::Text("Directional");
                ImGui::SliderFloat3("Direction", (float*)(&gfx.GetLights().directional.direction), -1.0f, 1.0f);
                ImGui::ColorEdit4("Colour", (float*)(&gfx.GetLights().directional.colour));

                auto pointLights = gfx.GetLights().pointLights;
                for (int i = 0; i < NUM_POINT_LIGHTS; i++)
                {
                    ImGui::Text("Point Light %d", i);
                    std::string posLabel = "Position";
                    posLabel.append(" [");
                    posLabel.append(std::to_string(i));
                    posLabel.append("]");

                    std::string colLabel = "Colour";
                    colLabel.append(" [");
                    colLabel.append(std::to_string(i));
                    colLabel.append("]");

                    std::string radLabel = "Radius";
                    radLabel.append(" [");
                    radLabel.append(std::to_string(i));
                    radLabel.append("]");

                    ImGui::SliderFloat3(posLabel.c_str(), ((float*)(&pointLights[i].position)), -300.0f, 300.0f);
                    ImGui::ColorEdit4(colLabel.c_str(), (float*)(&gfx.GetLights().pointLights[i].colour));
                    ImGui::SliderFloat(radLabel.c_str(), &gfx.GetLights().pointLights[i].radius, 0.0f, 50.0f);
                }

                ImGui::End();

                ImGui::Begin("Materials");
                for (auto renderer : model.renderers) {
                    Engine::Blinn* b = (Engine::Blinn*)renderer.material;

                    ImGui::Text("Renderers");
                   
                }

                ImGui::End();
            }

            //Input
            {
                if (bEnableCamera) {
                    if (Input::Mouse::MouseMoved()) {
                        auto mouseDelta = Input::Mouse::DeltaPosition();
                            cam.Look(mouseDelta.x, mouseDelta.y, 100.0f * dt);
                    }
                }
                if (Input::Keyboard::KeyPressed(Input::Keys::KB_KEY_TAB)) {
                    bEnableCamera = !bEnableCamera;
                    window.ShowMouseCursor(!bEnableCamera);
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
            for (auto i = 0; i < dragon.meshes.size(); i++) {
                gfx.Draw(dragon.worldMatrix, dragon.meshes.at(i), dragon.renderers.at(i), cam);
            }
            for (auto i = 0; i < model.meshes.size(); i++) {
                gfx.Draw(model.worldMatrix, model.meshes.at(i), model.renderers.at(i), cam);
            }

            gfx.Present();
        }
    }
}