#include "../inc/Graphics/Backends/DX11_GFX.h"

using namespace Engine;

constexpr UINT NUM_THREADS = 4;
Lights m_lights;

//DX11 SETUP ---------------------------------------------------------

void CreateDX11Device(Microsoft::WRL::ComPtr<ID3D11Device>& device, D3D_FEATURE_LEVEL* pFeatureLevel, Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context)
{
    //Create the DX11 Device
    UINT createDeviceFlags = 0;
#if defined(DEBUG) | defined(_DEBUG)
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_FEATURE_LEVEL featureLevels[] = {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_0
    };

    HR(
    D3D11CreateDevice(
        0,
        D3D_DRIVER_TYPE_HARDWARE,
        0,
        createDeviceFlags,
        featureLevels,
        ARRAYSIZE(featureLevels),
        D3D11_SDK_VERSION,
        device.ReleaseAndGetAddressOf(),
        pFeatureLevel,
        context.ReleaseAndGetAddressOf()),
        "\nError: D3D11 Device Creation Failed."
    );

}

void CreateDX11SwapChain(const HWND& windowHandle, Microsoft::WRL::ComPtr<ID3D11Device>& device, Microsoft::WRL::ComPtr<IDXGISwapChain>& swapChain, Engine::GraphicsMode graphicsMode = {})
{
    //Populate the Swap-chain desc
    DXGI_MODE_DESC bd = {  };
    {
        bd.Width = graphicsMode.xResolution;
        bd.Height = graphicsMode.yResolution;
        bd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        bd.RefreshRate.Numerator = graphicsMode.frameRate;
        bd.RefreshRate.Denominator = 1;
        bd.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
        bd.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    }

    DXGI_SWAP_CHAIN_DESC sd = {};
    {
        sd.BufferDesc = bd;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.BufferCount = 1;
        sd.OutputWindow = windowHandle;
        sd.Windowed = !graphicsMode.isFullscreen;
        sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
        sd.Flags = 0;
        if (graphicsMode.MSAACount > 0 && graphicsMode.MSAAQuality > 0) {
            sd.SampleDesc.Count = graphicsMode.MSAACount;
            sd.SampleDesc.Quality = graphicsMode.MSAAQuality - 1;
        }
        else {
            sd.SampleDesc.Count = 1;
            sd.SampleDesc.Quality = 0;
        }
    }

    //Create the swapchain
    IDXGIDevice* dxgiDevice = 0;
    HR(device->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice), "DXGI Device Acquisition Failed.");

    IDXGIAdapter* dxgiAdapter = 0;
    HR(dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter), "DXGI Adapter Acquisition Failed.");

    IDXGIFactory* dxgiFactory = 0;
    HR(dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory), "DXGI Factory Acquisition Failed.");

    HR(dxgiFactory->CreateSwapChain(dxgiDevice, &sd, swapChain.ReleaseAndGetAddressOf()), "Swapchain Creation Failed.");

    dxgiFactory->Release();
    dxgiAdapter->Release();
    dxgiDevice->Release();

    Log("Created SwapChain.\n");
}

//TODO: Create Deferred Contexts for use on multiple threads
void CreateDX11DeferredContexts(Microsoft::WRL::ComPtr<ID3D11Device>& device, std::vector<Microsoft::WRL::ComPtr<ID3D11DeviceContext>>& contexts)
{
    contexts.reserve(NUM_THREADS);

    for(UINT i = 0; i < NUM_THREADS; i++)
    {
        HR(device->CreateDeferredContext(
            0,
            contexts[i].GetAddressOf()
        ), "Deferred Context Creation Failed!");
    }
}

