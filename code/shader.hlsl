
RWTexture2D<float4> Output : register(u0);

cbuffer Constants : register(b0) {
    float3 CameraPos;
    uint pack1;
    float3 CameraDir;
    uint pack2;
    float3 CameraRight;
    uint pack3;
    float3 CameraUp;
    uint pack4;
    float CameraFilmDist;
    uint iTime;
    int2 iResolution;
};

#define MAX_STEPS 1000
#define MAX_DIST 10.0
#define MIN_SURFACE_DIST 0.0001

#define RGB(x, y, z) float3(x / 255.0, y / 255.0, z / 255.0)

struct collision_data {
    float Dist;
    int Iter;
};

interface DistanceEstimator {
    collision_data Intersect(float3 P);
};

collision_data
RayMarch(float3 Ro, float3 Rd, DistanceEstimator DE) {
    collision_data Collision;
    float TotalDist = 0.0;
    for(int I = 0; I < MAX_STEPS; I++) {
        float3 P = Ro + TotalDist * Rd;
        Collision = DE.Intersect(P);
        TotalDist += Collision.Dist;
        if(TotalDist > MAX_DIST) {
            Collision.Dist = -1.0;
            Collision.Iter = 0;
            return Collision;
        }
        if(Collision.Dist < MIN_SURFACE_DIST) {
            Collision.Dist = TotalDist;
            break;
        }
    }
    return Collision;
}

[numthreads(16, 16, 1)]
void CSMain(uint3 thread_id : SV_DispatchThreadID) {
    float2 uv = (thread_id - .5 * iResolution.xy) / iResolution.y;
    uv.y *= -1.0;    
        
    float3 ColorOut = float3(0.5123, 0.123, 0.872);

    float3 Ro = CameraPos;
    float3 Rd = normalize(CameraDir * CameraFilmDist + CameraRight * uv.x + CameraUp * uv.y);

    float2 c = uv;
    c.x -= 0.3;
    c*=2.2;
    float2 z = 0.0;
    float iter = 0.0;
    float MAX_ITER = 200.0;
    for(int I = 0; I < MAX_ITER; I++) {
        z = float2(z.x*z.x - z.y*z.y, 2.0*z.x*z.y) + c;
        if(length(z) > 2.0)
            break;
        iter++;
    }

    ColorOut.xyz = ColorOut.xyz / (1.0 + ColorOut.xyz);
    ColorOut.xyz += iter / MAX_ITER;
    ColorOut.xyz = pow(ColorOut.xyz, 1.0 / 2.2);

    Output[thread_id.xy] = float4(ColorOut.xyz, 1.0);
}
