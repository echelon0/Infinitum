
#define D3D_DEBUG 1

struct d3d12_framework {
    ID3D12Device *Device;
    ID3D12CommandQueue *CommandQueue;
    IDXGISwapChain3 *SwapChain3;
    ID3D12DescriptorHeap *CbvSrvUavDescriptorHeap;
    ID3D12Resource *UavCompute;
    ID3D12Resource *CbvCompute;
    ID3D12RootSignature *ComputeRootSignature;
    ID3D12PipelineState *ComputePSO;
    ID3D12CommandAllocator *CommandAllocator;
    ID3D12GraphicsCommandList *CommandList;
    ID3D12Fence *Fence;
    UINT64 FenceValue;
    HANDLE FenceEvent;
    u32 BackBufferCount;
};

struct upload_constants {
    float3 CameraPos;
    u32 pack1;
    float3 CameraDir;
    u32 pack2;
    float3 CameraRight;
    u32 pack3;    
    float3 CameraUp;
    u32 pack4;    
    f32 CameraFilmDist;
    u32 iTime;    
};

bool
InitD3D12(HWND WindowHandle, d3d12_framework *D3D12Framework) {
    HRESULT hr;

#if D3D_DEBUG
    ID3D12Debug *DebugController;
    D3D12GetDebugInterface(IID_PPV_ARGS(&DebugController));
    DebugController->EnableDebugLayer();
#endif
    
    hr = D3D12CreateDevice(0, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&D3D12Framework->Device));
    ReturnOnError(hr, "Direct3D 12 Error", "Failed to create device.");

#if D3D_DEBUG
    ID3D12InfoQueue *InfoQueue;
    if(SUCCEEDED(D3D12Framework->Device->QueryInterface(&InfoQueue))) {
        InfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
        InfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
        InfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, TRUE);
    }
