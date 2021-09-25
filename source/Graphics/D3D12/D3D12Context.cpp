//     _____                        ______             _            
//    / ____|                      |  ____|           (_)           
//   | (___   __ _ _   _  ___ ___  | |__   _ __   __ _ _ _ __   ___ 
//    \___ \ / _` | | | |/ __/ _ \ |  __| | '_ \ / _` | | '_ \ / _ \
//    ____) | (_| | |_| | (_|  __/ | |____| | | | (_| | | | | |  __/
//   |_____/ \__,_|\__,_|\___\___| |______|_| |_|\__, |_|_| |_|\___|
//                                                __/ |             
//                                               |___/              
// Copyright (C) 2011-2020
// Made by Marcus "Bitsauce" Vergara
// Distributed under the MIT license

#include <Sauce/Graphics.h>

#include <Graphics/D3D12/D3D12Context.h>

BEGIN_SAUCE_NAMESPACE

// Pipeline objects.
CD3DX12_VIEWPORT m_viewport;
CD3DX12_RECT m_scissorRect;
ComPtr<IDXGISwapChain4> m_swapChain;
ComPtr<ID3D12Device> m_device;
ComPtr<ID3D12Resource> m_renderTargets[2];
ComPtr<ID3D12CommandAllocator> m_commandAllocator;
ComPtr<ID3D12CommandQueue> m_commandQueue;
ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
UINT m_rtvDescriptorSize;
UINT m_cbvSrvUavDescriptorSize;

/**************************************************
 * Shader reflection data                         *
 **************************************************/

struct ShaderVariable
{
    uint32 dataSize;
    uint32 dataOffset;
};

struct ShaderConstantBuffer
{
    uint8* cbufferData;
    uint32 cbufferDataSize;
    unordered_map<string, ShaderVariable*> members;
};

/**************************************************
 * Shader device object structs                   *
 **************************************************/

struct ModelViewConstants
{
    Matrix4 ModelViewProj;
};

struct ShaderResourceHandle
{
    virtual ~ShaderResourceHandle() {}

    int32 handleOffset;
};

struct ShaderSrvHandle : public ShaderResourceHandle
{
    Texture2DRef texture;
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;
};

struct ShaderCbvHandle : public ShaderResourceHandle
{
    ComPtr<ID3D12Resource> constantBuffer;
    int32 constantBufferSize;
    D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
    ShaderConstantBuffer* cbufferMetadata;
};


/**************************************************
 * Device object definitions                      *
 **************************************************/

struct D3D12Texture2DDeviceObject : public Texture2DDeviceObject
{
    ComPtr<ID3D12Resource> texture;
};

struct D3D12ShaderDeviceObject : public ShaderDeviceObject
{
    ComPtr<ID3DBlob> vertexShader;
    ComPtr<ID3DBlob> pixelShader;
    ComPtr<ID3D12RootSignature> rootSignature;
    unordered_map<string, ShaderConstantBuffer*> uniformMap;

    unordered_map<string, ShaderResourceHandle*> cbvSrvUavHandles;
    int32 srvHandlesStartingOffset;
};

struct D3D12RenderTarget2DDeviceObject : public RenderTarget2DDeviceObject
{
};

struct D3D12VertexBufferDeviceObject : public VertexBufferDeviceObject
{
};

struct D3D12IndexBufferDeviceObject : public IndexBufferDeviceObject
{
};

/**************************************************
 * Error handling functions                       *
 **************************************************/

#ifdef SAUCE_DEBUG
void handleD3DError(const string& d3dCall, const HRESULT errorCode)
{
    THROW("handleD3DError(): returned 0x%X from function %s", errorCode, d3dCall.c_str());
}
#endif

#ifdef SAUCE_DEBUG
#define D3D_VERIFY(call) { HRESULT hr = call; if (FAILED(hr)) { handleD3DError(#call, hr); } }
#else
#define D3D_VERIFY(call) call
#endif

DXGI_FORMAT toDXGIVertexFormat(const Datatype datatype, const uint32 elementCount)
{
    switch (datatype)
    {
        case Datatype::Float:
        {
            switch (elementCount)
            {
                case 1:  return DXGI_FORMAT_R32_FLOAT;
                case 2:  return DXGI_FORMAT_R32G32_FLOAT;
                case 3:  return DXGI_FORMAT_R32G32B32_FLOAT;
                case 4:  return DXGI_FORMAT_R32G32B32A32_FLOAT;
                default: return DXGI_FORMAT_UNKNOWN;
            }
        }
        break;
        case Datatype::Uint32:
        {
            switch (elementCount)
            {
                case 1:  return DXGI_FORMAT_R32_UINT;
                case 2:  return DXGI_FORMAT_R32G32_UINT;
                case 3:  return DXGI_FORMAT_R32G32B32_UINT;
                case 4:  return DXGI_FORMAT_R32G32B32A32_UINT;
                default: return DXGI_FORMAT_UNKNOWN;
            }
        }
        break;
        case Datatype::Int32:
        {
            switch (elementCount)
            {
                case 1:  return DXGI_FORMAT_R32_SINT;
                case 2:  return DXGI_FORMAT_R32G32_SINT;
                case 3:  return DXGI_FORMAT_R32G32B32_SINT;
                case 4:  return DXGI_FORMAT_R32G32B32A32_SINT;
                default: return DXGI_FORMAT_UNKNOWN;
            }
        }
        break;
        case Datatype::Uint16:
        {
            switch (elementCount)
            {
                case 1:  return DXGI_FORMAT_R16_UINT;
                case 2:  return DXGI_FORMAT_R16G16_UINT;
                case 3:  return DXGI_FORMAT_UNKNOWN;
                case 4:  return DXGI_FORMAT_R16G16B16A16_UINT;
                default: return DXGI_FORMAT_UNKNOWN;
            }
        }
        break;
        case Datatype::Int16:
        {
            switch (elementCount)
            {
                case 1:  return DXGI_FORMAT_R16_SINT;
                case 2:  return DXGI_FORMAT_R16G16_SINT;
                case 3:  return DXGI_FORMAT_UNKNOWN;
                case 4:  return DXGI_FORMAT_R16G16B16A16_SINT;
                default: return DXGI_FORMAT_UNKNOWN;
            }
        }
        break;
        case Datatype::Uint8:
        {
            switch (elementCount)
            {
                case 1:  return DXGI_FORMAT_R8_UINT;
                case 2:  return DXGI_FORMAT_R8G8_UINT;
                case 3:  return DXGI_FORMAT_UNKNOWN;
                case 4:  return DXGI_FORMAT_R8G8B8A8_UINT;
                default: return DXGI_FORMAT_UNKNOWN;
            }
        }
        break;
        case Datatype::Int8:
        {
            switch (elementCount)
            {
                case 1:  return DXGI_FORMAT_R8_SINT;
                case 2:  return DXGI_FORMAT_R8G8_SINT;
                case 3:  return DXGI_FORMAT_UNKNOWN;
                case 4:  return DXGI_FORMAT_R8G8B8A8_SINT;
                default: return DXGI_FORMAT_UNKNOWN;
            }
        }
        break;
    }
    return DXGI_FORMAT_UNKNOWN;
}

