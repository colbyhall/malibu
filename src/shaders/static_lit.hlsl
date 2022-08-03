cbuffer bufs : register(b0) {
	float4x4 local_to_projection;
}

#define PI 3.141569

float3 fresnel_schlick(float cos_theta, float3 f0) {
	return f0 + (1.0 - f0) * pow(clamp(1.0 - cos_theta, 0.0, 1.0), 5.0);
}

float distribution_ggx(float3 n, float3 h, float roughness) {
	float a = roughness * roughness;
	float a2 = a * a;
	float n_dot_h = max(dot(n, h), 0.0);
	float n_dot_h_2 = n_dot_h * n_dot_h;

	float num = a2;
	float denom = n_dot_h_2 * (a2 - 1.0) + 1.0;
	denom = PI * denom * denom;

	return num / denom;
}

float geometry_schlick_ggx(float n_dot_v, float roughness) {
	float r = roughness + 1.0;
	float k = (r * r) / 8.0;

	float num = n_dot_v;
	float denom = n_dot_v * (1.0 - k) + k;

	return num / denom;
}

float geometry_smith(float3 n, float3 v, float3 l, float roughness) {
	float n_dot_v = max(dot(n, v), 0.0);
	float n_dot_l = max(dot(n, l), 0.0);
	float ggx2 = geometry_schlick_ggx(n_dot_v, roughness);
	float ggx1 = geometry_schlick_ggx(n_dot_l, roughness);

	return ggx1 * ggx2;
}

struct PSInput
{
	float4 position : SV_POSITION;
	float3 normal : NORMAL;
};

PSInput vs_main(float3 position : POSITION, float3 normal: NORMAL)
{
	PSInput result;

	float4 adjusted = float4(position.x, position.y, position.z, 1.0);
	result.position = mul(local_to_projection, adjusted);
	result.normal = normal;

	return result;
}

float4 ps_main(PSInput input) : SV_TARGET
{
	float3 color = float3(0.f, 1.f, 0.f);

	float3 a = float3(0.f, 0.f, 0.f);
	float3 b = float3(2.f, 3.f, -5.f);
	float3 light_direction = normalize(a - b);
	float3 light_color = float3(0.5f, 0.5f, 0.5f);

	float3 ambient = float3(0.1f, 0.1f, 0.1f);

	float3 diffuse = light_color * max(dot(input.normal, light_direction), 0.f);

	float3 final_color = color * (diffuse + ambient);

	return float4(final_color.x, final_color.y, final_color.z, 1.f);
	// return float4(color.x, color.y, color.z, 1.f);
	// return float4(input.normal.x, input.normal.y, input.normal.z, 1.f);
	// return abs(float4(input.normal.x, input.normal.y, input.normal.z, 1.f));
}