
#include <D3Dcompiler.h>
#include <d3d12.h>
#include <dxgi1_3.h>
#include <unknwn.h>
#include <comdef.h>

#define D3D_DEBUG 1

struct d3d12_state {
    ID3D12Device *Device;
    ID3D12CommandQueue *CommandQueue;
    IDXGISwapChain1 *SwapChain1;
    ID3D12DescriptorHeap *CBV_SRV_UAV_DescriptorHeap;
    ID3D12Resource *UAV_Compute;
    ID3D12RootSignature *ComputeRootSignature;
    ID3D12PipelineState *ComputePSO;    
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

bool
InitD3D12(HWND WindowHandle) {
    u32 BACK_BUFFER_COUNT = 2;
    HRESULT hr;

#if D3D_DEBUG
    ID3D12Debug *Debug;
    D3D12GetDebugInterface(IID_PPV_ARGS(&Debug));
    Debug->EnableDebugLayer();
#endif
    
    hr = D3D12CreateDevice(0, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&D3D12State.Device));
    if(FAILED(hr)) {
        char *ErrorMsg = CreateErrorMessage("Failed to create device.", hr);
        MessageBoxA(0, ErrorMsg, "Direct3D 12 Error", MB_OK|MB_ICONERROR);
        free(ErrorMsg);
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
        char *ErrorMsg = CreateErrorMessage("Failed to create command queue.", hr);
        MessageBoxA(0, ErrorMsg, "Direct3D 12 Error", MB_OK|MB_ICONERROR);
        free(ErrorMsg);
        return false;
    }   

    IDXGIFactory2 *DXGIFactory;
    hr = CreateDXGIFactory2(0, IID_PPV_ARGS(&DXGIFactory));
    if(FAILED(hr)) {
        char *ErrorMsg = CreateErrorMessage("Failed to create DXGIFactory object.", hr);                
        MessageBoxA(0, ErrorMsg, "Direct3D 12 Error", MB_OK|MB_ICONERROR);
        free(ErrorMsg);
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
        char *ErrorMsg = CreateErrorMessage("Failed to create swap chain.", hr);        
        MessageBoxA(0, ErrorMsg, "Direct3D 12 Error", MB_OK|MB_ICONERROR);
        free(ErrorMsg);
        return false;
    }

    D3D12_DESCRIPTOR_HEAP_DESC DescriptorHeapDesc = {};
    DescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    DescriptorHeapDesc.NumDescriptors = 1;
    DescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    DescriptorHeapDesc.NodeMask = 0;

    hr = D3D12State.Device->CreateDescriptorHeap(&DescriptorHeapDesc, IID_PPV_ARGS(&D3D12State.CBV_SRV_UAV_DescriptorHeap));
    if(FAILED(hr)) {
        char *ErrorMsg = CreateErrorMessage("Failed to create CBV/SRV/UAV descriptor heap.", hr);
        MessageBoxA(0, ErrorMsg, "Direct3D 12 Error", MB_OK|MB_ICONERROR);
        free(ErrorMsg);
        return false;   
    }

    RECT WindowRect = {};
    GetWindowRect(WindowHandle, &WindowRect);
    u32 WindowWidth = WindowRect.right - WindowRect.left;
    u32 WindowHeight = WindowRect.bottom - WindowRect.top;
    
    D3D12_CPU_DESCRIPTOR_HANDLE CBV_SRV_UAV_DescriptorHandle = D3D12State.CBV_SRV_UAV_DescriptorHeap->GetCPUDescriptorHandleForHeapStart();
    D3D12_UNORDERED_ACCESS_VIEW_DESC UAV_Desc = {};
    UAV_Desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    UAV_Desc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
    UAV_Desc.Buffer.FirstElement = 0;
    UAV_Desc.Buffer.NumElements = WindowWidth * WindowHeight;
    UAV_Desc.Buffer.StructureByteStride = 0;
    UAV_Desc.Buffer.CounterOffsetInBytes = 0;
    UAV_Desc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;
    
    D3D12State.Device->CreateUnorderedAccessView(D3D12State.UAV_Compute, 0, &UAV_Desc, CBV_SRV_UAV_DescriptorHandle);

    D3D12_DESCRIPTOR_RANGE UAV_DescriptorRange = {};
    UAV_DescriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
    UAV_DescriptorRange.NumDescriptors = 1;
    UAV_DescriptorRange.BaseShaderRegister = 0;
    UAV_DescriptorRange.RegisterSpace = 0;
    UAV_DescriptorRange.OffsetInDescriptorsFromTableStart = 0;
    
    D3D12_ROOT_PARAMETER ComputeRootParameter = {};
    ComputeRootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    ComputeRootParameter.DescriptorTable.NumDescriptorRanges = 1;
    ComputeRootParameter.DescriptorTable.pDescriptorRanges = &UAV_DescriptorRange;
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
        char *ErrorMsg = CreateErrorMessage("Failed to serialize compute root signature.", hr); 
        MessageBoxA(0, ErrorMsg, "Direct3D 12 Error", MB_OK|MB_ICONERROR);
        free(ErrorMsg);
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
        char *ErrorMsg = CreateErrorMessage("Failed to create compute pipeline state object.", hr); 
        MessageBoxA(0, ErrorMsg, "Direct3D 12 Error", MB_OK|MB_ICONERROR);
        free(ErrorMsg);
        return false;
    }

    return true;
}

