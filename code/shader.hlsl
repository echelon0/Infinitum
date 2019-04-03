
RWTexture2D<float4> Output : register(u0);

cbuffer Constants : register(b0) {
    float3 Color;
    float Subsurface;
    float Metalic;
    float Specular;
    float SpecularTint;
    float Roughness;
    float Anistropic;
    float Sheen;
    float SheenTint;
    float Clearcoat;
    float ClearcoatGloss;
    float AoDegree;
    float2 pack0;
    
    float3 CameraPos;
    uint pack1;
    float3 CameraDir;
    uint pack2;
    float3 CameraRight;
    uint pack3;
    float3 CameraUp;
    uint pack4;
    float CameraLensDist;
    uint iTime;
    int2 iResolution;
};

#define PI 3.14159
#define MAX_STEPS 256
#define MAX_DIST 5.0
//#define MIN_SURFACE_DIST min(0.0005 * length(CameraPos), 0.002)
#define MIN_SURFACE_DIST min(0.0005, DistanceEstimator(CameraPos).Dist)

#define MAX_ITERATIONS 15.0
#define DIVERGENCE 1.5
#define EPSILON_GRADIENT 0.000001

#define RGB(x, y, z) float3(x / 255.0, y / 255.0, z / 255.0)

struct collision_info {
    float Dist;
    float3 Normal;
    int Iter;
};

struct dist_iter {
    float Dist;
    int Iter;
};

dist_iter
DistanceEstimator(float3 Pos) {
    float3 Z = Pos;
    float Dr = 1.0;
    float R = 0.0;
    float I = 0.0;

    float Speed = 0.01;
    float MaxOrder = 12;
    float MinOrder = 4;
    float Order = ((sin(iTime * Speed)) % MaxOrder) + MinOrder;
    Order = 8.0;
    
    for(I; I < MAX_ITERATIONS; I++) {
        R = length(Z);
        if(R > DIVERGENCE) {
            break;
        }

        float Theta = acos(Z.z / R);
        float Phi = atan(Z.y / Z.x);
        Dr = pow(R, Order - 1.0) * Order * Dr + 1.0;

        float Zr = pow(R, Order);
        Theta = Theta * Order;
        Phi = Phi * Order;
        
        Z = Zr * float3(sin(Theta) * cos(Phi), sin(Phi) * sin(Theta), cos(Theta));
        Z = Z + Pos;
    }
    dist_iter Result;
    Result.Dist = 0.5 * log(R) * R / Dr;
    Result.Iter = I;
    return Result;
}

float3
ComputeNormal(float3 Pos, float PosDist) {
    float Epsilon = 0.0001;
    float PartialX = (DistanceEstimator(Pos + float3(Epsilon, 0.0, 0.0)).Dist - PosDist) / Epsilon;
    float PartialY = (DistanceEstimator(Pos + float3(0.0, Epsilon, 0.0)).Dist - PosDist) / Epsilon;
    float PartialZ = (DistanceEstimator(Pos + float3(0.0, 0.0, Epsilon)).Dist - PosDist) / Epsilon;
    float3 Normal = float3(PartialX, PartialY, PartialZ);
    return Normal;
}

collision_info
RayMarch(float3 Ro, float3 Rd) {
    float TotalDist = 0.0;
    collision_info Result;
    float3 Normal = 0.0;
    for(int I = 0; I < MAX_STEPS; I++) {
        float3 Pos = Ro + TotalDist * Rd;
        dist_iter DistIter = DistanceEstimator(Pos);
        Normal = ComputeNormal(Pos, DistIter.Dist);
        TotalDist += DistIter.Dist;
        if(DistIter.Dist < MIN_SURFACE_DIST) {
            Result.Dist = TotalDist;
            Result.Normal = Normal;
            Result.Iter = DistIter.Iter;
            break;
        }       
        if(TotalDist > MAX_DIST) {
            Result.Dist = -1.0;
            break;
        }
    }
    return Result;
}

float3
DisneyBRDF(float3 N, float3 L, float3 V) {
    float3 OutColor = float3(0.0, 0.0, 0.0);
    float3 Diffuse = Color / PI;

    float3 H = (L + V) / (length(L + V));
    float IndexFractal = 1.33;
    float IndexAir = 1.00029;
    float F0 = pow((IndexFractal - IndexAir) / (IndexFractal + IndexAir), 2.0);
    float SpecularF = F0 + (1.0 - F0) * pow((1.0 - dot(L, H)), 5.0);
    float SpecularG = pow(0.5 + Roughness / 2.0, 2.0);
    OutColor = Diffuse + SpecularF * SpecularG * (1.0 / (4.0 * dot(N, L) * dot(N, V)));
    return OutColor;
}

[numthreads(16, 16, 1)]
void CSMain(uint3 thread_id : SV_DispatchThreadID) {
    float2 uv = (thread_id - .5 * iResolution.xy) / iResolution.y;
    uv.y *= -1.0;            

    float3 CamPos = CameraPos;
    float3 Ro = CamPos.xyz;
    float3 Rd = normalize(CameraDir * CameraLensDist + CameraRight * uv.x + CameraUp * uv.y);

    float3 ColorOut = float3(0.0, 0.0, 0.0);
    collision_info Collision = RayMarch(Ro, Rd);
    if(Collision.Dist < 0.0) {
        float3 BottomColor = float3(0.0, 0.0, 0.9);
        float3 TopColor = float3(0.8, 0.8, 1.0);
        ColorOut = lerp(BottomColor, TopColor, (Rd.y + 1.0) / 2.0);
    } else {
        float3 LightDir = normalize(float3(-1.0, -0.5, -0.3));
        LightDir = normalize(CameraDir);
        float AO = 1.0;
        if(AoDegree > 0) {
            AO = pow(1.0 - (Collision.Iter / MAX_ITERATIONS), AoDegree);
        }
        ColorOut = DisneyBRDF(Collision.Normal, -LightDir, -CameraDir) * AO;
    }

    ColorOut.xyz = ColorOut.xyz / (1.0 + ColorOut.xyz);
    ColorOut.xyz = pow(ColorOut.xyz, 1.0 / 2.2);

    Output[thread_id.xy] = float4(ColorOut.xyz, 1.0);
}