void CreateDX11Views(Microsoft::WRL::ComPtr<ID3D11Device>& device, Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context, Microsoft::WRL::ComPtr<IDXGISwapChain>& swapChain, Microsoft::WRL::ComPtr<ID3D11RenderTargetView>& renderTargetView, Microsoft::WRL::ComPtr<ID3D11DepthStencilView>& depthStencilView, D3D11_TEXTURE2D_DESC& backBufferDesc, D3D11_VIEWPORT& viewport, Engine::GraphicsMode mode = {})
{
    //Create the render target view
    ID3D11Texture2D* backBuffer;
    HR(swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer), "\nBack buffer acquisition failed.");
    backBuffer->GetDesc(&backBufferDesc);

    HR(device->CreateRenderTargetView(backBuffer, nullptr, renderTargetView.ReleaseAndGetAddressOf()), "\nUnable to create Render Target View.");
    backBuffer->Release();

    //Create the Depth-Stencil buffer
    if (mode.enableDepthStencil)
    {
        D3D11_TEXTURE2D_DESC dsd = {};
        dsd.Width = mode.xResolution;
        dsd.Height = mode.yResolution;
        dsd.MipLevels = 1;
        dsd.ArraySize = 1;
        dsd.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        if (mode.MSAACount > 0) {
            dsd.SampleDesc.Count = mode.MSAACount;
            dsd.SampleDesc.Quality = mode.MSAAQuality - 1;
        }
        else {
            dsd.SampleDesc.Count = 1;
            dsd.SampleDesc.Quality = 0;
        }
        dsd.Usage = D3D11_USAGE_DEFAULT;
        dsd.BindFlags = D3D11_BIND_DEPTH_STENCIL;

        ID3D11Texture2D* depthStencilBuffer;
        HR(device->CreateTexture2D(&dsd, nullptr, &depthStencilBuffer), "\nUnable to create Depth-Stencil Buffer.");

        D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
        dsvd.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
        dsvd.Texture2D.MipSlice = 0;
        dsvd.Flags = 0;

        HR(device->CreateDepthStencilView(depthStencilBuffer, &dsvd, depthStencilView.GetAddressOf()), "\nUnable to create Depth-Stencil View.");

        depthStencilBuffer->Release();
    }

    //Bind views to the Output Merger and set the viewport
    context->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), depthStencilView.Get());

    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;
    viewport.Width = mode.xResolution;
    viewport.Height = mode.yResolution;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;

    context->RSSetViewports(1, &viewport);
}


//SHADERS -------------------------------------------------------------

Microsoft::WRL::ComPtr<ID3D10Blob> CompileShader(const Engine::EShaderType type)
{
    Microsoft::WRL::ComPtr<ID3D10Blob> pBlob;
    ID3D10Blob* pErr;
    UINT shaderFlags = 0x00;

#if defined(DEBUG) || defined(_DEBUG)
    shaderFlags |= D3DCOMPILE_DEBUG;
    shaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

    //Retrieve the paths to the raw and compiled shaders from the Resource Pool
    std::basic_string<wchar_t> fxPath = ResourcePool::GetShaderPath(type);
    std::basic_string<wchar_t> cfxPath = ResourcePool::GetCompiledShaderPath(type);

    //Attempt to load the precompiled shader object.
    D3DReadFileToBlob(cfxPath.c_str(), pBlob.ReleaseAndGetAddressOf());

    //If the precompiled shader is not valid, compile the shader directly from source.
    if(pBlob == nullptr)
    {
        Log("Compiling shader %s from source.\n", Engine::WStringToString(fxPath).c_str());
        Time timer;
        timer.Reset();
        timer.Start();

        HR(D3DCompileFromFile(fxPath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, nullptr, _In_ "fx_5_0", shaderFlags, 0, pBlob.ReleaseAndGetAddressOf(), &pErr), ("Unable to compile shader %s from file", Engine::WStringToString(fxPath).c_str()));
        
        timer.Tick();
        ERR(pErr != nullptr, ("Shader Compilation Errors Found: \n%s", (char*)(pErr->GetBufferPointer())));
        Engine::Log("Compiled shader %s in %fs\n", Engine::WStringToString(fxPath).c_str(), timer.DeltaTime());
    }

    return pBlob;
}

