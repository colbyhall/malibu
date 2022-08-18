cbuffer bufs : register(b0) {
	float4x4 local_to_projection;
}

Texture2D texture2d_table[] : register(t0);
SamplerState sampler_table : register(s0);

struct PSInput {
	float4 position : SV_POSITION;
	float4 color : COLOR;
	float4 scissor : SCISSOR;
	float2 uv : UV;
	float2 screen_pos : SCREEN_POS;
	uint tex2d : TEX;
};

struct VSInput {
	float3 position : POSITION;
	float4 color : COLOR;
	float4 scissor : SCISSOR;
	float2 uv : UV;
	uint tex2d : TEX;
};

PSInput vs_main(VSInput input) {
	PSInput result;

	result.position = mul(local_to_projection, float4(input.position, 1.0));
	result.color = input.color;
	result.scissor = input.scissor;
	result.uv = input.uv;
	result.screen_pos = input.position.xy;
	result.tex2d = input.tex2d;

	return result;
}

float4 ps_main(PSInput input) : SV_TARGET {
	bool in_scissor =
		input.screen_pos.x >= input.scissor.x &&
		input.screen_pos.y >= input.scissor.y &&
		input.screen_pos.x <= input.scissor.z &&
		input.screen_pos.y <= input.scissor.w;

	float4 output = input.color;
	if (true) {
		if (input.tex2d == 0) {
			return input.color;
		}
		float dist_alpha_mask = texture2d_table[input.tex2d].Sample(sampler_table, input.uv, 0).x;
		output.w *= smoothstep(0.6, 0.7, dist_alpha_mask);
	}
	return output;

}