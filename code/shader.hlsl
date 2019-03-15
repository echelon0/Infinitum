
RWTexture2D<float4> Output : register(u0);

cbuffer Constants : register(b0) {
    uint iTime;
    float3 CameraPos;
    float3 CameraDir;
    float3 CameraRight;
    float3 CameraUp;
    float CameraFilmDist;
};

#define MAX_STEPS 1000
#define MAX_DIST 10.0
#define MIN_SURFACE_DIST 0.01
#define MAX_ITERATIONS 5
#define RGB(x, y, z) float3(x / 255.0, y / 255.0, z / 255.0)

float sdBox(float3 P, float3 B)
{
    float3 Dist = abs(P) - B;
    return length(max(Dist.xyz, 0.0)) + min(max(Dist.x, max(Dist.y, Dist.z)), 0.);
}

float sdCross(float3 P)
{
  float da = sdBox(P.xyz,float3(99999.,1.0,1.0));
  float db = sdBox(P.yzx,float3(1.0,99999.,1.0));
  float dc = sdBox(P.zxy,float3(1.0,1.0,99999.));
  return min(da,min(db,dc));
}

struct collision_data {
    float Dist;
    int Iter;
};

interface DistanceEstimator {
    collision_data Intersect(float3 P);
};

class SpherePlaneDE : DistanceEstimator {
    collision_data Intersect(float3 P) {
        collision_data Collision;
        float3 Dist = 0.0;
        float3 s = float3(0.0, 1.0, 6.0);
        float sd = length(P - s) - 1.0;
        float pd = P.y;
        Collision.Dist = min(sd, pd);
        Collision.Iter = 1;
        return Collision;
    }
};

class MengerSpongeDE : DistanceEstimator {
    collision_data Intersect(float3 P) {
        float3 B = float3(1, 1, 1);
        float Dist = sdBox(P, B);
        float S = 1.0;
        int IterHit = 0;
        for(int Iter = 1; Iter <= MAX_ITERATIONS; Iter++) {
            float3 A = (P * S) % 2.0;
            S *= 3.0;
            float3 R = abs(3.0 * abs(A));
            float DistCross = sdCross(R) / S;
            if(Dist < -DistCross) {
                Dist = -DistCross;
                IterHit = Iter;
            }
        }
        collision_data Collision;
        Collision.Dist = Dist;
        Collision.Iter = IterHit;
        return Collision;
    }
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

float3
MengerSponge(float3 Ro, float3 Rd) {
    float3 BackgroundColor = float3(1.0, 1.0, 1.0);    
    float3 ColorOut = 0;
    MengerSpongeDE DE;
    float3 Mats[] = {
        RGB(255, 215, 96),      
        RGB(184, 197, 219),
        RGB(255, 117, 96),
        RGB(219, 133, 199),
        
        RGB(255, 215, 96),      
        RGB(184, 197, 219),
        RGB(255, 117, 96),
        RGB(219, 133, 199),
    };
    
    collision_data Collision = RayMarch(Ro, Rd, DE);
    if(Collision.Dist < 0.0) {
        ColorOut.xyz = BackgroundColor;
    } else {
        ColorOut.xyz = Mats[Collision.Iter];
        if(Collision.Iter > 5)
            ColorOut = 0;
    }
    return ColorOut;
}

[numthreads(16, 16, 1)]
void CSMain(uint3 thread_id : SV_DispatchThreadID) {
    float2 iResolution = float2(960, 580);
    float aspect = iResolution.x / iResolution.y;
    float2 uv = (thread_id - .5 * iResolution.xy) / iResolution.y;
        
    float3 ColorOut = 0;

    float3 Ro = CameraPos;
    //float3 Rd = normalize(CameraDir * CameraFilmDist + CameraRight * uv.x + CameraUp * uv.y);
    float3 Rd = normalize(float3(uv.x, uv.y, 1.0));

    ColorOut.xyz = MengerSponge(Ro, Rd);
    ColorOut.xyz = pow(ColorOut.xyz, 1.0 / 2.2);
    uv.y *= -1.0;
    Output[thread_id.xy] = float4(ColorOut.xyz, 1.0);
}