Microsoft::WRL::ComPtr<ID3DX11Effect> LoadShader(const Engine::MeshRenderer& renderer, const Microsoft::WRL::ComPtr<ID3D11Device>& device)
{
    // ERR((renderer == nullptr), "renderer was nullptr!");
    //Attempt to retrieve the compiled shader from the resource pool
    Microsoft::WRL::ComPtr<ID3DX11Effect> shader = ResourcePool::GetShader(renderer.shader);

    //If the shader isn't present, compile it, then add it to the resource pool
    if(shader == nullptr)
    {
        auto cShader = CompileShader(renderer.shader);
        ERR((cShader == nullptr), "Compiled Shader was Nullptr!");

        HR(D3DX11CreateEffectFromMemory(cShader->GetBufferPointer(), cShader->GetBufferSize(), 0, device.Get(), shader.GetAddressOf()), "Shader Compilation Failed!");

        ResourcePool::AddShader(shader.Get(), renderer.shader);
    }

    return shader;
}

void SetShaderState(const Microsoft::WRL::ComPtr<ID3DX11Effect>& shader, const Engine::MeshRenderer& renderer, Matrix4x4& world, Camera& camera, const Microsoft::WRL::ComPtr<ID3D11Device>& device, const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context)
{
    // Use the default technique as a fallback
    Microsoft::WRL::ComPtr<ID3DX11EffectTechnique> tech = shader->GetTechniqueByIndex(0);
    if(!renderer.technique.empty())
    {
        //Attempt to retrieve the effect technique by name.
        tech = shader->GetTechniqueByName(renderer.technique.c_str());
    }
    ERR(!tech->IsValid(), "Technique Is Invalid!");

    //Retrieve the effect pass
    D3DX11_PASS_DESC passDesc = {};
    Microsoft::WRL::ComPtr<ID3DX11EffectPass> pass = tech->GetPassByIndex(0);   //TODO: Multiple shader pass support
    HR(pass->GetDesc(&passDesc), "Pass Description Acquisition Failed!");

    //Set shader variables and input layout
    auto _setMatrixVar = [&](const std::basic_string<char>& semantic, const Matrix4x4& mat)
    {
        Microsoft::WRL::ComPtr<ID3DX11EffectVariable> var = shader->GetVariableBySemantic(semantic.c_str());
        ERR(var->IsValid() == false, "Variable Semantic is Invalid!");
        HR(var->AsMatrix()->SetMatrix(reinterpret_cast<const float*>(&mat._matrix)), ("Unable to set variable %s", semantic.c_str()));   
    };

    auto _setClassVar = [&](const std::basic_string<char>& name, const void* pData, size_t size)
    {
        Microsoft::WRL::ComPtr<ID3DX11EffectVariable> var = shader->GetVariableByName(name.c_str());
        ERR(var->IsValid() == false, "Variable Name is Invalid!");
        HR(var->SetRawValue(pData, 0, size), ("Unable to set variable %s", name.c_str()));
    };

    auto _setFloatVar = [&](const std::basic_string<char>& semantic, const float& value)
    {
        Microsoft::WRL::ComPtr<ID3DX11EffectVariable> var = shader->GetVariableBySemantic(semantic.c_str());
        ERR(var->IsValid() == false, "Variable Semantic is Invalid!");
        HR(var->SetRawValue(&value, 0, sizeof(float)), ("Unable to set variable %s", semantic.c_str()));
    };

    auto _setVector2Var = [&](const std::basic_string<char>& semantic, const Vector2f& value)
    {
        Microsoft::WRL::ComPtr<ID3DX11EffectVariable> var = shader->GetVariableBySemantic(semantic.c_str());
        ERR(var->IsValid() == false, "Variable Semantic is Invalid!");
        HR(var->SetRawValue(&value, 0, sizeof(Engine::Vector2f)), ("Unable to set variable %s", semantic.c_str()));
    };

    auto _setVector3Var = [&](const std::basic_string<char>& semantic, const Vector3f& value)
    {
        Microsoft::WRL::ComPtr<ID3DX11EffectVariable> var = shader->GetVariableBySemantic(semantic.c_str());
        ERR(var->IsValid() == false, "Variable Semantic is Invalid!");
        HR(var->SetRawValue(&value, 0, sizeof(Engine::Vector3f)), ("Unable to set variable %s", semantic.c_str()));
    };

    auto _setVector4Var = [&](const std::basic_string<char>& semantic, const Vector4f& value)
    {
        Microsoft::WRL::ComPtr<ID3DX11EffectVariable> var = shader->GetVariableBySemantic(semantic.c_str());
        ERR(var->IsValid() == false, "Variable Semantic is Invalid!");
        HR(var->SetRawValue(&value, 0, sizeof(Engine::Vector4f)), ("Unable to set variable %s", semantic.c_str()));
    };

    auto _setTextureVar = [&](const std::basic_string<char>& semantic, const std::basic_string<wchar_t>& path)
    {
        if (!path.empty())
        {
            Microsoft::WRL::ComPtr<ID3DX11EffectVariable> var = shader->GetVariableBySemantic(semantic.c_str());
            ERR(var->IsValid() == false, "Variable Semantic is Invalid!");

            std::basic_string<wchar_t> rPath;
            //Convert the path to a relative path
            if (path.starts_with(L"Resources\\") == false) {
                rPath = L"Resources\\";
            }
            rPath.append(path);
            
            Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> tex = ResourcePool::GetTexture(rPath);

            //If the resource pool doesn't have the texture, Attempt to load it, then add it.
            if (tex.Get() == nullptr)
            {
                //If the texture type is .dds, use the DDS loaders.
                if (path.substr(path.find_last_of(L".") + 1) == L"dds") {
                    HR(DirectX::CreateDDSTextureFromFile(device.Get(), context.Get(), rPath.c_str(), nullptr, &tex), ("Unable to load Texture <%s>", Engine::WStringToString(rPath.c_str()).c_str()));
                }

                //Assume the texture type is WIC compatible otherwise.
                else {
                    HR(DirectX::CreateWICTextureFromFile(device.Get(), context.Get(), rPath.c_str(), nullptr, &tex), ("Unable to load Texture <%s>", Engine::WStringToString(rPath.c_str()).c_str()));
                }

                ResourcePool::AddTexture(tex.Get(), rPath);
            }

            ERR(tex.Get() == nullptr, "Texture Invalid!");

            var = shader->GetVariableBySemantic(semantic.c_str());
            HR(var->AsShaderResource()->SetResource(tex.Get()), ("Unable to set Effect Texture %s \nPath:%s", semantic.c_str(), Engine::WStringToString(path).c_str()));
        }
    };

    //Set the Input Layout
    {
        std::vector<D3D11_INPUT_ELEMENT_DESC> ieDesc = {};
        Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout = ResourcePool::GetInputLayout(renderer.shader);
        
        if (inputLayout.Get() == nullptr)
        {
            switch (renderer.shader)
            {
                using enum EShaderType;
            case(Basic):
                ieDesc.push_back({ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });
                break;
            case Blinn:
                ieDesc.push_back({ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });
                ieDesc.push_back({ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });
                ieDesc.push_back({ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });
                ieDesc.push_back({ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });
                ieDesc.push_back({ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });
                break;
            case SpriteRenderer:
                ieDesc.push_back({ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });
                ieDesc.push_back({ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });
                break;
            default:
                break;
            }

            //Cache Input Layout
            HR(device->CreateInputLayout(&ieDesc.front(), (UINT)ieDesc.size(), passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, inputLayout.GetAddressOf()), "Error: Input Layout Creation Failed.");
            ResourcePool::AddInputLayout(inputLayout, renderer.shader);
        }

        ERR(inputLayout.Get() == nullptr, "Input layout is Nullptr!");
        context->IASetInputLayout(inputLayout.Get());
        
    }
    //Set Effect Variables
    {
        const Matrix4x4 viewProjection = Engine::Math::MatrixMultiply(camera.viewMatrix, camera.projMatrix);
        const Matrix4x4 worldViewProj = Engine::Math::MatrixMultiply(world, viewProjection);
        
        const Matrix4x4 worldProj2D = Engine::Math::MatrixMultiply(world, camera.orthoProjMatrix);

        switch (renderer.shader)
        {
            using enum EShaderType;
        case(Basic):
            _setMatrixVar("WORLDVIEWPROJECTION", worldViewProj);
            _setVector4Var("COLOUR", Engine::Math::Normalize(((Engine::Basic*)renderer.material)->Diffuse));
            break;
        case(Blinn):
            _setMatrixVar("WORLD", world);
            _setMatrixVar("WORLDVIEWPROJECTION", worldViewProj);
            _setVector4Var("AMBIENT", Engine::Math::Normalize(((Engine::Blinn*)renderer.material)->Ambient));
            _setVector4Var("DIFFUSE", Engine::Math::Normalize(((Engine::Blinn*)renderer.material)->Diffuse));
            _setVector4Var("Specular", Engine::Math::Normalize(((Engine::Blinn*)renderer.material)->Specular));
            _setFloatVar("SPECULARPOWER", ((Engine::Blinn*)renderer.material)->SpecularPower);
            _setVector3Var("CAMERAPOSITION", camera.Position);
            _setTextureVar("T_DIFFUSE", ((Engine::Blinn*)renderer.material)->DiffuseMap);
            _setTextureVar("T_NORMAL", ((Engine::Blinn*)renderer.material)->NormalMap);
            _setTextureVar("T_SPECULAR", ((Engine::Blinn*)renderer.material)->SpecularMap);
            _setClassVar("pointLights", (void*)(&m_lights.pointLights), sizeof(PointLight) * NUM_POINT_LIGHTS);
            _setClassVar("directionalLight", (void*)(&m_lights.directional), sizeof(DirectionalLight));
            _setClassVar("spotLights", (void*)(& m_lights.spotLights), sizeof(SpotLight)* NUM_SPOT_LIGHTS);
            break;
        case SpriteRenderer:
            _setMatrixVar("WORLDVIEWPROJECTION", worldProj2D);
            _setTextureVar("T_ATLAS", ((Engine::SpriteRenderer*)renderer.material)->TextureAtlas);
            break;
        default:
            break;
        }
    }

    pass->Apply(0, context.Get());

}


