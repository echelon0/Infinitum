
struct camera {
    float3 Pos;
    float3 Dir;
    float3 Right;
    float3 Up;
    f32 FilmDist;

    float3 TargetDir;
};

void
InitCamera(camera *Camera) {
    Camera->Pos = float3(0.0f, 0.0f, 0.0f);
    Camera->Dir = float3(0.0f, 0.0f, 1.0);
    Camera->Right = float3(1.0f, 0.0f, 0.0f);
    Camera->Up = float3(0.0f, 1.0f, 0.0f);
    Camera->FilmDist = 1.0f;    
}

void
UpdateCamera(camera *Camera, input_state *InputState) {
    float3 MovementDir = float3(0.0, 0.0, 0.0);
    f32 MovementSpeed = 0.005f;
    MovementDir += Camera->Dir * InputState->W_KEY;
    MovementDir -= Camera->Dir * InputState->S_KEY;    
    MovementDir -= Camera->Right * InputState->A_KEY;
    MovementDir += Camera->Right * InputState->D_KEY;
    MovementDir += Camera->Up * InputState->SPACE_KEY;
    MovementDir -= Camera->Up * InputState->CTRL_KEY;
    MovementDir = normalize(MovementDir);
    Camera->Pos += MovementDir * MovementSpeed;

    int2 PixelDragVector = InputState->CURRENT_CURSOR_SCREEN_POS - InputState->PREV_CURSOR_SCREEN_POS;
    if(InputState->CURSOR_RESET_TO_CENTER && (PixelDragVector.x != 0 || PixelDragVector.y != 0)) {
        int MaxPixelDragRadius = InputState->ScreenCenter.y * 2;
        float2 DragPercentage = float2((f32)PixelDragVector.x, (f32)PixelDragVector.y) / (f32)MaxPixelDragRadius;
//        Camera->TargetDir = Camera->Dir + Camera->Right * DragPercentage.x + Camera->Up * DragPercentage.y;
        InputState->CURSOR_RESET_TO_CENTER = 0;
        
        rotate(&Camera->Dir, DragPercentage.x, float3(0.0f, 0.0f, 0.0f), float3(0.0f, 1.0f, 0.0));
        rotate(&Camera->Right, DragPercentage.x, float3(0.0f, 0.0f, 0.0f), float3(0.0f, 1.0f, 0.0));
        rotate(&Camera->Up, DragPercentage.x, float3(0.0f, 0.0f, 0.0f), float3(0.0f, 1.0f, 0.0));
        
        rotate(&Camera->Dir, DragPercentage.y, float3(0.0f, 0.0f, 0.0f), Camera->Right);      
        rotate(&Camera->Up, DragPercentage.y, float3(0.0f, 0.0f, 0.0f), Camera->Right);
    }
}

