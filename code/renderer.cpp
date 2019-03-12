
#include <D3Dcompiler.h>
#include <d3d11.h>
#include <d3d12.h>
#include <dxgi1_4.h>
#include <unknwn.h>
#include <comdef.h>

#define D3D_DEBUG 1

struct d3d12_state {
    ID3D12Device *Device;
    ID3D12CommandQueue *CommandQueue;
    IDXGISwapChain3 *SwapChain3;
    ID3D12DescriptorHeap *CbvSrvUavDescriptorHeap;
    ID3D12Resource *UavCompute;
    ID3D12RootSignature *ComputeRootSignature;
    ID3D12PipelineState *ComputePSO;
    ID3D12CommandAllocator *CommandAllocator;
    ID3D12GraphicsCommandList *CommandList;
    ID3D12Fence *Fence;
    UINT64 FenceValue;
    HANDLE FenceEvent;
    u32 BackBufferCount;
} D3D12State;

char *
CreateErrorMessage(char *SimpleMsg, HRESULT hr) {
    _com_error err(hr);
    char *ErrorMsg = (char *)malloc((StrLen(SimpleMsg) + StrLen((char *)err.ErrorMessage())) * sizeof(char) + (sizeof(char) * 3));
    StrCopy(ErrorMsg, SimpleMsg);
    StrCat(ErrorMsg, "\n\n");
    StrCat(ErrorMsg, (char *)err.ErrorMessage());
    return ErrorMsg;
}

void
DisplayErrorMessageBox(char *Title, char *SimpleMsg, HRESULT hr) {
    char *ErrorMsg = CreateErrorMessage(SimpleMsg, hr); 
    MessageBoxA(0, ErrorMsg, Title, MB_OK|MB_ICONERROR);
    free(ErrorMsg);
}

bool
InitD3D12(HWND WindowHandle) {
    HRESULT hr;

#if D3D_DEBUG
    ID3D12Debug *Debug;
    D3D12GetDebugInterface(IID_PPV_ARGS(&Debug));
    Debug->EnableDebugLayer();
#endif
    
    hr = D3D12CreateDevice(0, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&D3D12State.Device));
    if(FAILED(hr)) {
        DisplayErrorMessageBox("Direct3D 12 Error", "Failed to create device.", hr);
        return false;
    }

