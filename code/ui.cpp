
void
InitUI(HWND WindowHandle, d3d12_framework *D3D12Framework) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    
    ImGui_ImplWin32_Init(WindowHandle);
    ImGui_ImplDX12_Init(D3D12Framework->Device, 1,
                        DXGI_FORMAT_R8G8B8A8_UNORM,
                        D3D12Framework->CbvSrvUavDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
                        D3D12Framework->CbvSrvUavDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
}

void
RenderUI(ID3D12GraphicsCommandList *CommandList) {
    ImGui_ImplDX12_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
    ImGui::Begin("Hello, world!");
    ImGui::End();
    
    ImGui::Render();
    ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), CommandList);
}

void
ShutdownUI() {
    ImGui_ImplDX12_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();    
}