string D3D12Context::PSOKey::getHash() const
{
    stringstream hash;
    for (uint32 vertexAttributeIndex = 0; vertexAttributeIndex < (uint32)VertexAttribute::Max; vertexAttributeIndex++)
    {
        const VertexAttribute attribute = VertexAttribute(vertexAttributeIndex);
        if (vertexFormat.isAttributeEnabled(attribute))
        {
            const uint32   attributeOffset = vertexFormat.getAttributeOffset(attribute);
            const Datatype attributeDatatype = vertexFormat.getDatatype(attribute);
            const uint32   attributeElementCount = vertexFormat.getElementCount(attribute);
            hash << "1" << attributeOffset << (uint32)attributeDatatype << attributeElementCount;
        }
        else
        {
            hash << "0";
        }
    }
    hash << shader;
    return hash.str();
}

D3D12Context::D3D12Context()
{
}

D3D12Context::~D3D12Context()
{
}

bool D3D12Context::initialize(DescType graphicsContextDesc)
{
    m_owningWindow = graphicsContextDesc.owningWindow;
    const Vector2I frameBufferSize = m_owningWindow->getSize();

    m_viewport = CD3DX12_VIEWPORT(0.0f, 0.0f, frameBufferSize.x, frameBufferSize.y);
    m_scissorRect = CD3DX12_RECT(0, 0, frameBufferSize.x, frameBufferSize.y);
    m_frameIndex = 0;
    m_rtvDescriptorSize = 0;
    //m_immediateDrawVertexBufferSize = 0;

	// TODO:
    UINT dxgiFactoryFlags = 0;
	if (graphicsContextDesc.enableDebugLayer)
	{
		ComPtr<ID3D12Debug> debugController;
        D3D_VERIFY(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)));
		debugController->EnableDebugLayer();
        dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
	}

    ComPtr<IDXGIFactory6> factory;
    D3D_VERIFY(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&factory)));
	D3D_VERIFY(D3D12CreateDevice(NULL, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_device)));

    // Describe and create the command queue.
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    D3D_VERIFY(m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_commandQueue)));

    // Describe and create the swap chain.
    DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
    swapChainDesc.BufferCount = 2;
    swapChainDesc.BufferDesc.Width = frameBufferSize.x;
    swapChainDesc.BufferDesc.Height = frameBufferSize.y;
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.OutputWindow = (HWND)m_owningWindow->getOSHandle();
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.Windowed = TRUE;

    ComPtr<IDXGISwapChain> swapChainBase;
    D3D_VERIFY(factory->CreateSwapChain(
        m_commandQueue.Get(),
        &swapChainDesc,
        &swapChainBase
    ));
    D3D_VERIFY(swapChainBase.As(&m_swapChain));

    // We will handle ALT + ENTER ourselves
    D3D_VERIFY(factory->MakeWindowAssociation((HWND)m_owningWindow->getOSHandle(), DXGI_MWA_NO_ALT_ENTER));

    uint32 frameIndex = m_swapChain->GetCurrentBackBufferIndex();

    // Create descriptor heaps
    {
        // Describe and create a render target view (RTV) descriptor heap.
        D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
        rtvHeapDesc.NumDescriptors = 2;
        rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        D3D_VERIFY(m_device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_rtvHeap)));
        m_rtvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

        m_cbvSrvUavDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    }

    // Create back buffer render targets
    {
        CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());

        // Create a RTV for each frame.
        for (UINT n = 0; n < 2; n++)
        {
            D3D_VERIFY(m_swapChain->GetBuffer(n, IID_PPV_ARGS(&m_renderTargets[n])));
            m_device->CreateRenderTargetView(m_renderTargets[n].Get(), nullptr, rtvHandle);
            rtvHandle.Offset(1, m_rtvDescriptorSize);
        }
    }

    D3D_VERIFY(m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocator)));

    // Create fence objects for the command queue
    {
        D3D_VERIFY(m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_commandQueueFence)));
        m_commandQueueFenceValue = 1;

        // Create an event handle to use for frame synchronization.
        m_commandQueueFenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        if (m_commandQueueFenceEvent == nullptr)
        {
            D3D_VERIFY(HRESULT_FROM_WIN32(GetLastError()));
        }
    }

    // Create default shader
    {
        const string shaderSource =
            "cbuffer ModelViewConstants : register(b0)\n"
            "{\n"
            "    float4x4 u_ModelViewProj;\n"
            "}\n"
            "\n"
            "Texture2D u_Texture : register(t0);\n"
            "SamplerState u_Sampler : register(s0);\n"
            "\n"
            "struct PSInput\n"
            "{\n"
            "    float4 position : SV_POSITION;\n"
            "    float4 color    : COLOR;\n"
            "    float2 texCoord : TEXCOORD;\n"
            "};\n"
            "\n"
            "PSInput VSMain(float3 position : POSITION, float2 texCoord : TEXCOORD, float4 color : COLOR)\n"
            "{\n"
            "    PSInput result;\n"
            "    result.position = mul(float4(position.x, position.y, position.z, 1.0f), u_ModelViewProj);\n"
            "    result.texCoord = texCoord;\n"
            "    result.color = color;\n"
            "    return result;\n"
            "}\n"
            "\n"
            "float4 PSMain(PSInput input) : SV_TARGET\n"
            "{\n"
            "    return u_Texture.Sample(u_Sampler, input.texCoord) * input.color;\n"
            "}\n";

        ShaderDesc shaderDesc;
        shaderDesc.debugName = "DefaultShader";
        shaderDesc.shaderSourceVS = shaderSource;
        shaderDesc.shaderSourcePS = shaderSource;
        s_defaultShader = CreateNew<Shader>(shaderDesc);
    }

    // Create blank texture
    {
        uint8 pixel[4];
        //pixel[0] = pixel[1] = pixel[2] = pixel[3] = 255;
        pixel[0] = 255; pixel[1] = 200; pixel[2] = 200; pixel[3] = 255;
        Pixmap pixmap(1, 1, PixelFormat(PixelComponents::Rgba, PixelDatatype::Uint8), pixel);

        Texture2DDesc textureDesc;
        textureDesc.debugName = "DefaultTexture";
        textureDesc.pixmap = &pixmap;
        textureDesc.wrapping = TextureWrapping::Repeat;
        textureDesc.filtering = TextureFiltering::Nearest;
        s_defaultTexture = CreateNew<Texture2D>(textureDesc);
    }

    {
        // We default to an ortographic projection where top-left is (0, 0) and bottom-right is (w, h)
        setProjectionMatrix(createOrtographicMatrix(0, frameBufferSize.x, 0, frameBufferSize.y));

        setSize(frameBufferSize.x, frameBufferSize.y);
    }

    // Flush the command queue before submitting a new command list
    flushCommandQueue();

    // Clear back buffer render targets
    {
        ComPtr<ID3D12GraphicsCommandList> commandList;
        D3D_VERIFY(m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator.Get(), nullptr, IID_PPV_ARGS(&commandList)));

        // Indicate that the back buffer will be used as a render target.
        auto transition = CD3DX12_RESOURCE_BARRIER::Transition(m_renderTargets[m_frameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
        commandList->ResourceBarrier(1, &transition);

        CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart(), m_frameIndex, m_rtvDescriptorSize);
        commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

        // Record commands.
        const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
        commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

        D3D_VERIFY(commandList->Close());

        // Execute the command list.
        ID3D12CommandList* ppCommandLists[] = { commandList.Get() };
        m_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
    }

	return true;
}