#if D3D_DEBUG
    ID3D12InfoQueue *InfoQueue;
    if(SUCCEEDED(D3D12State.Device->QueryInterface(&InfoQueue))) {
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
    
    hr = D3D12State.Device->CreateCommandQueue(&CommandQueueDesc, IID_PPV_ARGS(&D3D12State.CommandQueue));
    if(FAILED(hr)) {
        DisplayErrorMessageBox("Direct3D 12 Error", "Failed to create command queue.", hr);
        return false;
    }   

    IDXGIFactory2 *DXGIFactory;
    hr = CreateDXGIFactory2(0, IID_PPV_ARGS(&DXGIFactory));
    if(FAILED(hr)) {
        DisplayErrorMessageBox("Direct3D 12 Error", "Failed to create DXGIFactory object.", hr);
        return false;       
    }

    RECT WindowRect = {};
    GetWindowRect(WindowHandle, &WindowRect);
    u32 WindowWidth = WindowRect.right - WindowRect.left;
    u32 WindowHeight = WindowRect.bottom - WindowRect.top;
    
    D3D12State.BackBufferCount = 2;
    
    DXGI_SWAP_CHAIN_DESC1 SwapChainDesc = {};
    SwapChainDesc.Width = WindowWidth;
    SwapChainDesc.Height = WindowHeight;
    SwapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    SwapChainDesc.Stereo = 0;
    SwapChainDesc.SampleDesc.Count = 1;
    SwapChainDesc.SampleDesc.Quality = 0;
    SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    SwapChainDesc.BufferCount = D3D12State.BackBufferCount;
    SwapChainDesc.Scaling = DXGI_SCALING_STRETCH;
    SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    SwapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
    SwapChainDesc.Flags = 0;

    IDXGISwapChain1 *SwapChain1;    
    hr = DXGIFactory->CreateSwapChainForHwnd(D3D12State.CommandQueue, WindowHandle, &SwapChainDesc, 0, 0, &SwapChain1);
    if(FAILED(hr)) {
        DisplayErrorMessageBox("Direct3D 12 Error", "Failed to create swap chain.", hr);
        return false;
    }
    SwapChain1->QueryInterface(IID_PPV_ARGS(&D3D12State.SwapChain3));
    
    D3D12_DESCRIPTOR_HEAP_DESC DescriptorHeapDesc = {};
    DescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    DescriptorHeapDesc.NumDescriptors = 1;
    DescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    DescriptorHeapDesc.NodeMask = 0;

    hr = D3D12State.Device->CreateDescriptorHeap(&DescriptorHeapDesc, IID_PPV_ARGS(&D3D12State.CbvSrvUavDescriptorHeap));
    if(FAILED(hr)) {
        DisplayErrorMessageBox("Direct3D 12 Error", "Failed to create CBV/SRV/UAV descriptor heap.", hr);
        return false;   
    }
    
    D3D12_CPU_DESCRIPTOR_HANDLE CbvSrvUavDescriptorHandle = D3D12State.CbvSrvUavDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
    D3D12_UNORDERED_ACCESS_VIEW_DESC UavDesc = {};
    UavDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    UavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
    UavDesc.Buffer.FirstElement = 0;
    UavDesc.Buffer.NumElements = WindowWidth * WindowHeight;
    UavDesc.Buffer.StructureByteStride = 0;
    UavDesc.Buffer.CounterOffsetInBytes = 0;
    UavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;
    
    D3D12State.Device->CreateUnorderedAccessView(D3D12State.UavCompute, 0, &UavDesc, CbvSrvUavDescriptorHandle);

    D3D12_HEAP_PROPERTIES DefaultHeapProperties = {};
    DefaultHeapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
    DefaultHeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    DefaultHeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
    DefaultHeapProperties.CreationNodeMask = 0;
    DefaultHeapProperties.VisibleNodeMask = 0;

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
    
    hr = D3D12State.Device->CreateCommittedResource(&DefaultHeapProperties, D3D12_HEAP_FLAG_NONE, &UaResourceDesc, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, 0, IID_PPV_ARGS(&D3D12State.UavCompute));
    if(FAILED(hr)) {
        DisplayErrorMessageBox("Direct3D 12 Error", "Failed to create committed resource: unordered access.", hr);
        return false;
    }                                                    
    
    D3D12_DESCRIPTOR_RANGE UavDescriptorRange = {};
    UavDescriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
    UavDescriptorRange.NumDescriptors = 1;
    UavDescriptorRange.BaseShaderRegister = 0;
    UavDescriptorRange.RegisterSpace = 0;
    UavDescriptorRange.OffsetInDescriptorsFromTableStart = 0;
    
    D3D12_ROOT_PARAMETER ComputeRootParameter = {};
    ComputeRootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    ComputeRootParameter.DescriptorTable.NumDescriptorRanges = 1;
    ComputeRootParameter.DescriptorTable.pDescriptorRanges = &UavDescriptorRange;
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
    if(FAILED(hr)) {
        DisplayErrorMessageBox("Direct3D 12 Error", "Failed to serialize compute root signature.", hr);
        return false;
    }
    
    hr = D3D12State.Device->CreateRootSignature(0, SerializedRootSignatureBlob->GetBufferPointer(), SerializedRootSignatureBlob->GetBufferSize(), IID_PPV_ARGS(&D3D12State.ComputeRootSignature));
    if(FAILED(hr)) {
        char *SimpleMsg = "Failed to create compute root signature.\n\n";       
        char *ErrorMsg = (char *)malloc(StrLen(SimpleMsg) * sizeof(char) + SerializerErrorBlob->GetBufferSize() + sizeof(char));
        StrCopy(ErrorMsg, SimpleMsg);
        StrNCat(ErrorMsg, (char *)SerializerErrorBlob->GetBufferPointer(), SerializerErrorBlob->GetBufferSize());
        MessageBoxA(0, ErrorMsg, "Direct3D 12 Error", MB_OK|MB_ICONERROR);
        free(ErrorMsg);
    }
    

    LPCWSTR ShaderPath = L"../code/shader.hlsl";
    UINT ShaderCompilerFlags = D3DCOMPILE_OPTIMIZATION_LEVEL3;

    ID3DBlob *CSCodeBlob;
    ID3DBlob *CSErrorBlob = 0;
    hr = D3DCompileFromFile(ShaderPath, 0, 0, "CSMain", "cs_5_0", ShaderCompilerFlags, 0, &CSCodeBlob, &CSErrorBlob);
    if(FAILED(hr)) {
        if(!CSErrorBlob) {
            MessageBoxA(0, "Failed to compile compute shader.\n\nCould not retrieve compiler error messages.", "Direct3D 12 Error", MB_OK|MB_ICONERROR);
        } else {
            char *SimpleMsg = "Failed to compile compute shader.\n\n";      
            char *ErrorMsg = (char *)malloc(StrLen(SimpleMsg) * sizeof(char) + CSErrorBlob->GetBufferSize() + sizeof(char));
            StrCopy(ErrorMsg, SimpleMsg);
            StrNCat(ErrorMsg, (char *)CSErrorBlob->GetBufferPointer(), CSErrorBlob->GetBufferSize());
            MessageBoxA(0, ErrorMsg, "Direct3D 12 Error", MB_OK|MB_ICONERROR);
            free(ErrorMsg);
        }
        return false;
    }

    D3D12_COMPUTE_PIPELINE_STATE_DESC ComputePipelineStateDesc = {};
    ComputePipelineStateDesc.pRootSignature = D3D12State.ComputeRootSignature;
    ComputePipelineStateDesc.CS.pShaderBytecode = CSCodeBlob->GetBufferPointer(); 
    ComputePipelineStateDesc.CS.BytecodeLength = CSCodeBlob->GetBufferSize();   
    ComputePipelineStateDesc.NodeMask = 0;
    ComputePipelineStateDesc.CachedPSO = {};
    ComputePipelineStateDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

    hr = D3D12State.Device->CreateComputePipelineState(&ComputePipelineStateDesc, IID_PPV_ARGS(&D3D12State.ComputePSO));
    if(FAILED(hr)) {
        DisplayErrorMessageBox("Direct3D 12 Error", "Failed to create compute pipeline state object.", hr);
        return false;
    }
    
    hr = D3D12State.Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&D3D12State.CommandAllocator));
    if(FAILED(hr)) {
        DisplayErrorMessageBox("Direct3D 12 Error", "Failed to create command allocator.", hr);
        return false;   
    }

    hr = D3D12State.Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, D3D12State.CommandAllocator, D3D12State.ComputePSO, IID_PPV_ARGS(&D3D12State.CommandList));
    if(FAILED(hr)) {
        DisplayErrorMessageBox("Direct3D 12 Error", "Failed to create command list.", hr);
        return false;
    }

    hr = D3D12State.CommandList->Close();
    if(FAILED(hr)) {
        DisplayErrorMessageBox("Direct3D 12 Error", "Failed to close command list at initialization.", hr);
        return false;
    }

    hr = D3D12State.Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&D3D12State.Fence));
    if(FAILED(hr)) {
        DisplayErrorMessageBox("Direct3D 12 Error", "Failed to create fence.", hr);
        return false;   
    }
    
    D3D12State.FenceValue = 0;
    D3D12State.FenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    return true;
}

