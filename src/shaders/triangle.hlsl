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
	float3 color = float3(1.f, 1.f, 0.f);

	float3 a = float3(0.f, 0.f, 0.f);
	float3 b = float3(2.f, 3.f, -5.f);
	float3 light_direction = normalize(b - a);
	float3 light_color = float3(0.5f, 0.5f, 0.5f);

	float3 ambient = float3(0.1f, 0.1f, 0.1f);

	float3 diffuse = light_color * max(dot(input.normal, light_direction), 0.f);

	float3 final_color = color * (diffuse + ambient);

    return float4(final_color.x, final_color.y, final_color.z, 1.f);
	// return float4(color.x, color.y, color.z, 1.f);
	// return float4(input.normal.x, input.normal.y, input.normal.z, 1.f);
}