ComPtr<ID3D12PipelineState> D3D12Context::createOrFindPSO(const VertexFormat& vertexFormat, ShaderDeviceObject** outShaderDeviceObject)
{
    // Define the vertex input layout
    vector<D3D12_INPUT_ELEMENT_DESC> inputElementDescs;
    for (uint32 vertexAttributeIndex = 0; vertexAttributeIndex < (uint32)VertexAttribute::Max; vertexAttributeIndex++)
    {
        const VertexAttribute attribute = VertexAttribute(vertexAttributeIndex);
        if (vertexFormat.isAttributeEnabled(attribute))
        {
            const uint64 attributeOffset = vertexFormat.getAttributeOffset(attribute);
            D3D12_INPUT_ELEMENT_DESC inputElementDesc;
            inputElementDesc.SemanticIndex = 0;
            inputElementDesc.InputSlot = 0;
            inputElementDesc.AlignedByteOffset = attributeOffset;
            inputElementDesc.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
            inputElementDesc.InstanceDataStepRate = 0;
            inputElementDesc.Format = toDXGIVertexFormat(vertexFormat.getDatatype(attribute), vertexFormat.getElementCount(attribute));
            switch (attribute)
            {
                case VertexAttribute::Position:
                    inputElementDesc.SemanticName = "POSITION";
                    break;

                case VertexAttribute::Color:
                    inputElementDesc.SemanticName = "COLOR";
                    break;

                case VertexAttribute::TexCoord:
                    inputElementDesc.SemanticName = "TEXCOORD";
                    break;

                case VertexAttribute::Normal:
                    inputElementDesc.SemanticName = "NORMAL";
                    break;

                default:
                    continue;
            }
            inputElementDescs.push_back(inputElementDesc);
        }
    }

    // Get D3D12 shader object
    ShaderRef shader = m_currentState->shader;
    if (!shader)
    {
        shader = s_defaultShader;
    }

    if (m_currentState->texture)
    {
        shader->setSampler2D("u_Texture", m_currentState->texture);
    }
    else if (!m_currentState->shader)
    {
        shader->setSampler2D("u_Texture", s_defaultTexture);
    }

    D3D12ShaderDeviceObject* shaderDeviceObject = getDeviceObject<D3D12ShaderDeviceObject>(shader);
    *outShaderDeviceObject = shaderDeviceObject;

    // Set projection matrix
    Matrix4 modelViewProjection = m_currentState->projectionMatrix * m_currentState->transformationMatrixStack.top();
    shader->setUniformMatrix4f("u_ModelViewProj", modelViewProjection.get());

    //m_constantBufferData.ModelViewProj = modelViewProjection;
    //memcpy(m_pCbvDataBegin, &m_constantBufferData, sizeof(m_constantBufferData));

    // Describe and create the graphics pipeline state object (PSO)
    PSOKey psoKey;
    psoKey.shader = shader;
    psoKey.vertexFormat = vertexFormat;
    const string psoKeyHash = psoKey.getHash();

    unordered_map<string, ComPtr<ID3D12PipelineState>>::iterator psoItr = m_availablePSOs.find(psoKeyHash);
    if (psoItr == m_availablePSOs.end())
    {
        D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
        psoDesc.InputLayout = { inputElementDescs.data(), (UINT)inputElementDescs.size() };
        psoDesc.pRootSignature = shaderDeviceObject->rootSignature.Get();
        psoDesc.VS = CD3DX12_SHADER_BYTECODE(shaderDeviceObject->vertexShader.Get());
        psoDesc.PS = CD3DX12_SHADER_BYTECODE(shaderDeviceObject->pixelShader.Get());
        psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
        psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
        psoDesc.DepthStencilState.DepthEnable = FALSE;
        psoDesc.DepthStencilState.StencilEnable = FALSE;
        psoDesc.SampleMask = UINT_MAX;
        psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        psoDesc.NumRenderTargets = 1;
        psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
        psoDesc.SampleDesc.Count = 1;

        ComPtr<ID3D12PipelineState> pso;
        D3D_VERIFY(m_device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pso)));
        m_availablePSOs[psoKeyHash] = pso;
        return pso;
    }
    return m_availablePSOs[psoKeyHash];
}