bool
Render(u32 WindowWidth, u32 WindowHeight) {
    HRESULT hr;

    hr = D3D12State.CommandAllocator->Reset();
    if(FAILED(hr)) {
        DisplayErrorMessageBox("Direct3D 12 Error", "Failed to reset command allocator.", hr);
        return false;
    }

    hr = D3D12State.CommandList->Reset(D3D12State.CommandAllocator, NULL);
    if(FAILED(hr)) {
        DisplayErrorMessageBox("Direct3D 12 Error", "Failed to reset command list.", hr);
        return false;
    }

    D3D12State.CommandList->SetPipelineState(D3D12State.ComputePSO);
    D3D12State.CommandList->SetComputeRootSignature(D3D12State.ComputeRootSignature);
    D3D12State.CommandList->SetDescriptorHeaps(1, &D3D12State.CbvSrvUavDescriptorHeap);
    D3D12_GPU_DESCRIPTOR_HANDLE CbvSrvUavDescriptorHandle = D3D12State.CbvSrvUavDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
    D3D12State.CommandList->SetComputeRootDescriptorTable(0, CbvSrvUavDescriptorHandle);

    D3D12State.CommandList->Dispatch((UINT)ceil((f32)WindowWidth / 16.0f), (UINT)ceil((f32)WindowHeight / 16.0f), 1);

    UINT BackBufferIndex = D3D12State.SwapChain3->GetCurrentBackBufferIndex();
    ID3D12Resource *BackBuffer;
    hr = D3D12State.SwapChain3->GetBuffer(BackBufferIndex, IID_PPV_ARGS(&BackBuffer));
    if(FAILED(hr)) {
        DisplayErrorMessageBox("Direct3D 12 Error", "Failed to retrieve back buffer.", hr);
        return false;
    }

    //Transition barriers
    D3D12_RESOURCE_BARRIER UaToSrcCopy = {};
    UaToSrcCopy.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    UaToSrcCopy.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    UaToSrcCopy.Transition.pResource = D3D12State.UavCompute;
    UaToSrcCopy.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    UaToSrcCopy.Transition.StateBefore = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
    UaToSrcCopy.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_SOURCE;
    
    D3D12_RESOURCE_BARRIER SrcCopyToUa = {};
    SrcCopyToUa.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    SrcCopyToUa.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    SrcCopyToUa.Transition.pResource = D3D12State.UavCompute;
    SrcCopyToUa.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    SrcCopyToUa.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_SOURCE;
    SrcCopyToUa.Transition.StateAfter = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;

    D3D12_RESOURCE_BARRIER PresentToDestCopy = {};
    PresentToDestCopy.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    PresentToDestCopy.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    PresentToDestCopy.Transition.pResource = BackBuffer;
    PresentToDestCopy.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    PresentToDestCopy.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
    PresentToDestCopy.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_DEST;
    
    D3D12_RESOURCE_BARRIER DestCopyToPresent = {};
    DestCopyToPresent.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    DestCopyToPresent.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    DestCopyToPresent.Transition.pResource = BackBuffer;
    DestCopyToPresent.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    DestCopyToPresent.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
    DestCopyToPresent.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
    
    D3D12State.CommandList->ResourceBarrier(1, &UaToSrcCopy);
    D3D12State.CommandList->ResourceBarrier(1, &PresentToDestCopy);
    D3D12State.CommandList->CopyResource(BackBuffer, D3D12State.UavCompute);
    D3D12State.CommandList->ResourceBarrier(1, &SrcCopyToUa);
    D3D12State.CommandList->ResourceBarrier(1, &DestCopyToPresent);

    D3D12State.CommandList->Close();
    
    ID3D12CommandList *CommandLists[] = {D3D12State.CommandList};
    D3D12State.CommandQueue->ExecuteCommandLists(1, CommandLists);
    
    hr = D3D12State.SwapChain3->Present(1, 0);
    if(FAILED(hr)) {
        DisplayErrorMessageBox("Direct3D 12 Error", "Swap chain present failed.", hr);
        return false;
    }

    D3D12State.FenceValue++;
    D3D12State.CommandQueue->Signal(D3D12State.Fence, D3D12State.FenceValue);
    D3D12State.Fence->SetEventOnCompletion(D3D12State.FenceValue, D3D12State.FenceEvent);
    WaitForSingleObject(D3D12State.FenceEvent, INFINITE);

    return true;
}

