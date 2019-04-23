
struct ui_state {
    u32 ColorPickerOpen;
    u32 AoDegreeClosed;
    float PrevAoDegree;
};

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
BuildUI(ui_state *UiState, upload_constants *UploadConstants) {
    ImGui::Begin("Menu", 0, ImGuiWindowFlags_AlwaysAutoResize);

    ImGui::Text("Ambient Occlusion:");
    ImGui::SameLine();
    if(!UiState->AoDegreeClosed && ImGui::Button("Toggle Off")) {
        UiState->AoDegreeClosed = 1;
        UiState->PrevAoDegree = UploadConstants->AoDegree;
        UploadConstants->AoDegree = 0.0f;
    } else if(UiState->AoDegreeClosed && ImGui::Button("Toggle On")) {
        UiState->AoDegreeClosed = 0;
        UploadConstants->AoDegree = UiState->PrevAoDegree;
    }    
    if(!UiState->AoDegreeClosed) {
        ImGui::SliderFloat("Degree", &UploadConstants->AoDegree, 0.0f, 5.0f, "%.1f");
    }

    ImGui::Text("Anti Aliasing:");
    ImGui::SameLine();
    if(UploadConstants->AA && ImGui::Button("Toggle Off")) {
        UploadConstants->AA = 0;
    } else if(!UploadConstants->AA && ImGui::Button("Toggle On")) {
        UploadConstants->AA = 1;        
    }

    ImGui::Text("BRDF terms:");
    if(UiState->ColorPickerOpen) {
        float col[3] = {UploadConstants->Color.x, UploadConstants->Color.y, UploadConstants->Color.z};
        ImGui::ColorPicker3("Color", col);
        UploadConstants->Color = float3(col[0], col[1], col[2]);
        if(ImGui::Button("Done")) {
            UiState->ColorPickerOpen = 0;           
        }
    } else if(ImGui::Button("Color")) {
        UiState->ColorPickerOpen = 1;
    }
    
    ImGui::SliderFloat("Specular", &UploadConstants->Specular, 0.0f, 1.0f, "%.2f");
    ImGui::SliderFloat("Roughness", &UploadConstants->Roughness, 0.5f, 1.0f, "%.2f");
    
    ImGui::SliderFloat("Detail", &UploadConstants->MinSurfaceDist, 0.000001f, 0.0035f, "%.6f");
    
    ImGui::End();
}

void
ShutdownUI() {
    ImGui_ImplDX12_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();    
}