void D3D12Context::executeDrawCommandList(ShaderDeviceObject* shaderDeviceObjectBase, ComPtr<ID3D12PipelineState> pso, const D3D12_VERTEX_BUFFER_VIEW& vertexBufferView)
{
    D3D12ShaderDeviceObject* shaderDeviceObject = dynamic_cast<D3D12ShaderDeviceObject*>(shaderDeviceObjectBase);
    assert(shaderDeviceObject);

    // Create a CBV, SRV, UAV heap used for this draw call only
    D3D12_DESCRIPTOR_HEAP_DESC cbvSrvUavHeapDesc = {};
    cbvSrvUavHeapDesc.NumDescriptors = shaderDeviceObject->srvHandlesStartingOffset + 1; // TODO: Proper formula here
    cbvSrvUavHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    cbvSrvUavHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

    ComPtr<ID3D12DescriptorHeap> cbvSrvUavHeap;
    D3D_VERIFY(m_device->CreateDescriptorHeap(&cbvSrvUavHeapDesc, IID_PPV_ARGS(&cbvSrvUavHeap)));
    m_inflightCbvSrvUavHeaps.push_back(cbvSrvUavHeap);

    // TODO: Would be more optimized if we just started with handle at offset 0
    // and incremented it with cbvHandle.Offset() while iterating a sorted list

    for (const pair<string, ShaderResourceHandle*>& uniformKV : shaderDeviceObject->cbvSrvUavHandles)
    {
        ShaderCbvHandle* cbvHandle = dynamic_cast<ShaderCbvHandle*>(uniformKV.second);
        ShaderSrvHandle* srvHandle = dynamic_cast<ShaderSrvHandle*>(uniformKV.second);
        if (cbvHandle)
        {
            const int32 handleOffset = cbvHandle->handleOffset;
            CD3DX12_CPU_DESCRIPTOR_HANDLE cbvHandleOffset(cbvSrvUavHeap->GetCPUDescriptorHandleForHeapStart(), handleOffset, m_cbvSrvUavDescriptorSize);
            m_device->CreateConstantBufferView(&cbvHandle->cbvDesc, cbvHandleOffset);

            // Map and initialize the constant buffer. We don't unmap this until the
            // app closes. Keeping things mapped for the lifetime of the resource is okay.
            CD3DX12_RANGE readRange(0, 0); // We do not intend to read from this resource on the CPU.
            UINT8* cbvDataBegin = nullptr;
            D3D_VERIFY(cbvHandle->constantBuffer->Map(0, &readRange, reinterpret_cast<void**>(&cbvDataBegin)));
            assert(cbvDataBegin);

            // Copy constant buffer mem
            const ShaderConstantBuffer* const cbuffer = cbvHandle->cbufferMetadata;
            memcpy(cbvDataBegin, cbuffer->cbufferData, cbuffer->cbufferDataSize);
            cbvHandle->constantBuffer->Unmap(0, &readRange);
        }
        else if (srvHandle)
        {
            // TODO: Don't recreate them if they already exist
            const int32 handleOffset = srvHandle->handleOffset;
            D3D12Texture2DDeviceObject* textureDeviceObject = getDeviceObject<D3D12Texture2DDeviceObject>(srvHandle->texture);
            CD3DX12_CPU_DESCRIPTOR_HANDLE srvHandlesStartingOffset(cbvSrvUavHeap->GetCPUDescriptorHandleForHeapStart(), handleOffset, m_cbvSrvUavDescriptorSize);
            m_device->CreateShaderResourceView(textureDeviceObject->texture.Get(), &srvHandle->srvDesc, srvHandlesStartingOffset);
        }
        else
        {
            assert(false);
        }
    }

    // Create the command list
    ComPtr<ID3D12GraphicsCommandList> commandList;
    D3D_VERIFY(m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator.Get(), pso.Get(), IID_PPV_ARGS(&commandList)));

    // Set necessary state.
    commandList->SetGraphicsRootSignature(shaderDeviceObject->rootSignature.Get());

    ID3D12DescriptorHeap* ppHeaps[] = { cbvSrvUavHeap.Get() };
    commandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
    
    commandList->SetGraphicsRootDescriptorTable(0, cbvSrvUavHeap->GetGPUDescriptorHandleForHeapStart());
    CD3DX12_GPU_DESCRIPTOR_HANDLE srvHandle(cbvSrvUavHeap->GetGPUDescriptorHandleForHeapStart(), shaderDeviceObject->srvHandlesStartingOffset, m_cbvSrvUavDescriptorSize); // TODO: This won't work; hardcoded to offset 1
    commandList->SetGraphicsRootDescriptorTable(1, srvHandle);

    commandList->RSSetViewports(1, &m_viewport);
    commandList->RSSetScissorRects(1, &m_scissorRect);

    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart(), m_frameIndex, m_rtvDescriptorSize);
    commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

    // Record commands.
    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    commandList->IASetVertexBuffers(0, 1, &vertexBufferView);
    commandList->DrawInstanced(3, 1, 0, 0);

    D3D_VERIFY(commandList->Close());

    // Execute the command list
    ID3D12CommandList* ppCommandLists[] = { commandList.Get() };
    m_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
    
    flushCommandQueue(); // TODO: This should not be here... Need some way of making sure that any future draw call doesn't change the underlying object while the command list executes though
}

void D3D12Context::flushCommandQueue()
{
    // Signal and increment the fence value
    const UINT64 fence = m_commandQueueFenceValue;
    D3D_VERIFY(m_commandQueue->Signal(m_commandQueueFence.Get(), fence));
    m_commandQueueFenceValue++;

    // Wait until the command queue has finised executing
    if (m_commandQueueFence->GetCompletedValue() < fence)
    {
        D3D_VERIFY(m_commandQueueFence->SetEventOnCompletion(fence, m_commandQueueFenceEvent));
        WaitForSingleObject(m_commandQueueFenceEvent, INFINITE);
    }
}

void D3D12Context::enable(const Capability cap)
{
}

void D3D12Context::disable(const Capability cap)
{
}

bool D3D12Context::isEnabled(const Capability cap)
{
	return false;
}

void D3D12Context::enableScissor(const int x, const int y, const int w, const int h)
{
}

void D3D12Context::disableScissor()
{
}

void D3D12Context::setPointSize(const float pointSize)
{
}

void D3D12Context::setLineWidth(const float lineWidth)
{
}

void D3D12Context::setViewportSize(const uint w, const uint h)
{
}

