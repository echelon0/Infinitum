
struct input_state {
    bool W_KEY;
    bool A_KEY;
    bool S_KEY;
    bool D_KEY;
    bool SPACE_KEY;
    bool CTRL_KEY;
    
    bool LEFT_MOUSE_DOWN;
    bool RIGHT_MOUSE_DOWN;
    int2 PREV_CURSOR_POS;
    int2 CURRENT_CURSOR_POS;
};

void
UpdateInputState(input_state *InputState, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch(uMsg) {
        case WM_KEYDOWN: {
            if(wParam == 'W') {
                InputState->W_KEY = true;
            }
            if(wParam == 'A') {
                InputState->A_KEY = true;
            }
            if(wParam == 'S') {
                InputState->S_KEY = true;
            }
            if(wParam == 'D') {
                InputState->D_KEY = true;
            }
            if(wParam == VK_SPACE) {
                InputState->SPACE_KEY = true;              
            }
            if(wParam == VK_CONTROL) {
                InputState->CTRL_KEY = true;              
            }                    
        } break;

        case WM_KEYUP: {       
            if(wParam == 'W') {
                InputState->W_KEY = false;
            }
            if(wParam == 'A') {
                InputState->A_KEY = false;
            }
            if(wParam == 'S') {
                InputState->S_KEY = false;
            }
            if(wParam == 'D') {
                InputState->D_KEY = false;
            }
            if(wParam == VK_SPACE) {
                InputState->SPACE_KEY = false;              
            }
            if(wParam == VK_CONTROL) {
                InputState->CTRL_KEY = false;              
            }       
        } break;

        case WM_LBUTTONDOWN: {
            InputState->LEFT_MOUSE_DOWN = true;
        } break;

        case WM_LBUTTONUP: {
            InputState->LEFT_MOUSE_DOWN = false;
        } break;
            
        case WM_RBUTTONDOWN: {
            InputState->RIGHT_MOUSE_DOWN = true;
        } break;

        case WM_RBUTTONUP: {
            InputState->RIGHT_MOUSE_DOWN = false;
        } break;

        case WM_MOUSEMOVE: {
            InputState->PREV_CURSOR_POS = InputState->CURRENT_CURSOR_POS;
            InputState->CURRENT_CURSOR_POS.x = GET_X_LPARAM(lParam);
            InputState->CURRENT_CURSOR_POS.y = GET_Y_LPARAM(lParam);
        } break;                
    }
}
