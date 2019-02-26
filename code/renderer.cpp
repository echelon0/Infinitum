
#include <D3Dcompiler.h>
#include <d3d12.h>

bool
InitD3D12() {

    D3D12_ROOT_SIGNATURE_DESC ComputeRootSignatureDesc = {};
    D3D12_ROOT_CONSTANTS ComputeRootConstants = {};
    
    
    LPCWSTR ShaderPath = L"../code/shader.hlsl";
    UINT ShaderCompilerFlags = D3DCOMPILE_OPTIMIZATION_LEVEL3;

    ID3DBlob *CSCodeBlob;
    ID3DBlob *CSErrorBlob;
    if(!SUCCEEDED(D3DCompileFromFile(ShaderPath, 0, 0, "CSMain", "cs_5_0",
                                     ShaderCompilerFlags, 0, &CSCodeBlob, &CSErrorBlob))) {
        LOG_ERROR("Vertex Shader Compliation Failed", (char *)CSErrorBlob->GetBufferPointer());
        return false;
    }

    D3D12_COMPUTE_PIPELINE_STATE_DESC ComputePipelineStateDesc = {};
//    ComputePipelineStateDesc.pRootSignature =;
//    ComputePipelineStateDesc.CS             =;
//    ComputePipelineStateDesc.NodeMask       =;
//    ComputePipelineStateDesc.CachedPSO      =;
//    ComputePipelineStateDesc.Flags          =;

    return true;
}