void D3D12Context::clear(const uint32 clearMask, const Color& clearColor, const double clearDepth, const int32 clearStencil)
{
    // Clear values from current back buffer render target(s)
    {
        // Create a command list to clear the color render target
        ComPtr<ID3D12GraphicsCommandList> commandList;
        D3D_VERIFY(m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator.Get(), nullptr, IID_PPV_ARGS(&commandList)));
        CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart(), m_frameIndex, m_rtvDescriptorSize);
        commandList->ClearRenderTargetView(rtvHandle, (float*)&clearColor, 0, nullptr);
        D3D_VERIFY(commandList->Close());

        // Execute the command list
        ID3D12CommandList* ppCommandLists[] = { commandList.Get() };
        m_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
    }
}

void D3D12Context::saveScreenshot(string filePath)
{
}

Matrix4 D3D12Context::createOrtographicMatrix(const float l, const float r, const float t, const float b, const float n, const float f) const
{
    // Returns an ortographic projection matrix (typically for 2D rendering)
	Matrix4 mat(
		2.0f / (r - l), 0.0f,            0.0f,           -((r + l) / (r - l)),
		0.0f,           2.0f / (t - b),  0.0f,           -((t + b) / (t - b)),
		0.0f,           0.0f,           -2.0f / (f - n), -((f + n) / (f - n)),
		0.0f,           0.0f,            0.0f,            1.0f);
	return mat;
}

Matrix4 D3D12Context::createPerspectiveMatrix(const float fov, const float aspectRatio, const float zNear, const float zFar) const
{
	return Matrix4();
}

Matrix4 D3D12Context::createLookAtMatrix(const Vector3F& position, const Vector3F& fwd) const
{
	return Matrix4();
}

void D3D12Context::drawIndexedPrimitives(const PrimitiveType type, const VertexArray& vertices, const uint vertexCount, const uint* indices, const uint indexCount)
{
}

void D3D12Context::drawIndexedPrimitives(const PrimitiveType type, const VertexBufferRef vertexBuffer, const IndexBufferRef indexBuffer)
{
}

void D3D12Context::drawPrimitives(const PrimitiveType type, const VertexArray& vertices, const uint vertexCount)
{
    // If there are no vertices to draw, do nothing
    if (vertexCount == 0)
    {
        return;
    }

    const uint32 vertexBufferKey = vertices.getVertexDataSize();
    ImmediateVertexBuffer immediateVertexBuffer;

    // Check if we already have a free vertex buffer of the requested size
    unordered_map<uint32, vector<ImmediateVertexBuffer>>::iterator itr =
        m_availableImmediateDrawVertexBuffers.find(vertexBufferKey);
    if (itr != m_availableImmediateDrawVertexBuffers.end())
    {
        vector<ImmediateVertexBuffer>& availableVertexBuffers = m_availableImmediateDrawVertexBuffers[vertexBufferKey];
        immediateVertexBuffer = availableVertexBuffers.back();
        availableVertexBuffers.pop_back();
        if (availableVertexBuffers.empty())
        {
            m_availableImmediateDrawVertexBuffers.erase(vertexBufferKey);
        }
    }
    else
    {
        // Note: using upload heaps to transfer static data like vert buffers is not 
        // recommended. Every time the GPU needs it, the upload heap will be marshalled 
        // over. Please read up on Default Heap usage. An upload heap is used here for 
        // code simplicity and because there are very few verts to actually transfer.
        ComPtr<ID3D12Resource> vertexBuffer;
        auto heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
        auto buffer = CD3DX12_RESOURCE_DESC::Buffer(vertices.getVertexDataSize());
        m_device->CreateCommittedResource(
            &heapProperties,
            D3D12_HEAP_FLAG_NONE,
            &buffer,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            NULL,
            IID_PPV_ARGS(&vertexBuffer));

        // Create vertex buffer view
        D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
        vertexBufferView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
        vertexBufferView.StrideInBytes = vertices.getVertexFormat().getVertexSizeInBytes();
        vertexBufferView.SizeInBytes = vertices.getVertexDataSize();

        // Copy the vertex data to the vertex buffer resource
        uint8* vertexDataMapped;
        CD3DX12_RANGE readRange(0, 0); // Pass a (0, 0) range as we're not reading data on the CPU
        D3D_VERIFY(vertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&vertexDataMapped)));
        memcpy(vertexDataMapped, vertices.getVertexData(), vertices.getVertexDataSize());
        vertexBuffer->Unmap(0, NULL);

        // Store buffer for reuse
        immediateVertexBuffer.vertexBuffer = vertexBuffer;
        immediateVertexBuffer.vertexBufferView = vertexBufferView;
        m_immediateDrawVertexBuffers[vertexBufferKey].push_back(immediateVertexBuffer);
    }

    //setupContext();
    ShaderDeviceObject* shaderDeviceObject;
    ComPtr<ID3D12PipelineState> pso = createOrFindPSO(vertices.getVertexFormat(), &shaderDeviceObject);
    executeDrawCommandList(shaderDeviceObject, pso, immediateVertexBuffer.vertexBufferView);
}

void D3D12Context::drawPrimitives(const PrimitiveType type, const VertexBufferRef vertexBuffer)
{
}