//MODEL LOADING --------------------------------------------------------

void CreateBuffers(const Engine::MeshFilter& mesh, Engine::Camera& camera, const EShaderType& type, const Microsoft::WRL::ComPtr<ID3D11Device>& device, const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context, bool cacheBuffer = true)
{
    unsigned vertexSize = 0;
    unsigned stride = 0;
    unsigned offset = 0;

    std::bitset<10> attributeFlags;

    //Set the attribute flags based on the shader type
    switch(type)
    {
        using enum EShaderType;
    case(Basic):
        attributeFlags.set((size_t)EVertexAttributes::Position);
        break;
    case(Blinn):
        attributeFlags.set((size_t)EVertexAttributes::Position);
        attributeFlags.set((size_t)EVertexAttributes::TexCoord);
        attributeFlags.set((size_t)EVertexAttributes::Normal);
        attributeFlags.set((size_t)EVertexAttributes::Tangent);
        attributeFlags.set((size_t)EVertexAttributes::Binormal);
    break;
    case SpriteRenderer:
        attributeFlags.set((size_t)EVertexAttributes::Position);
        attributeFlags.set((size_t)EVertexAttributes::TexCoord);
        break;
    default:
        break;
    }

    //Set the correct vertex size (4 bytes per float)
    attributeFlags.test((size_t)EVertexAttributes::Position) ? vertexSize += 3 : NULL;
    attributeFlags.test((size_t)EVertexAttributes::TexCoord) ? vertexSize += 2 : NULL;
    attributeFlags.test((size_t)EVertexAttributes::Normal) ? vertexSize += 3 : NULL;
    attributeFlags.test((size_t)EVertexAttributes::Tangent) ? vertexSize += 3 : NULL;
    attributeFlags.test((size_t)EVertexAttributes::Binormal) ? vertexSize += 3 : NULL;

    Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
    if (cacheBuffer) {
        vertexBuffer = ResourcePool::GetVertexBuffer(&mesh);
    }
    if (vertexBuffer.Get() == nullptr) {
        std::vector<float> verts;
        verts.reserve(mesh.Vertices.size() * vertexSize);

        //Assume all models include vertex positions
        for (size_t i = 0; i < mesh.Vertices.size(); i++)
        {
            //Load Positions, and other attributes, based on flags
            if (attributeFlags.test((size_t)EVertexAttributes::Position) && !mesh.Vertices.empty())
            {
                verts.emplace_back(mesh.Vertices[i].x);
                verts.emplace_back(mesh.Vertices[i].y);
                verts.emplace_back(mesh.Vertices[i].z);
            }

            if (attributeFlags.test((size_t)EVertexAttributes::TexCoord))
            {
                if (!mesh.TexCoords.empty())
                {
                    verts.emplace_back(mesh.TexCoords[i].x);
                    verts.emplace_back(mesh.TexCoords[i].y);
                }
                else
                {
                    //Log("Warning: <%s> Mesh texture coordinates are invalid. Defaulting to null.\n", mesh.Name);
                    verts.emplace_back(0.0f);
                    verts.emplace_back(0.0f);
                }
            }

            if (attributeFlags.test((size_t)EVertexAttributes::Normal))
            {
                if (!mesh.Normals.empty())
                {
                    verts.emplace_back(mesh.Normals[i].x);
                    verts.emplace_back(mesh.Normals[i].y);
                    verts.emplace_back(mesh.Normals[i].z);
                }
                else
                {
                    verts.emplace_back(0.0f);
                    verts.emplace_back(0.0f);
                    verts.emplace_back(0.0f);
                }
            }

            if (attributeFlags.test((size_t)EVertexAttributes::Tangent))
            {
                if (!mesh.Tangents.empty())
                {
                    verts.emplace_back(mesh.Tangents[i].x);
                    verts.emplace_back(mesh.Tangents[i].y);
                    verts.emplace_back(mesh.Tangents[i].z);
                }
                else
                {
                    verts.emplace_back(0.0f);
                    verts.emplace_back(0.0f);
                    verts.emplace_back(0.0f);
                }
            }

            if (attributeFlags.test((size_t)EVertexAttributes::Binormal))
            {
                if (!mesh.Binormals.empty())
                {
                    verts.emplace_back(mesh.Binormals[i].x);
                    verts.emplace_back(mesh.Binormals[i].y);
                    verts.emplace_back(mesh.Binormals[i].z);
                }
                else
                {
                    verts.emplace_back(0.0f);
                    verts.emplace_back(0.0f);
                    verts.emplace_back(0.0f);
                }
            }
        }

        verts.shrink_to_fit();

        //Bind the Vertex Buffer
        if (!verts.empty())
        {
            D3D11_BUFFER_DESC vbd;
            ZeroMemory(&vbd, sizeof(vbd));
            vbd.ByteWidth = (UINT)verts.size() * sizeof(float);
            vbd.Usage = D3D11_USAGE_IMMUTABLE;
            vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

            D3D11_SUBRESOURCE_DATA vertexSubresourceData = {};
            ZeroMemory(&vertexSubresourceData, sizeof(vertexSubresourceData));

            
            vertexSubresourceData.pSysMem = verts.data();
            HR(device->CreateBuffer(&vbd, &vertexSubresourceData, vertexBuffer.ReleaseAndGetAddressOf()), "Vertex Buffer Creation Failed!");
        }

        ResourcePool::AddVertexBuffer(vertexBuffer.Get(), &mesh);
    }

    stride = sizeof(float) * vertexSize;
    offset = 0;

    ERR(vertexBuffer.Get() == nullptr, "Vertex Buffer is Invalid!");

    context->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);

    Microsoft::WRL::ComPtr<ID3D11Buffer> ib;

    if (cacheBuffer) {
        ib = ResourcePool::GetIndexBuffer(&mesh);
    }
    //Load the Index Buffer
    if (!mesh.Indices.empty()) {
        if (ib.Get() == nullptr) {
            //If the index buffer doesn't exist, Load model indices
            D3D11_BUFFER_DESC ibd = {};
            ibd.Usage = D3D11_USAGE_DEFAULT;
            ibd.ByteWidth = (UINT)(mesh.Indices.size() * sizeof(int));
            ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
            ibd.CPUAccessFlags = 0;
            ibd.MiscFlags = 0;
            ibd.StructureByteStride = sizeof(int);

            D3D11_SUBRESOURCE_DATA iInitData = {};
            iInitData.pSysMem = &mesh.Indices[0];

            device->CreateBuffer(&ibd, &iInitData, ib.ReleaseAndGetAddressOf());

            ResourcePool::AddIndexBuffer(ib.Get(), &mesh);
        }

        ERR(ib.Get() == nullptr, "Index Buffer is Invalid!");
        context->IASetIndexBuffer(ib.Get(), DXGI_FORMAT_R32_UINT, 0);
    }
        
}

