
RWTexture2D<float4> Output : register(u0);

#define MAX_STEPS 1000
#define MAX_DIST 100000.
#define MIN_SURFACE_DIST .01

float
DistEstimator(float3 P) {
    float3 s = float3(0, 1, 6);
    float sd = length(P - s) - 1.;
    float pd = P.y;
    return min(sd, pd);
    return sd;
}

float
RayMarch(float3 Ro, float3 Rd) {
    float Dist = 0.;
    for(int I = 0; I < MAX_STEPS; I++) {
        float3 P = Ro + Dist * Rd;
        float DistToScene = DistEstimator(P);
        Dist += DistToScene;
        if(Dist > MAX_DIST || DistToScene < MIN_SURFACE_DIST) break;
    }
    return Dist;
}

[numthreads(16, 16, 1)]
void CSMain(uint3 thread_id : SV_DispatchThreadID) {
    float2 iResolution = float2(960, 580);
    float aspect = iResolution.x / iResolution.y;
    float2 uv = (thread_id - .5 * iResolution.xy) / iResolution.xy;
    uv.y *= -1.;
        
    float3 ColorOut = 0;

    float3 CameraPos = float3(0, 1, 0);
    float3 CameraDir = float3(0, 0, 1);
    float FilmDist = 1.;

    float3 Ro = CameraPos;
    float3 Rd = normalize(float3(uv.x * aspect, uv.y, 1));

    ColorOut.xyz = RayMarch(Ro, Rd);
    ColorOut.xyz /= 10.;

    Output[thread_id.xy] = float4(ColorOut.xyz, 1.);
}
