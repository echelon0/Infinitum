
#include <D3Dcompiler.h>
#include <d3d12.h>
#include <dxgi1_3.h>
#include <unknwn.h>

#define DEBUG 1

struct d3d12_state {
    ID3D12Device *Device;
    ID3D12CommandQueue *CommandQueue;
    IDXGISwapChain1 *SwapChain1;
} D3D12State;

bool
InitD3D12(HWND WindowHandle) {
    HRESULT hr;

#if DEBUG
    ID3D12Debug *D3D12Debug;
    D3D12GetDebugInterface(__uuidof(ID3D12Debug), (void **)(&D3D12Debug));
    D3D12Debug->EnableDebugLayer();
#endif
    
    hr = D3D12CreateDevice(0, D3D_FEATURE_LEVEL_12_0, __uuidof(ID3D12Device), (void **)(&D3D12State.Device));
    if(FAILED(hr)) {
        LOG_ERROR("Direct3D 12 Error", "Failed to create device");
        return false;
    }

    D3D12_COMMAND_QUEUE_DESC CommandQueueDesc = {};
    CommandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    CommandQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
    CommandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    CommandQueueDesc.NodeMask = 0;
    
    hr = D3D12State.Device->CreateCommandQueue(&CommandQueueDesc, __uuidof(ID3D12CommandQueue), (void **)(&D3D12State.CommandQueue));
    if(FAILED(hr)) {
        LOG_ERROR("Direct3D 12 Error", "Failed to create a command queue");
        return false;
    }   

    DXGI_SWAP_CHAIN_DESC1 SwapChainDesc = {};
    SwapChainDesc.Width = 0;
    SwapChainDesc.Height = 0;
    SwapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    SwapChainDesc.Stereo = FALSE;
    SwapChainDesc.SampleDesc.Count = 1;
    SwapChainDesc.SampleDesc.Quality = 0;
    SwapChainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER;
    SwapChainDesc.BufferCount = 2;
    SwapChainDesc.Scaling = DXGI_SCALING_STRETCH;
    SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    SwapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
    SwapChainDesc.Flags = 0;

    IDXGIFactory2 *DXGIFactory;
    hr = CreateDXGIFactory2(0, __uuidof(IDXGIFactory2), (void **)(&DXGIFactory));
    if(FAILED(hr)) {
        LOG_ERROR("Direct3D 12 Error", "Failed to create DXGIFactory object");
        return false;       
    }
    
    hr = DXGIFactory->CreateSwapChainForHwnd(D3D12State.CommandQueue, WindowHandle, &SwapChainDesc, 0, 0, &D3D12State.SwapChain1);
    if(FAILED(hr)) {
        LOG_ERROR("Direct3D 12 Error", "Failed to create swap chain");
        return false;
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
