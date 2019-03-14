
struct camera {
    float3 Pos;
    float3 Dir;
    float3 Right;
    float3 Up;
    f32 FilmDist;
};

void
UpdateCamera(camera *Camera, input_state InputState) {
    float3 MovementDir = float3(0.0, 0.0, 0.0);
    f32 MovementSpeed = 0.2f;
    if(InputState.W_KEY) {
        MovementDir += Camera->Dir;
    }
    if(InputState.A_KEY) {
        MovementDir -= Camera->Right;
    }
    if(InputState.S_KEY) {
        MovementDir -= Camera->Dir;
    }
    if(InputState.D_KEY) {
        MovementDir += Camera->Right;
    }
    if(InputState.SPACE_KEY) {
        MovementDir += Camera->Up;
    }
    if(InputState.CTRL_KEY) {
        MovementDir -= Camera->Up;
    }
    MovementDir = normalize(MovementDir);
    Camera->Pos += MovementDir * MovementSpeed;
}