void D3D12Context::presentFrame()
{
    // Indicate that the back buffer will now be used to present
    {
        ComPtr<ID3D12GraphicsCommandList> commandList;
        D3D_VERIFY(m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator.Get(), nullptr, IID_PPV_ARGS(&commandList)));
        auto transition = CD3DX12_RESOURCE_BARRIER::Transition(m_renderTargets[m_frameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
        commandList->ResourceBarrier(1, &transition);
        D3D_VERIFY(commandList->Close());

        // Execute the command list
        ID3D12CommandList* ppCommandLists[] = { commandList.Get() };
        m_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
    }

    // Present the frame
    D3D_VERIFY(m_swapChain->Present(1, 0));
    flushCommandQueue();
    m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();

    //m_activePSOs.clear();
    //m_immediateDrawVertexBufferView.clear();
    //m_immediateDrawVertexBuffer.clear();
    
    m_inflightCbvSrvUavHeaps.clear(); // TODO: Instead of clearing these it would be better to reuse them in the next frame

    m_availableImmediateDrawVertexBuffers = m_immediateDrawVertexBuffers;

    // Command list allocators can only be reset when the associated 
    // command lists have finished execution on the GPU; apps should use 
    // fences to determine GPU execution progress.
    D3D_VERIFY(m_commandAllocator->Reset());

    // Indicate that the back buffer will be used as a render target
    {
        // Create a command list to transition the back buffer into a render target state
        ComPtr<ID3D12GraphicsCommandList> commandList;
        D3D_VERIFY(m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator.Get(), nullptr, IID_PPV_ARGS(&commandList)));
        auto transition = CD3DX12_RESOURCE_BARRIER::Transition(m_renderTargets[m_frameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
        commandList->ResourceBarrier(1, &transition);
        D3D_VERIFY(commandList->Close());

        // Execute the command list
        ID3D12CommandList* ppCommandLists[] = { commandList.Get() };
        m_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
    }
}

void D3D12Context::texture2D_createDeviceObject(Texture2DDeviceObject*& outTextureDeviceObject, const string& deviceObjectName)
{
    // TODO:
    //texture->SetName("DebugName");

    D3D12Texture2DDeviceObject* textureDeviceObject = new D3D12Texture2DDeviceObject();
    outTextureDeviceObject = textureDeviceObject;
}

void D3D12Context::texture2D_destroyDeviceObject(Texture2DDeviceObject*& outTextureDeviceObject)
{
    D3D12Texture2DDeviceObject* textureDeviceObject = dynamic_cast<D3D12Texture2DDeviceObject*>(outTextureDeviceObject);
    assert(textureDeviceObject);
    delete textureDeviceObject;
    outTextureDeviceObject = nullptr;
}

void D3D12Context::texture2D_copyToGPU(Texture2DDeviceObject* textureDeviceObjectBase, const PixelFormat pixelFormat, const uint32 width, const uint32 height, uint8* textureData)
{
    D3D12Texture2DDeviceObject* textureDeviceObject = dynamic_cast<D3D12Texture2DDeviceObject*>(textureDeviceObjectBase);
    assert(textureDeviceObject);

    // Describe and create a Texture2D
    D3D12_RESOURCE_DESC textureDesc = {};
    textureDesc.MipLevels = 1;
    textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // TODO: Custom texture format
    textureDesc.Width = width;
    textureDesc.Height = height;
    textureDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
    textureDesc.DepthOrArraySize = 1;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.SampleDesc.Quality = 0;
    textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;

    ComPtr<ID3D12Resource> texture;
    {
        auto heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
        D3D_VERIFY(m_device->CreateCommittedResource(
            &heapProperties,
            D3D12_HEAP_FLAG_NONE,
            &textureDesc,
            D3D12_RESOURCE_STATE_COPY_DEST,
            nullptr,
            IID_PPV_ARGS(&texture)));
    }

    const UINT64 uploadBufferSize = GetRequiredIntermediateSize(texture.Get(), 0, 1);

    // Create the GPU upload buffer
    ComPtr<ID3D12Resource> textureUploadHeap;
    {
        auto heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
        auto buffer = CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize);
        D3D_VERIFY(m_device->CreateCommittedResource(
            &heapProperties,
            D3D12_HEAP_FLAG_NONE,
            &buffer,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&textureUploadHeap)));
    }

    // Copy data to the intermediate upload heap and then schedule a copy 
    // from the upload heap to the Texture2D
    D3D12_SUBRESOURCE_DATA textureUploadData = {};
    textureUploadData.pData = textureData;
    textureUploadData.RowPitch = width * pixelFormat.getDataTypeSizeInBytes();
    textureUploadData.SlicePitch = textureUploadData.RowPitch * height;

    // Upload texture data immediately
    {
        ComPtr<ID3D12GraphicsCommandList> commandList;
        D3D_VERIFY(m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator.Get(), nullptr, IID_PPV_ARGS(&commandList)));
        UpdateSubresources(commandList.Get(), texture.Get(), textureUploadHeap.Get(), 0, 0, 1, &textureUploadData);
        auto transition = CD3DX12_RESOURCE_BARRIER::Transition(texture.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
        commandList->ResourceBarrier(1, &transition);
        D3D_VERIFY(commandList->Close());

        // Execute the command list
        ID3D12CommandList* ppCommandLists[] = { commandList.Get() };
        m_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
    }

    textureDeviceObject->texture = texture;

    flushCommandQueue();
}

void D3D12Context::texture2D_copyToCPUReadable(Texture2DDeviceObject* textureDeviceObject, uint8** outTextureData)
{
}

void D3D12Context::texture2D_updateSubregion(Texture2DDeviceObject* textureDeviceObject, const uint32 x, const uint32 y, const uint32 subRegionWidth, const uint32 subRegionHeight, uint8* textureData)
{
}

void D3D12Context::texture2D_updateFiltering(Texture2DDeviceObject* textureDeviceObject, const TextureFiltering filtering)
{
}

void D3D12Context::texture2D_updateWrapping(Texture2DDeviceObject* textureDeviceObject, const TextureWrapping wrapping)
{
}

void D3D12Context::texture2D_clearTexture(Texture2DDeviceObject* textureDeviceObject)
{
}

/**************************************************
 * Shader internal API implementation             *
 **************************************************/