#endif

    D3D12_COMMAND_QUEUE_DESC CommandQueueDesc = {};
    CommandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    CommandQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
    CommandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    CommandQueueDesc.NodeMask = 0;
    
    hr = D3D12Framework->Device->CreateCommandQueue(&CommandQueueDesc, IID_PPV_ARGS(&D3D12Framework->CommandQueue));
    ReturnOnError(hr, "Direct3D 12 Error", "Failed to create command queue");

    IDXGIFactory2 *DXGIFactory;
    hr = CreateDXGIFactory2(0, IID_PPV_ARGS(&DXGIFactory));
    ReturnOnError(hr, "Direct3D 12 Error", "Failed to create DXGIFactory object.");

    RECT WindowRect = {};
    GetWindowRect(WindowHandle, &WindowRect);
    u32 WindowWidth = WindowRect.right - WindowRect.left;
    u32 WindowHeight = WindowRect.bottom - WindowRect.top;
    
    D3D12Framework->BackBufferCount = 2;
    
    DXGI_SWAP_CHAIN_DESC1 SwapChainDesc = {};
    SwapChainDesc.Width = WindowWidth;
    SwapChainDesc.Height = WindowHeight;
    SwapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    SwapChainDesc.Stereo = 0;
    SwapChainDesc.SampleDesc.Count = 1;
    SwapChainDesc.SampleDesc.Quality = 0;
    SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    SwapChainDesc.BufferCount = D3D12Framework->BackBufferCount;
    SwapChainDesc.Scaling = DXGI_SCALING_STRETCH;
    SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    SwapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
    SwapChainDesc.Flags = 0;

    IDXGISwapChain1 *SwapChain1;    
    hr = DXGIFactory->CreateSwapChainForHwnd(D3D12Framework->CommandQueue, WindowHandle, &SwapChainDesc, 0, 0, &SwapChain1);
    ReturnOnError(hr, "Direct3D 12 Error", "Failed to create swap chain.");

    SwapChain1->QueryInterface(IID_PPV_ARGS(&D3D12Framework->SwapChain3));
    
    D3D12_DESCRIPTOR_HEAP_DESC DescriptorHeapDesc = {};
    DescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    DescriptorHeapDesc.NumDescriptors = 2;
    DescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    DescriptorHeapDesc.NodeMask = 0;

    hr = D3D12Framework->Device->CreateDescriptorHeap(&DescriptorHeapDesc, IID_PPV_ARGS(&D3D12Framework->CbvSrvUavDescriptorHeap));
    ReturnOnError(hr, "Direct3D 12 Error", "Failed to create CBV/SRV/UAV descriptor heap.");

    D3D12_CPU_DESCRIPTOR_HANDLE CbvSrvUavDescriptorHandle = D3D12Framework->CbvSrvUavDescriptorHeap->GetCPUDescriptorHandleForHeapStart();    

    D3D12_HEAP_PROPERTIES DefaultHeapProperties = D3D12HeapProperties(D3D12_HEAP_TYPE_DEFAULT);
    D3D12_HEAP_PROPERTIES UploadHeapProperties = D3D12HeapProperties(D3D12_HEAP_TYPE_UPLOAD);
    
    D3D12_RESOURCE_DESC UaResourceDesc = {};
    UaResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    UaResourceDesc.Alignment = 0;
    UaResourceDesc.Width = WindowWidth;
    UaResourceDesc.Height = WindowHeight;
    UaResourceDesc.DepthOrArraySize = 1;
    UaResourceDesc.MipLevels = 1;
    UaResourceDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    UaResourceDesc.SampleDesc.Count = 1;
    UaResourceDesc.SampleDesc.Quality = 0;
    UaResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    UaResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
    
    hr = D3D12Framework->Device->CreateCommittedResource(&DefaultHeapProperties, D3D12_HEAP_FLAG_NONE,
                                                         &UaResourceDesc, D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
                                                         0, IID_PPV_ARGS(&D3D12Framework->UavCompute));
    ReturnOnError(hr, "Direct3D 12 Error", "Failed to create committed resource: unordered access.");
    
    D3D12_UNORDERED_ACCESS_VIEW_DESC UavDesc = {};
    UavDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    UavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
    UavDesc.Texture2D.MipSlice = 0;
    UavDesc.Texture2D.PlaneSlice = 0;
    D3D12Framework->Device->CreateUnorderedAccessView(D3D12Framework->UavCompute, 0, &UavDesc, CbvSrvUavDescriptorHandle);

    D3D12_DESCRIPTOR_RANGE UavDescriptorRange = {};
    UavDescriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
    UavDescriptorRange.NumDescriptors = 1;
    UavDescriptorRange.BaseShaderRegister = 0;
    UavDescriptorRange.RegisterSpace = 0;
    UavDescriptorRange.OffsetInDescriptorsFromTableStart = 0;

    D3D12_RESOURCE_DESC CbResourceDesc = {};
    CbResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    CbResourceDesc.Alignment = 0;    
    CbResourceDesc.Width = sizeof(upload_constants) + 256 - sizeof(upload_constants) % 256;
    CbResourceDesc.Height = 1;
    CbResourceDesc.DepthOrArraySize = 1;
    CbResourceDesc.MipLevels = 1;    
    CbResourceDesc.Format = DXGI_FORMAT_UNKNOWN;
    CbResourceDesc.SampleDesc.Count = 1;
    CbResourceDesc.SampleDesc.Quality = 0;
    CbResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    CbResourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

    hr = D3D12Framework->Device->CreateCommittedResource(&UploadHeapProperties, D3D12_HEAP_FLAG_NONE,
                                                         &CbResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ,
                                                         0, IID_PPV_ARGS(&D3D12Framework->CbvCompute));
    ReturnOnError(hr, "Direct3D 12 Error", "Failed to create committed resource: constant buffer.");
    
    D3D12_CONSTANT_BUFFER_VIEW_DESC CbvDesc = {};
    CbvDesc.BufferLocation = D3D12Framework->CbvCompute->GetGPUVirtualAddress();
    CbvDesc.SizeInBytes = (UINT)CbResourceDesc.Width;
    D3D12Framework->Device->CreateConstantBufferView(&CbvDesc, {CbvSrvUavDescriptorHandle.ptr + D3D12Framework->Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)});

    D3D12_DESCRIPTOR_RANGE CbvDescriptorRange = {};
    CbvDescriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
    CbvDescriptorRange.NumDescriptors = 1;
    CbvDescriptorRange.BaseShaderRegister = 0;
    CbvDescriptorRange.RegisterSpace = 0;
    CbvDescriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

    D3D12_DESCRIPTOR_RANGE *DescriptorRanges = (D3D12_DESCRIPTOR_RANGE *)malloc(2 * sizeof(D3D12_DESCRIPTOR_RANGE));
    DescriptorRanges[0] = UavDescriptorRange;
    DescriptorRanges[1] = CbvDescriptorRange;
    
    D3D12_ROOT_PARAMETER ComputeRootParameter = {};
    ComputeRootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    ComputeRootParameter.DescriptorTable.NumDescriptorRanges = 2;
    ComputeRootParameter.DescriptorTable.pDescriptorRanges = DescriptorRanges;
    ComputeRootParameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
    
    D3D12_ROOT_SIGNATURE_DESC ComputeRootSignatureDesc = {};
    ComputeRootSignatureDesc.NumParameters = 1;
    ComputeRootSignatureDesc.pParameters = &ComputeRootParameter;
    ComputeRootSignatureDesc.NumStaticSamplers = 0;
    ComputeRootSignatureDesc.pStaticSamplers = 0;
    ComputeRootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_STREAM_OUTPUT;

    ID3DBlob *SerializedRootSignatureBlob;
    ID3DBlob *SerializerErrorBlob;
    hr = D3D12SerializeRootSignature(&ComputeRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &SerializedRootSignatureBlob, &SerializerErrorBlob);
    ReturnOnErrorBlob(hr, "Direct3D 12 Error", "Failed to serialize compute root signature.", SerializerErrorBlob);
    
    hr = D3D12Framework->Device->CreateRootSignature(0, SerializedRootSignatureBlob->GetBufferPointer(), SerializedRootSignatureBlob->GetBufferSize(), IID_PPV_ARGS(&D3D12Framework->ComputeRootSignature));
    ReturnOnError(hr, "Direct3D 12 Error", "Failed to create compute root signature.");

    LPCWSTR ShaderPath = L"../code/shader.hlsl";
    UINT ShaderCompilerFlags = D3DCOMPILE_OPTIMIZATION_LEVEL3;

    ID3DBlob *CSCodeBlob;
    ID3DBlob *CSErrorBlob = 0;
    hr = D3DCompileFromFile(ShaderPath, 0, 0, "CSMain", "cs_5_0", ShaderCompilerFlags, 0, &CSCodeBlob, &CSErrorBlob);
    ReturnOnErrorBlob(hr, "Direct3D 12 Error", "Failed to compile compute shader.", CSErrorBlob);

    D3D12_COMPUTE_PIPELINE_STATE_DESC ComputePipelineStateDesc = {};
    ComputePipelineStateDesc.pRootSignature = D3D12Framework->ComputeRootSignature;
    ComputePipelineStateDesc.CS.pShaderBytecode = CSCodeBlob->GetBufferPointer(); 
    ComputePipelineStateDesc.CS.BytecodeLength = CSCodeBlob->GetBufferSize();   
    ComputePipelineStateDesc.NodeMask = 0;
    ComputePipelineStateDesc.CachedPSO = {};
    ComputePipelineStateDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

    hr = D3D12Framework->Device->CreateComputePipelineState(&ComputePipelineStateDesc, IID_PPV_ARGS(&D3D12Framework->ComputePSO));
    ReturnOnError(hr, "Direct3D 12 Error", "Failed to create compute pipeline state object.");
    
    hr = D3D12Framework->Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&D3D12Framework->CommandAllocator));
    ReturnOnError(hr, "Direct3D 12 Error", "Failed to create command allocator.");

    hr = D3D12Framework->Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, D3D12Framework->CommandAllocator, D3D12Framework->ComputePSO, IID_PPV_ARGS(&D3D12Framework->CommandList));
    ReturnOnError(hr, "Direct3D 12 Error", "Failed to create command list.");

    hr = D3D12Framework->CommandList->Close();
    ReturnOnError(hr, "Direct3D 12 Error", "Failed to close command list at initialization.");    

    hr = D3D12Framework->Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&D3D12Framework->Fence));
    ReturnOnError(hr, "Direct3D 12 Error", "Failed to create fence.");

    D3D12Framework->FenceValue = 0;
    D3D12Framework->FenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    
    return true;
}

