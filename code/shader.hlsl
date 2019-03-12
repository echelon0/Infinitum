
RWTexture2D<float4> Output : register(u0);

[numthreads(16, 16, 1)]
void CSMain(uint3 thread_id : SV_DispatchThreadID) {
    Output[thread_id.xy] = float4(0.5, 0.7, 0.5, 1.0);
}