ComPtr<ID3DBlob> D3D12Context::compileAndReflect(ShaderDeviceObject* shaderDeviceObjectBase, const string& shaderCode, const string& shaderMain, const string& shaderTarget, ReflectionInfo& outReflectionInfo)
{
    D3D12ShaderDeviceObject* shaderDeviceObject = dynamic_cast<D3D12ShaderDeviceObject*>(shaderDeviceObjectBase);
    assert(shaderDeviceObject);

#if defined(_DEBUG) // TODO: Use runtime flag instead
    // Enable better shader debugging with the graphics debugging tools
    UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
    UINT compileFlags = 0;
#endif

    ComPtr<ID3DBlob> compiledShader;
    ComPtr<ID3DBlob> errorMessages;
    HRESULT hr = D3DCompile(shaderCode.c_str(), shaderCode.size(), NULL, NULL, NULL, shaderMain.c_str(), shaderTarget.c_str(), compileFlags, 0, &compiledShader, &errorMessages);
    if (FAILED(hr))
    {
        if (errorMessages)
        {
            THROW("%s", (char*)errorMessages->GetBufferPointer());
        }
        else
        {
            handleD3DError("D3DCompile()", hr);
        }
    }

    ComPtr<ID3D12ShaderReflection> shaderReflection;
    D3D_VERIFY(D3DReflect(compiledShader->GetBufferPointer(), compiledShader->GetBufferSize(), IID_PPV_ARGS(&shaderReflection)));

    int32 cbvSrvUavOffset = 0;

    D3D12_SHADER_DESC shaderDesc;
    D3D_VERIFY(shaderReflection->GetDesc(&shaderDesc));
    for (uint32 cbufferIndex = 0; cbufferIndex < shaderDesc.ConstantBuffers; ++cbufferIndex)
    {
        ID3D12ShaderReflectionConstantBuffer* cbuffer = shaderReflection->GetConstantBufferByIndex(cbufferIndex);
        D3D12_SHADER_BUFFER_DESC cbufferDesc;
        D3D_VERIFY(cbuffer->GetDesc(&cbufferDesc));

        ShaderConstantBuffer* cbufferMetadata = new ShaderConstantBuffer;
        cbufferMetadata->cbufferData = new uint8[cbufferDesc.Size];
        cbufferMetadata->cbufferDataSize = cbufferDesc.Size;

        for (uint32 variableIndex = 0; variableIndex < cbufferDesc.Variables; ++variableIndex)
        {
            ID3D12ShaderReflectionVariable* variable = cbuffer->GetVariableByIndex(variableIndex);
            D3D12_SHADER_VARIABLE_DESC variableDesc;
            D3D_VERIFY(variable->GetDesc(&variableDesc));

            ShaderVariable* memberMetadata = new ShaderVariable;
            memberMetadata->dataOffset = variableDesc.StartOffset;
            memberMetadata->dataSize = variableDesc.Size;
            cbufferMetadata->members[variableDesc.Name] = memberMetadata;

            // Map variable name to constant buffer
            shaderDeviceObject->uniformMap[variableDesc.Name] = cbufferMetadata;
        }

        // Create the constant buffer
        {
            ComPtr<ID3D12Resource> constantBuffer;
            auto heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
            auto buffer = CD3DX12_RESOURCE_DESC::Buffer((cbufferDesc.Size + 255) & ~255);
            D3D_VERIFY(m_device->CreateCommittedResource(
                &heapProperties,
                D3D12_HEAP_FLAG_NONE,
                &buffer,
                D3D12_RESOURCE_STATE_GENERIC_READ,
                nullptr,
                IID_PPV_ARGS(&constantBuffer))
            );

            // Create a ConstantBufferView handle
            D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
            cbvDesc.BufferLocation = constantBuffer->GetGPUVirtualAddress();
            cbvDesc.SizeInBytes = (cbufferDesc.Size + 255) & ~255; // CB size is required to be 256-byte aligned

            ShaderCbvHandle* cbvHandle = new ShaderCbvHandle();
            cbvHandle->handleOffset = cbvSrvUavOffset;
            cbvHandle->constantBuffer = constantBuffer;
            cbvHandle->constantBufferSize = cbufferDesc.Size;
            cbvHandle->cbvDesc = cbvDesc;
            cbvHandle->cbufferMetadata = cbufferMetadata;

            shaderDeviceObject->cbvSrvUavHandles[cbufferDesc.Name] = cbvHandle;
            cbvSrvUavOffset++;
        }
    }

    // Iterate and keep track of all bindable resource
    for (uint32 resourceIndex = 0; resourceIndex < shaderDesc.BoundResources; ++resourceIndex)
    {
        D3D12_SHADER_INPUT_BIND_DESC resourceDesc;
        D3D_VERIFY(shaderReflection->GetResourceBindingDesc(resourceIndex, &resourceDesc));
        LOG("Name: %s, BindPoint: %i", resourceDesc.Name, resourceDesc.BindPoint);

        switch (resourceDesc.Type)
        {
            case D3D_SIT_CBUFFER:
            {
                outReflectionInfo.numCbuffers++;
                outReflectionInfo.cbufferBindPointMax = max(outReflectionInfo.cbufferBindPointMax, resourceDesc.BindPoint);
            }
            break;

            case D3D_SIT_TEXTURE:
            {
                ShaderSrvHandle* srvHandle = new ShaderSrvHandle();
                srvHandle->handleOffset = cbvSrvUavOffset;
                shaderDeviceObject->cbvSrvUavHandles[resourceDesc.Name] = srvHandle;

                outReflectionInfo.numSrvs++;
                outReflectionInfo.srvBindPointMax = max(outReflectionInfo.srvBindPointMax, resourceDesc.BindPoint);
            }
            break;

            case D3D_SIT_SAMPLER:
            {
                outReflectionInfo.numSamplers++;
                outReflectionInfo.samplerBindPointMax = max(outReflectionInfo.samplerBindPointMax, resourceDesc.BindPoint);
            }
            break;
        }
        cbvSrvUavOffset++;
    }

    return compiledShader;
}

void D3D12Context::shader_createDeviceObject(ShaderDeviceObject*& outShaderDeviceObject, const string& deviceObjectName)
{
    D3D12ShaderDeviceObject* shaderDeviceObject = new D3D12ShaderDeviceObject();
    outShaderDeviceObject = shaderDeviceObject;
}

void D3D12Context::shader_destroyDeviceObject(ShaderDeviceObject*& outShaderDeviceObject)
{
    D3D12ShaderDeviceObject* shaderDeviceObject = dynamic_cast<D3D12ShaderDeviceObject*>(outShaderDeviceObject);
    assert(shaderDeviceObject);
    delete shaderDeviceObject;
    outShaderDeviceObject = nullptr;
}

