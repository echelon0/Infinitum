
#include <D3Dcompiler.h>
#include <d3d12.h>
#include <dxgi1_3.h>
#include <unknwn.h>

#define D3D_DEBUG 1

struct d3d12_state {
    ID3D12Device *Device;
    ID3D12CommandQueue *CommandQueue;
    IDXGISwapChain1 *SwapChain1;
    ID3D12DescriptorHeap *RTV_DescriptorHeap;
} D3D12State;

bool
InitD3D12(HWND WindowHandle) {
    u32 BACK_BUFFER_COUNT = 2;
    HRESULT hr;

#if D3D_DEBUG
    ID3D12Debug *Debug;
    D3D12GetDebugInterface(IID_PPV_ARGS(&Debug));
    Debug->EnableDebugLayer();
#endif
    
    hr = D3D12CreateDevice(0, D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&D3D12State.Device));
    if(FAILED(hr)) {
        MessageBoxA(0, "Failed to create device.", "Direct3D 12 Error", MB_OK|MB_ICONERROR);
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
        MessageBoxA(0, "Failed to create a command queue.", "Direct3D 12 Error", MB_OK|MB_ICONERROR);
        return false;
    }   

    IDXGIFactory2 *DXGIFactory;
    hr = CreateDXGIFactory2(0, IID_PPV_ARGS(&DXGIFactory));
    if(FAILED(hr)) {
        MessageBoxA(0, "Failed to create DXGIFactory object.", "Direct3D 12 Error", MB_OK|MB_ICONERROR);
        return false;       
    }
    
    DXGI_SWAP_CHAIN_DESC1 SwapChainDesc = {};
    SwapChainDesc.Width = 0;
    SwapChainDesc.Height = 0;
    SwapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    SwapChainDesc.Stereo = 0;
    SwapChainDesc.SampleDesc.Count = 1;
    SwapChainDesc.SampleDesc.Quality = 0;
    SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    SwapChainDesc.BufferCount = BACK_BUFFER_COUNT;
    SwapChainDesc.Scaling = DXGI_SCALING_STRETCH;
    SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    SwapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
    SwapChainDesc.Flags = 0;
    
    hr = DXGIFactory->CreateSwapChainForHwnd(D3D12State.CommandQueue, WindowHandle, &SwapChainDesc, 0, 0, &D3D12State.SwapChain1);
    if(FAILED(hr)) {
        MessageBoxA(0, "Failed to create swap chain.", "Direct3D 12 Error", MB_OK|MB_ICONERROR);        
        return false;
    }

    //NOTE: Render target view descriptor heap
    D3D12_DESCRIPTOR_HEAP_DESC DescriptorHeapDesc = {};
    DescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    DescriptorHeapDesc.NumDescriptors = 1;
    DescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    DescriptorHeapDesc.NodeMask = 0;

    hr = D3D12State.Device->CreateDescriptorHeap(&DescriptorHeapDesc, IID_PPV_ARGS(&D3D12State.RTV_DescriptorHeap));
    if(FAILED(hr)) {
        MessageBoxA(0, "Failed to create descriptor heap for render target view.", "Direct3D 12 Error", MB_OK|MB_ICONERROR);
        return false;   
    }

    D3D12_CPU_DESCRIPTOR_HANDLE RTV_DescriptorHandle = D3D12State.RTV_DescriptorHeap->GetCPUDescriptorHandleForHeapStart();
    u32 RTV_DescriptorSize = D3D12State.Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);    
    for(u32 BufferIndex = 0; BufferIndex < BACK_BUFFER_COUNT; BufferIndex+=1) {
        ID3D12Resource *BackBuffer;
        hr = D3D12State.SwapChain1->GetBuffer(BufferIndex, IID_PPV_ARGS(&BackBuffer));
        if(FAILED(hr)) {
            MessageBoxA(0, "Failed to create render target view for back buffer." , "Direct3D 12 Error", MB_OK|MB_ICONERROR);
        }
    }
    
    /*
    D3D12_ROOT_SIGNATURE_DESC ComputeRootSignatureDesc = {};
    D3D12_ROOT_CONSTANTS ComputeRootConstants = {};
    
    
    LPCWSTR ShaderPath = L"../code/shader.hlsl";
    UINT ShaderCompilerFlags = D3DCOMPILE_OPTIMIZATION_LEVEL3;

    ID3DBlob *CSCodeBlob;
    ID3DBlob *CSErrorBlob;
    hr = D3DCompileFromFile(ShaderPath, 0, 0, "CSMain", "cs_5_0", ShaderCompilerFlags, 0, &CSCodeBlob, &CSErrorBlob);
    if(FAILED(hr)) {
        LOG_ERROR("Vertex Shader Compliation Failed", (char *)CSErrorBlob->GetBufferPointer());
        return false;
    }

    D3D12_COMPUTE_PIPELINE_STATE_DESC ComputePipelineStateDesc = {};
    */
//    ComputePipelineStateDesc.pRootSignature =;
//    ComputePipelineStateDesc.CS             =;
//    ComputePipelineStateDesc.NodeMask       =;
//    ComputePipelineStateDesc.CachedPSO      =;
//    ComputePipelineStateDesc.Flags          =;

    return true;
}