bool
Render(d3d12_framework *D3D12Framework, u32 WindowWidth, u32 WindowHeight, upload_constants *Constants) {
    void *CbPtr;
    D3D12_RANGE MemoryRange = { 0, sizeof(upload_constants) };    
    D3D12Framework->CbvCompute->Map(0, &MemoryRange, &CbPtr);
    //MemCopy(CbPtr, (void *)Constants, (u32)MemoryRange.End);
    upload_constants *CbStruct = (upload_constants *)CbPtr;
    CbStruct->iTime = Constants->iTime;
    CbStruct->CameraPos = Constants->CameraPos;
    CbStruct->CameraDir = Constants->CameraDir;
    CbStruct->CameraRight = Constants->CameraRight;
    CbStruct->CameraUp = Constants->CameraUp;
    CbStruct->CameraFilmDist = Constants->CameraFilmDist;
    D3D12Framework->CbvCompute->Unmap(0, &MemoryRange);
                      
    HRESULT hr;
    hr = D3D12Framework->CommandAllocator->Reset();
    ReturnOnError(hr, "Direct3D 12 Error", "Failed to reset command allocator.");

    hr = D3D12Framework->CommandList->Reset(D3D12Framework->CommandAllocator, NULL);
    ReturnOnError(hr, "Direct3D 12 Error", "Failed to reset command list.");

    D3D12Framework->CommandList->SetPipelineState(D3D12Framework->ComputePSO);
    D3D12Framework->CommandList->SetComputeRootSignature(D3D12Framework->ComputeRootSignature);
    D3D12Framework->CommandList->SetDescriptorHeaps(1, &D3D12Framework->CbvSrvUavDescriptorHeap);
    D3D12_GPU_DESCRIPTOR_HANDLE CbvSrvUavDescriptorHandle = D3D12Framework->CbvSrvUavDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
    D3D12Framework->CommandList->SetComputeRootDescriptorTable(0, CbvSrvUavDescriptorHandle);

    D3D12Framework->CommandList->Dispatch((UINT)ceil((f32)WindowWidth / 16.0f), (UINT)ceil((f32)WindowHeight / 16.0f), 1);

    UINT BackBufferIndex = D3D12Framework->SwapChain3->GetCurrentBackBufferIndex();
    ID3D12Resource *BackBuffer;
    hr = D3D12Framework->SwapChain3->GetBuffer(BackBufferIndex, IID_PPV_ARGS(&BackBuffer));
    ReturnOnError(hr, "Direct3D 12 Error", "Failed to retrieve back buffer.");

    D3D12_RESOURCE_BARRIER UavToSrcCopy = D3D12Transition(D3D12Framework->UavCompute, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);
    D3D12_RESOURCE_BARRIER SrcCopyToUav = D3D12Transition(D3D12Framework->UavCompute, D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
    D3D12_RESOURCE_BARRIER PresentToDestCopy = D3D12Transition(BackBuffer, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_COPY_DEST);
    D3D12_RESOURCE_BARRIER DestCopyToPresent = D3D12Transition(BackBuffer, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PRESENT);    
    
    D3D12Framework->CommandList->ResourceBarrier(1, &UavToSrcCopy);
    D3D12Framework->CommandList->ResourceBarrier(1, &PresentToDestCopy);
    
    D3D12Framework->CommandList->CopyResource(BackBuffer, D3D12Framework->UavCompute);
    
    D3D12Framework->CommandList->ResourceBarrier(1, &SrcCopyToUav);
    D3D12Framework->CommandList->ResourceBarrier(1, &DestCopyToPresent);

    D3D12Framework->CommandList->Close();
    
    ID3D12CommandList *CommandLists[] = {D3D12Framework->CommandList};
    D3D12Framework->CommandQueue->ExecuteCommandLists(1, CommandLists);
    
    hr = D3D12Framework->SwapChain3->Present(1, 0);
    ReturnOnError(hr, "Direct3D 12 Error", "Swap chain present failed.");

    D3D12Framework->FenceValue++;
    D3D12Framework->CommandQueue->Signal(D3D12Framework->Fence, D3D12Framework->FenceValue);
    D3D12Framework->Fence->SetEventOnCompletion(D3D12Framework->FenceValue, D3D12Framework->FenceEvent);
    WaitForSingleObject(D3D12Framework->FenceEvent, INFINITE);

    return true;
}