void SetPrimitiveTopology(const Engine::MeshRenderer renderer,const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context)
{
    switch (renderer.topology)
    {
    case(Engine::EPrimitiveTopology::Points):
        context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
        break;
    case(Engine::EPrimitiveTopology::TriangleList):
        context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        break;
    case(Engine::EPrimitiveTopology::TriangleStrip):
        context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
        break;
    case(Engine::EPrimitiveTopology::LineList):
        context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
        break;
    case(Engine::EPrimitiveTopology::LineStrip):
        context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
        break;

    default:
        break;
    }
}



//DX11 Graphics Class Methods -----------------------------------------------------------------

/**
 * \brief Initializes graphics within a window.
 * \param window The Window to initialize graphics in.
 * \param mode The Graphics Mode - describing how to render the graphics.
 * \return 
 */
bool DX11_GFX::Init(Engine::Window& window, Engine::GraphicsMode mode)
{

    //DEBUG TEMP
    m_lights.directional = { .colour{1, 1, 1, 0.2}, .direction{0.5f, -0.5f, 0.8f} };
    m_lights.pointLights[0] = { .colour{0.8f, 0.1f, 0.4f, 0.6f}, .position{0.0f, 5.0f, 0.0f}, .radius{65.0f} };
    //m_lights.pointLights[1] = { .colour{1.0f, 1.0f, 0.0f, 1.0f}, .position{3.0f, 10.0f, -50.0f}, .radius{85.0f} };
    //m_lights.pointLights[2] = { .colour{1.0f, 0.0f, 1.0f, 1.0f}, .position{15.0f, 30.0f, -40.0f}, .radius{105.0f} };
    //m_lights.pointLights[3] = { .colour{0.0f, 0.0f, 1.0f, 1.0f}, .position{20.0f, 20.0f, -20.0f}, .radius{40.0f} };

    
    //Validate the window handle
    ERR(window.m_WindowHandle == nullptr, "Window Handle is invalid!");

    CreateDX11Device(m_pDevice, &m_FeatureLevel, m_pContext);
    HR(m_pDevice->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &m_MSAAQuality), "\n4x MSAA is not supported by this device.");

    CreateDX11SwapChain(window.m_WindowHandle, m_pDevice, m_pSwapChain, mode);

    CreateDX11Views(m_pDevice, m_pContext, m_pSwapChain, m_pRenderTargetView, m_pDepthStencilView, m_BackBufferDesc, m_Viewport, mode);

    window.m_Gfx = this;
    return true;
}

