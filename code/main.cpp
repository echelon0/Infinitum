
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
#include "renderer.cpp"

static bool GlobalIsRunning = true;
input_state GlobalInputState = {};

LRESULT CALLBACK
WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if(uMsg == WM_CLOSE || (uMsg == WM_KEYDOWN && wParam == VK_ESCAPE)) {
        GlobalIsRunning = false;
    }
    UpdateInputState(&GlobalInputState, hWnd, uMsg, wParam, lParam);
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
    
    i32 WindowWidth  = 960;
    i32 WindowHeight = 580;
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
    
    d3d12_framework D3D12Framework = {};
    if(!InitD3D12(WindowHandle, &D3D12Framework)) return 0;
    
    u32 iTime = 0;
    camera Camera = {};
    InitCamera(&Camera);
    upload_constants ComputeShaderConstants = {};
    GlobalIsRunning = true;    
    while(GlobalIsRunning) {
        if(GetActiveWindow() == WindowHandle)
            SetCursorToCenter(&GlobalInputState); 
        MSG Message;
        while(PeekMessage(&Message, WindowHandle, 0, 0, PM_REMOVE)) {
            TranslateMessage(&Message);
            DispatchMessage(&Message);
        }
        
        ComputeShaderConstants.iTime = iTime;
        ComputeShaderConstants.CameraPos = Camera.Pos;
        ComputeShaderConstants.CameraDir = Camera.Dir;
        ComputeShaderConstants.CameraRight = Camera.Right;
        ComputeShaderConstants.CameraUp = Camera.Up;
        ComputeShaderConstants.CameraFilmDist = Camera.FilmDist;
        Render(&D3D12Framework, WindowWidth, WindowHeight, &ComputeShaderConstants);
        
        iTime += 1;
        UpdateCamera(&Camera, &GlobalInputState); 
    }
    return 0;
}
