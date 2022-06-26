cbuffer bufs : register(b0) {
    float4x4 local_to_view;
}

struct PSInput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

PSInput vs_main(float4 position : POSITION, float4 color : COLOR)
{
    PSInput result;

    result.position = mul(local_to_view, position);
    result.color = color;

    return result;
}

float4 ps_main(PSInput input) : SV_TARGET
{
    return input.color;
}