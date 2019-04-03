
#include <windows.h>
#include <windowsx.h>
#include <D3Dcompiler.h>
#include <d3d11.h>
#include <d3d12.h>
#include <dxgi1_4.h>
#include <comdef.h>

#include "common.h"
#include "memory.cpp"
#include "string.cpp"
#include "math.cpp"
#include "win32_error.cpp"
#include "win32_input.cpp"
#include "win32_helper.cpp"
#include "d3d12_helper.cpp"
#include "camera.cpp"

#include "../code/vendor/imgui/imgui_widgets.cpp"
#include "../code/vendor/imgui/imgui_draw.cpp"
#include "../code/vendor/imgui/imgui.cpp"
#include "../code/vendor/imgui/imgui_impl_win32.cpp"
#include "../code/vendor/imgui/imgui_impl_dx12.cpp"
#include "renderer.cpp"
#include "ui.cpp"


static bool GlobalIsRunning = true;
input_state GlobalInputState = {};
ui_state GlobalUiState = {};
u32 GlobalMenuOpen = 0;
WPARAM MenuToggleButton = 'M';

#define D3D_DEBUG 1

LRESULT CALLBACK
WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if(uMsg == WM_CLOSE) {
        GlobalIsRunning = false;
    }
    if(uMsg == WM_KEYDOWN && wParam == VK_ESCAPE) {
        if(GlobalMenuOpen) {
            ResetCursor(&GlobalInputState);         
            GlobalMenuOpen = 0;
        }
        else {
            GlobalIsRunning = 0;
        }
    }
    if(uMsg == WM_KEYDOWN && wParam == MenuToggleButton) {
        ResetCursor(&GlobalInputState);         
        GlobalMenuOpen = !GlobalMenuOpen;
    }
    if(ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam)) {
        return true;
    }
    UpdateInputState(&GlobalInputState, hWnd, uMsg, wParam, lParam, !GlobalMenuOpen);
    return DefWindowProc(hWnd, uMsg, wParam, lParam);    
}

int WINAPI
wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
    WNDCLASSEXA WindowClass   = {};
    WindowClass.cbSize        = sizeof(WNDCLASSEXA);
    WindowClass.style         = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
    WindowClass.lpfnWndProc   = WindowProc;
    WindowClass.hInstance     = hInstance;
    WindowClass.hCursor       = LoadCursorA(hInstance, IDC_CROSS);
    WindowClass.lpszClassName = "Infinitum";
    
    RegisterClassExA(&WindowClass);
    
    i32 WindowWidth  = 1366;
    i32 WindowHeight = 768;
    int2 WindowPos = Win32CalcWindowPosition(WindowWidth, WindowHeight);
        
    HWND WindowHandle = CreateWindowExA(0, WindowClass.lpszClassName,
                                        "Infinitum", WS_OVERLAPPEDWINDOW | WS_BORDER | WS_VISIBLE,
                                        WindowPos.x, WindowPos.y,
                                        WindowWidth, WindowHeight,
                                        0, 0,
                                        hInstance, 0);
    if(!WindowHandle) return 0;

    POINT Center = { WindowWidth / 2, WindowHeight / 2};    
    ClientToScreen(WindowHandle, &Center);
    GlobalInputState.ScreenCenter = int2(Center.x, Center.y);
    GlobalInputState.WindowDim = int2(WindowWidth, WindowHeight);

    SetCursorPos(GlobalInputState.ScreenCenter.x, GlobalInputState.ScreenCenter.y);    
    RECT CursorRectLimit = {WindowPos.x, WindowPos.y, WindowPos.x + WindowWidth, WindowPos.y + WindowHeight};
    ClipCursor(&CursorRectLimit);
    u32 CursorVisibleFlag = 0;
    ShowCursor(0);
    
    d3d12_framework D3D12Framework = {};
    if(!InitD3D12(WindowHandle, &D3D12Framework)) return 1;
    
    camera Camera = {};
    InitCamera(&Camera);

    InitUI(WindowHandle, &D3D12Framework);

    upload_constants ComputeShaderConstants = {};       
    DefaultUserShaderInput(&ComputeShaderConstants);
    
    u32 iTime = 0;    
    GlobalIsRunning = true;
    
    while(GlobalIsRunning) {
        MSG Message;
        while(PeekMessage(&Message, WindowHandle, 0, 0, PM_REMOVE)) {
            TranslateMessage(&Message);
            DispatchMessage(&Message);
        }

        if(GlobalMenuOpen) {
            ImGui_ImplDX12_NewFrame();
            ImGui_ImplWin32_NewFrame();
            ImGui::NewFrame();      
            BuildUI(&GlobalUiState, &ComputeShaderConstants);
        }

        FillUploadConstants(&ComputeShaderConstants, &Camera, iTime, int2(WindowWidth, WindowHeight));
        
        Render(&D3D12Framework, WindowWidth, WindowHeight, &ComputeShaderConstants, GlobalMenuOpen);

        UpdateCamera(&Camera, &GlobalInputState);        

        if((CursorVisibleFlag == 0) && GlobalMenuOpen) {
            ShowCursor(1);
            CursorVisibleFlag = 1;
        } else if((CursorVisibleFlag == 1) && !GlobalMenuOpen) {
            ShowCursor(0);
            CursorVisibleFlag = 0;
        }
        if((GetActiveWindow() == WindowHandle) && !GlobalMenuOpen)
            SetCursorPos(GlobalInputState.ScreenCenter.x, GlobalInputState.ScreenCenter.y);

        iTime++;        
    }
    return 0;
}
