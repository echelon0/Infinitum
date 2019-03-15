
struct input_state {
    u8 W_KEY;
    u8 A_KEY;
    u8 S_KEY;
    u8 D_KEY;
    u8 SPACE_KEY;
    u8 CTRL_KEY;
    
    u8 LEFT_MOUSE_DOWN;
    u8 RIGHT_MOUSE_DOWN;
    int2 PREV_CURSOR_POS;
    int2 CURRENT_CURSOR_POS;
    u8 CURSOR_RESET_TO_CENTER;
    int2 ClientCenter;
};

void
SetCursorToClientCenter(input_state *InputState) {
    SetCursorPos(InputState->ClientCenter.x, InputState->ClientCenter.y);
    InputState->CURSOR_RESET_TO_CENTER = 1;
    InputState->PREV_CURSOR_POS = InputState->CURRENT_CURSOR_POS;
    InputState->CURRENT_CURSOR_POS = int2(InputState->ClientCenter.x, InputState->ClientCenter.y);
}

void
UpdateInputState(input_state *InputState, UINT uMsg, WPARAM wParam, LPARAM lParam) {
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
            if(wParam == VK_SPACE) {
                InputState->SPACE_KEY = 0;              
            }
            if(wParam == VK_CONTROL) {
                InputState->CTRL_KEY = 0;              
            }       
        } break;

        case WM_LBUTTONDOWN: {
            InputState->LEFT_MOUSE_DOWN = 1;
        } break;

        case WM_LBUTTONUP: {
            InputState->LEFT_MOUSE_DOWN = 0;
        } break;
            
        case WM_RBUTTONDOWN: {
            InputState->RIGHT_MOUSE_DOWN = 1;
        } break;

        case WM_RBUTTONUP: {
            InputState->RIGHT_MOUSE_DOWN = 0;
        } break;

        case WM_MOUSEMOVE: {
            InputState->PREV_CURSOR_POS = InputState->CURRENT_CURSOR_POS;
            InputState->CURRENT_CURSOR_POS.x = GET_X_LPARAM(lParam);
            InputState->CURRENT_CURSOR_POS.y = GET_Y_LPARAM(lParam);
        } break;                
    }
}