/**
 * \brief Clears the window to a specified colour (8-bit RGBA format).
 */
void DX11_GFX::Clear(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{ 
    const float clr[4] = { (float)r / 255.0f, (float)g / 255.0f, (float)b / 255.0f, (float)a / 255.0f };
    m_pContext->ClearRenderTargetView(m_pRenderTargetView.Get(), clr);
    m_pContext->ClearDepthStencilView(m_pDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

/**
 * \brief Presents the graphics to the screen
 */
void DX11_GFX::Present()
{
    m_pSwapChain->Present(0, 0);
}

/**
 * \brief Changes the graphics display mode
 * \param window 
 * \param mode 
 */
void DX11_GFX::SetGraphicsMode(const Window& window, GraphicsMode mode)
{
    m_pSwapChain->SetFullscreenState(mode.isFullscreen, nullptr);

    m_gMode = mode;
}

/**
 * \brief Draws Geometry to the screen.
 * \param worldMatrix 
 * \param mesh 
 * \param renderer 
 * \param camera 
 */
void DX11_GFX::Draw(Matrix4x4& worldMatrix, const MeshFilter& mesh, const MeshRenderer& renderer, Camera& camera)
{
    //Draw the model using its world matrix, and the camera's view and projection matrices

    //Load the appropriate shader
    auto shader = LoadShader(renderer, m_pDevice);

    //Bind the vertex and index buffers to the pipeline
    CreateBuffers(mesh, camera, renderer.shader, m_pDevice, m_pContext);

    //Set shader parameters
    SetShaderState(shader, renderer, worldMatrix, camera, m_pDevice, m_pContext);


    //Set the primitive topology appropriately
    SetPrimitiveTopology(renderer, m_pContext);

    //...

    if (!mesh.Indices.empty()) {
        m_pContext->DrawIndexed((UINT)mesh.Indices.size(), 0, 0);
    }
    else if(!mesh.Vertices.empty())
    {
        m_pContext->Draw((UINT)mesh.Vertices.size(), 0);
    }


}/**W
 * \brief Draws Text to the screen.
 * \param worldMatrix 
 * \param sprite 
 * \param renderer 
 * \param camera 
 */
void DX11_GFX::Draw(Matrix4x4& worldMatrix, const Sprite& sprite, Camera& camera)
{
    //Draw the model using its world matrix, and the camera's view and projection matrices

    //Load the appropriate shader
    auto shader = LoadShader(sprite.m_Renderer, m_pDevice);

    //Bind the vertex and index buffers to the pipeline
    CreateBuffers(sprite.m_Sprite, camera, sprite.m_Renderer.shader, m_pDevice, m_pContext, false); //NOTE: Hack to get text rendering properly without spritebatching. 

    //Set shader parameters
    SetShaderState(shader, sprite.m_Renderer, worldMatrix, camera, m_pDevice, m_pContext);


    //Set the primitive topology appropriately
    SetPrimitiveTopology(sprite.m_Renderer, m_pContext);

    //...

    if (!sprite.m_Sprite.Indices.empty()) {
        m_pContext->DrawIndexed((UINT)sprite.m_Sprite.Indices.size(), 0, 0);
    }
    else if(!sprite.m_Sprite.Vertices.empty())
    {
        m_pContext->Draw((UINT)sprite.m_Sprite.Vertices.size(), 0);
    }
    
}
