
cbuffer RayTraceConstants : register(b0)
{
	float2 SunPos : packoffset(c0);
	float InitDecay : packoffset(c0.z);
	float DistDecay : packoffset(c0.w);
	float3 RayColor : packoffset(c1);
	float MaxDeltaLen : packoffset(c1.w);
}

Texture2D<float> IntensityTex : register(t0);
SamplerState LinearSampler: register(s0);

struct VS_OUTPUT {
	float4 position : SV_POSITION;
	float2 UV : TEXCOORD0;
};

float4 main(VS_OUTPUT In) : SV_TARGET
{
	float rayIntensity = IntensityTex.Sample(LinearSampler, In.UV);

	return float4(RayColor * rayIntensity, 1.0f);
}