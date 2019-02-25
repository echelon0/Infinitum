
#define LOG_ERROR(Title, Message) MessageBoxA(0, Message, Title, MB_OK|MB_ICONERROR)

#include "common.h"
#include "math.h"
#include <windows.h>

bool GlobalIsRunning = true;

LRESULT CALLBACK
WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    LRESULT Result = 0;
    switch(uMsg) {
        case WM_CLOSE:
            GlobalIsRunning = false;
            break;
        case WM_KEYDOWN:
            if(wParam == VK_ESCAPE) {
                GlobalIsRunning = false;
            }
            break;
        default:
            Result = DefWindowProc(hWnd, uMsg, wParam, lParam);
            break;
    }
    return Result;
}

int2
Win32CalcWindowPosition(i32 WindowWidth, i32 WindowHeight) {
    i32 ScreenWidth  = GetSystemMetrics(SM_CXSCREEN);
    i32 ScreenHeight = GetSystemMetrics(SM_CYSCREEN);
    i32 X = (ScreenWidth / 2) - (WindowWidth / 2);
    i32 Y = (ScreenHeight / 2) - (WindowHeight / 2);
    return int2(X, Y);
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

    if(RegisterClassExA(&WindowClass)) {
        i32 WindowWidth  = 960;
        i32 WindowHeight = 580;
        int2 WindowPos = Win32CalcWindowPosition(WindowWidth, WindowHeight);
        
        HWND WindowHandle = CreateWindowExA(0, WindowClass.lpszClassName,
                                            "Infinitum", WS_OVERLAPPEDWINDOW | WS_BORDER | WS_VISIBLE,
                                            WindowPos.x, WindowPos.y,
                                            WindowWidth, WindowHeight,
                                            0, 0,
                                            hInstance, 0);
        if(WindowHandle) {
            GlobalIsRunning = true;
            
            while(GlobalIsRunning) {
                MSG Message;
                while(PeekMessage(&Message, WindowHandle, 0, 0, PM_REMOVE)) {
                    TranslateMessage(&Message);
                    DispatchMessage(&Message);
                }
                
            }
            
        } else {
            LOG_ERROR("Windows API Error", "Call to CreateWindowExA() failed.");
        }
        
    } else {
        LOG_ERROR("Windows API Error", "Call to RegisterClassExA() failed.");
    }
    
    return 0;
}
