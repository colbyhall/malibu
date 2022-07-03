cbuffer bufs : register(b0) {
    float4x4 local_to_projection;
}

struct PSInput
{
    float4 position : SV_POSITION;
    float3 normal : COLOR;
};

PSInput vs_main(float4 position : POSITION, float3 normal: COLOR)
{
    PSInput result;

    result.position = mul(local_to_projection, position);
    result.normal = normal;

    return result;
}

float4 ps_main(PSInput input) : SV_TARGET
{
    return float4(input.normal.x, input.normal.y, input.normal.z, 1.f);
}