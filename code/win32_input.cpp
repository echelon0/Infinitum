
struct input_state {
    bool W_KEY;
    bool A_KEY;
    bool S_KEY;
    bool D_KEY;
    bool SPACE_KEY;
    bool CTRL_KEY;

    bool LEFT_MOUSE_DOWN;
    int2 PREV_POS;
    int2 CURRENT_POS;
    bool SET_DRAG_FLOATTOR;
    int2 PER_FRAME_DRAG_FLOATTOR;
    float2 PER_FRAME_DRAG_FLOATTOR_PERCENT;
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

        case WM_MOUSEMOVE: {
            InputState->PREV_POS = InputState->CURRENT_POS;
            InputState->CURRENT_POS.x = GET_X_LPARAM(lParam);
            InputState->CURRENT_POS.y = GET_Y_LPARAM(lParam);
        } break;                
    }
}