void D3D12Context::shader_compileShader(ShaderDeviceObject* shaderDeviceObjectBase, const string& vsSource, const string& psSource, const string& gsSource)
{
    D3D12ShaderDeviceObject* shaderDeviceObject = dynamic_cast<D3D12ShaderDeviceObject*>(shaderDeviceObjectBase);
    assert(shaderDeviceObject);

    // Create the pipeline state, which includes compiling and loading shaders
    ReflectionInfo vsReflectionInfo, psReflectionInfo;
    shaderDeviceObject->vertexShader = compileAndReflect(shaderDeviceObjectBase, vsSource, "VSMain", "vs_5_0", vsReflectionInfo);
    shaderDeviceObject->pixelShader = compileAndReflect(shaderDeviceObjectBase, psSource, "PSMain", "ps_5_0", psReflectionInfo);

    //// TODO: Should probably be moved to compileShader
    // Create a default root signature
    {
        shaderDeviceObject->srvHandlesStartingOffset = max(vsReflectionInfo.cbufferBindPointMax, psReflectionInfo.cbufferBindPointMax) + 1;

        CD3DX12_DESCRIPTOR_RANGE1 ranges[2];
        ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, max(vsReflectionInfo.cbufferBindPointMax, psReflectionInfo.cbufferBindPointMax) + 1, 0);
        ranges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, max(vsReflectionInfo.srvBindPointMax, psReflectionInfo.srvBindPointMax) + 1, 0);

        CD3DX12_ROOT_PARAMETER1 rootParameters[2];
        rootParameters[0].InitAsDescriptorTable(1, &ranges[0], D3D12_SHADER_VISIBILITY_ALL);
        rootParameters[1].InitAsDescriptorTable(1, &ranges[1], D3D12_SHADER_VISIBILITY_ALL);

        D3D12_STATIC_SAMPLER_DESC sampler = {};
        sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
        sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        sampler.MipLODBias = 0;
        sampler.MaxAnisotropy = 0;
        sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
        sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
        sampler.MinLOD = 0.0f;
        sampler.MaxLOD = D3D12_FLOAT32_MAX;
        sampler.ShaderRegister = 0;
        sampler.RegisterSpace = 0;
        sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

        CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
        rootSignatureDesc.Init_1_1(_countof(rootParameters), rootParameters, 1, &sampler, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

        ComPtr<ID3DBlob> signature;
        ComPtr<ID3DBlob> errorMessages;
        D3D_VERIFY(D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_1, &signature, &errorMessages));
        D3D_VERIFY(m_device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&shaderDeviceObject->rootSignature)));

        if (errorMessages)
        {
            char* errorMessagesStr = (char*)errorMessages->GetBufferPointer();
            THROW("%s", errorMessagesStr);
        }
    }
}

void D3D12Context::shader_setUniform(ShaderDeviceObject* shaderDeviceObjectBase, const string& uniformName, const Datatype datatype, const uint32 numComponentsPerElement, const uint32 numElements, const void* data)
{
    D3D12ShaderDeviceObject* shaderDeviceObject = dynamic_cast<D3D12ShaderDeviceObject*>(shaderDeviceObjectBase);
    assert(shaderDeviceObject);

    unordered_map<string, ShaderConstantBuffer*>::iterator cbufferItr = shaderDeviceObject->uniformMap.find(uniformName);
    if (cbufferItr != shaderDeviceObject->uniformMap.end())
    {
        ShaderVariable* member = cbufferItr->second->members[uniformName];
        memcpy(cbufferItr->second->cbufferData, ((uint8*)data) + member->dataOffset, member->dataSize);
    }
}

void D3D12Context::shader_setSampler2D(ShaderDeviceObject* shaderDeviceObjectBase, const string& uniformName, Texture2DRef texture)
{
    D3D12ShaderDeviceObject* shaderDeviceObject = dynamic_cast<D3D12ShaderDeviceObject*>(shaderDeviceObjectBase);
    assert(shaderDeviceObject);

    ShaderResourceHandle* resourceHandle;
    {
        const unordered_map<string, ShaderResourceHandle*>::iterator& itr = shaderDeviceObject->cbvSrvUavHandles.find(uniformName);
        if (itr == shaderDeviceObject->cbvSrvUavHandles.end())
        {
            return;
        }
        resourceHandle = itr->second;
    }

    ShaderSrvHandle* srvHandle = dynamic_cast<ShaderSrvHandle*>(resourceHandle);
    srvHandle->texture = texture;
    {
        // Create a SRV desc for the texture
        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;//textureDesc.Format; // TODO:
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MipLevels = 1;
        srvHandle->srvDesc = srvDesc;
    }
}

void D3D12Context::renderTarget2D_createDeviceObject(RenderTarget2DDeviceObject*& outRenderTargetDeviceObject, const string& deviceObjectName)
{
}

void D3D12Context::renderTarget2D_destroyDeviceObject(RenderTarget2DDeviceObject*& outRenderTargetDeviceObject)
{
}

void D3D12Context::renderTarget2D_initializeRenderTarget(RenderTarget2DDeviceObject* renderTargetDeviceObject, const Texture2DRef* targetTextures, const uint32 targetCount)
{
}

void D3D12Context::renderTarget2D_bindRenderTarget(RenderTarget2DDeviceObject* renderTargetDeviceObject)
{
}

void D3D12Context::vertexBuffer_createDeviceObject(VertexBufferDeviceObject*& outVertexBufferDeviceObject, const string& deviceObjectName)
{
}

void D3D12Context::vertexBuffer_destroyDeviceObject(VertexBufferDeviceObject*& outVertexBufferDeviceObject)
{
}

void D3D12Context::vertexBuffer_initializeVertexBuffer(VertexBufferDeviceObject* vertexBufferDeviceObject, const BufferUsage bufferUsage, const VertexArray& vertices, const uint32 vertexCount)
{
}

void D3D12Context::vertexBuffer_modifyVertexBuffer(VertexBufferDeviceObject* vertexBufferDeviceObject, const uint32 startIndex, const VertexArray& vertices, const uint32 vertexCount)
{
}

void D3D12Context::vertexBuffer_bindVertexBuffer(VertexBufferDeviceObject* vertexBufferDeviceObject)
{
}

void D3D12Context::indexBuffer_createDeviceObject(IndexBufferDeviceObject*& outIndexBufferDeviceObject, const string& deviceObjectName)
{
}

void D3D12Context::indexBuffer_destroyDeviceObject(IndexBufferDeviceObject*& outIndexBufferDeviceObject)
{
}

void D3D12Context::indexBuffer_initializeIndexBuffer(IndexBufferDeviceObject* indexBufferDeviceObject, const BufferUsage bufferUsage, const uint32* indices, const uint32 indexCount)
{
}

void D3D12Context::indexBuffer_modifyIndexBuffer(IndexBufferDeviceObject* indexBufferDeviceObject, const uint32 startIndex, const uint32* indices, const uint32 indexCount)
{
}

void D3D12Context::indexBuffer_bindIndexBuffer(IndexBufferDeviceObject* indexBufferDeviceObject)
{
}

END_SAUCE_NAMESPACE
