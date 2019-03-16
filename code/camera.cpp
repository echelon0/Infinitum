
struct ortho {
    float3 Dir;
    float3 Right;
    float3 Up;
};

struct camera {
    float3 Pos;
    ortho Frame;
    f32 FilmDist;
    ortho TargetFrame;
};

void
InitCamera(camera *Camera) {
    Camera->Pos = float3(0.0f, 0.0f, 0.0f);
    Camera->Frame.Dir = float3(0.0f, 0.0f, 1.0);
    Camera->Frame.Right = float3(1.0f, 0.0f, 0.0f);
    Camera->Frame.Up = float3(0.0f, 1.0f, 0.0f);
    Camera->FilmDist = 1.0f;
    Camera->TargetFrame = Camera->Frame;
}

void
UpdateCamera(camera *Camera, input_state *InputState) {
    float3 MovementDir = float3(0.0, 0.0, 0.0);
    f32 MovementSpeed = 0.005f;
    MovementDir += Camera->Frame.Dir * InputState->W_KEY;
    MovementDir -= Camera->Frame.Dir * InputState->S_KEY;    
    MovementDir -= Camera->Frame.Right * InputState->A_KEY;
    MovementDir += Camera->Frame.Right * InputState->D_KEY;
    MovementDir += Camera->Frame.Up * InputState->SPACE_KEY;
    MovementDir -= Camera->Frame.Up * InputState->CTRL_KEY;
    MovementDir = normalize(MovementDir);
    Camera->Pos += MovementDir * MovementSpeed;

    float2 PixelDragVector = float2(InputState->CURRENT_CURSOR_SCREEN_POS) - float2(InputState->ScreenCenter);
    if((PixelDragVector.x != 0.0f || PixelDragVector.y != 0.0f)) {
        float2 DragPercentage = PixelDragVector / float2(InputState->WindowDim);

        rotate(&Camera->TargetFrame.Dir, DragPercentage.x, float3(0.0f, 0.0f, 0.0f), float3(0.0f, 1.0f, 0.0));
        rotate(&Camera->TargetFrame.Right, DragPercentage.x, float3(0.0f, 0.0f, 0.0f), float3(0.0f, 1.0f, 0.0));
        rotate(&Camera->TargetFrame.Up, DragPercentage.x, float3(0.0f, 0.0f, 0.0f), float3(0.0f, 1.0f, 0.0));
        rotate(&Camera->TargetFrame.Dir, DragPercentage.y, float3(0.0f, 0.0f, 0.0f), Camera->TargetFrame.Right);      
        rotate(&Camera->TargetFrame.Up, DragPercentage.y, float3(0.0f, 0.0f, 0.0f), Camera->TargetFrame.Right);
    }
    f32 RotationT = 0.2f;
    Camera->Frame.Dir = lerp(Camera->Frame.Dir, Camera->TargetFrame.Dir, RotationT);
    Camera->Frame.Right = lerp(Camera->Frame.Right, Camera->TargetFrame.Right, RotationT);
    Camera->Frame.Up = lerp(Camera->Frame.Up, Camera->TargetFrame.Up, RotationT);
}

