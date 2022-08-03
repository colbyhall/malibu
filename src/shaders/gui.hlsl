cbuffer bufs : register(b0) {
	float4x4 local_to_projection;
}

struct PSInput
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

PSInput vs_main(float3 position : POSITION, float4 color: COLOR, float2 uv: UV)
{
	PSInput result;

	result.position = mul(local_to_projection, float4(position, 1.0));
	result.color = color;

	return result;
}

float4 ps_main(PSInput input) : SV_TARGET
{
	return float4(input.color.x, input.color.y, input.color.z, 1.f);

	// float3 color = input.color
	// float3 a = float3(0.f, 0.f, 0.f);
	// float3 b = float3(2.f, 3.f, -5.f);
	// float3 light_direction = normalize(a - b);
	// float3 light_color = float3(0.5f, 0.5f, 0.5f);
	// float3 ambient = float3(0.1f, 0.1f, 0.1f);
	// float3 diffuse = light_color * max(dot(input.normal, light_direction), 0.f);
	// float3 final_color = color * (diffuse + ambient);

	// return float4(color.x, color.y, color.z, 1.f);
	// return float4(input.normal.x, input.normal.y, input.normal.z, 1.f);
	// return abs(float4(input.normal.x, input.normal.y, input.normal.z, 1.f));
}