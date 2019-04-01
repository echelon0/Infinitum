
struct input_state {
    u8 W_KEY;
    u8 A_KEY;
    u8 S_KEY;
    u8 D_KEY;
    u8 UP_KEY;
    u8 DOWN_KEY;
    u8 SPACE_KEY;
    u8 CTRL_KEY;
    
    int2 PREV_CURSOR_SCREEN_POS;
    int2 CURRENT_CURSOR_SCREEN_POS;
    int2 ScreenCenter;
    int2 WindowDim;
};

void
ResetCursor(input_state *InputState) {
    InputState->CURRENT_CURSOR_SCREEN_POS = InputState->ScreenCenter;
    InputState->PREV_CURSOR_SCREEN_POS = InputState->CURRENT_CURSOR_SCREEN_POS;    
}

void
UpdateInputState(input_state *InputState, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, u32 CaptureMouse) {
    switch(uMsg) {
        case WM_KEYDOWN: {
            if(wParam == 'W') {
                InputState->W_KEY = 1;
            }
            if(wParam == 'A') {
                InputState->A_KEY = 1;
            }
            if(wParam == 'S') {
                InputState->S_KEY = 1;
            }
            if(wParam == 'D') {
                InputState->D_KEY = 1;
            }
            if(wParam == VK_UP) {
                InputState->UP_KEY = 1;
            }
            if(wParam == VK_DOWN) {
                InputState->DOWN_KEY = 1;
            }
            if(wParam == VK_SPACE) {
                InputState->SPACE_KEY = 1;
            }
            if(wParam == VK_CONTROL) {
                InputState->CTRL_KEY = 1;              
            }                    
        } break;

        case WM_KEYUP: {       
            if(wParam == 'W') {
                InputState->W_KEY = 0;
            }
            if(wParam == 'A') {
                InputState->A_KEY = 0;
            }
            if(wParam == 'S') {
                InputState->S_KEY = 0;
            }
            if(wParam == 'D') {
                InputState->D_KEY = 0;
            }
            if(wParam == VK_UP) {
                InputState->UP_KEY = 0;
            }
            if(wParam == VK_DOWN) {
                InputState->DOWN_KEY = 0;
            }
            if(wParam == VK_SPACE) {
                InputState->SPACE_KEY = 0;
            }
            if(wParam == VK_CONTROL) {
                InputState->CTRL_KEY = 0;
            }       
        } break;

        case WM_MOUSEMOVE: {
            if(CaptureMouse) {
                POINT CurrentPos = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
                ClientToScreen(hWnd, &CurrentPos);
                InputState->PREV_CURSOR_SCREEN_POS = InputState->CURRENT_CURSOR_SCREEN_POS;
                InputState->CURRENT_CURSOR_SCREEN_POS = int2(CurrentPos.x, CurrentPos.y);
            }
        } break;
    }
}
