
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

#define MAX_STEPS 256
#define MAX_DIST 5.0
#define MIN_SURFACE_DIST 0.001

#define ORDER 8.0
#define MAX_ITERATIONS 15.0
#define DIVERGENCE 1.5
#define EPSILON_GRADIENT 0.000001
//#define EPSILION dist to cam

#define RGB(x, y, z) float3(x / 255.0, y / 255.0, z / 255.0)

struct collision_data {
    float Dist;
    float3 Normal;
    int Iter;
};

float
DistanceEstimator(float3 Pos) {
    float3 Z = Pos;
    float Dr = 1.0;
    float R = 0.0;

    for(int I = 0; I < MAX_ITERATIONS; I++) {
        R = length(Z);
        if(R > DIVERGENCE) {
            break;
        }

        float Theta = acos(Z.z / R);
        float Phi = atan(Z.y / Z.x);
        Dr = pow(R, ORDER - 1.0) * ORDER * Dr + 1.0;

        float Zr = pow(R, ORDER);
        Theta = Theta * ORDER;
        Phi = Phi * ORDER;
        
        Z = Zr * float3(sin(Theta) * cos(Phi), sin(Phi) * sin(Theta), cos(Theta));
        Z = Z + Pos;
    }
    return 0.5 * log(R) * R / Dr;
}

float
RayMarch(float3 Ro, float3 Rd) {
    float TotalDist = 0.0;
    for(int I = 0; I < MAX_STEPS; I++) {
        float3 Pos = Ro + TotalDist * Rd;
        float Dist = DistanceEstimator(Pos);
        TotalDist += Dist;
        if(Dist < MIN_SURFACE_DIST) {
            break;
        }       
        if(TotalDist > MAX_DIST) {
            return -1.0;
        }
    }
    return TotalDist;
}

float3
RenderMandelbulb(float3 Ro, float3 Rd) {
    float3 ColorOut = 0;
    float Dist = RayMarch(Ro, Rd);
    if(Dist < 0.0) {
        return float3(1.0, 1.0, 1.0);
    }
    //float AO = 1.0 - (Collision.Iter / MAX_ITERATIONS);
    float3 Diffuse = RGB(173, 59, 255);
    ColorOut.xyz = Diffuse * Dist;
    return ColorOut;
}

[numthreads(16, 16, 1)]
void CSMain(uint3 thread_id : SV_DispatchThreadID) {
    float2 uv = (thread_id - .5 * iResolution.xy) / iResolution.y;
    uv.y *= -1.0;    
        
    float3 ColorOut = float3(0.0, 0.0, 0.0);

    float3 Ro = CameraPos.xyz;
    float3 Rd = normalize(CameraDir * CameraFilmDist + CameraRight * uv.x + CameraUp * uv.y);

    ColorOut = RenderMandelbulb(Ro, Rd);

    ColorOut.xyz = ColorOut.xyz / (1.0 + ColorOut.xyz);
    ColorOut.xyz = pow(ColorOut.xyz, 1.0 / 2.2);

    Output[thread_id.xy] = float4(ColorOut.xyz, 1.0);
}
