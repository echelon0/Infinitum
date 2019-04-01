
void
InitUI(HWND WindowHandle, d3d12_framework *D3D12Framework) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    
    ImGui_ImplWin32_Init(WindowHandle);
    ImGui_ImplDX12_Init(D3D12Framework->Device, 1,
                        DXGI_FORMAT_R8G8B8A8_UNORM,
                        D3D12Framework->ImGuiSrvCPUDescHandle,
                        D3D12Framework->ImGuiSrvGPUDescHandle);
}

void
BuildUI(brdf_parameters *BRDF) {
    ImGui::Begin("Menu");
    float col[3] = {BRDF->Color.x, BRDF->Color.y, BRDF->Color.z};
    ImGui::ColorPicker3("Color", col);
    BRDF->Color = float3(col[0], col[1], col[2]);
    ImGui::End();
}

void
ShutdownUI() {
    ImGui_ImplDX12_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();    
}
