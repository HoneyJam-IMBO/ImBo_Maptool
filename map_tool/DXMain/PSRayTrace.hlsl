cbuffer RayTraceConstants : register(b0)
{
	float2 SunPos : packoffset(c0);
	float InitDecay : packoffset(c0.z);
	float DistDecay : packoffset(c0.w);
	float3 RayColor : packoffset(c1);
	float MaxDeltaLen : packoffset(c1.w);
}

Texture2D<float> OcclusionTex : register(t0);
SamplerState LinearSampler : register(s0);

struct VS_OUTPUT {
	float4 position : SV_POSITION;
	float2 UV : TEXCOORD0;
};

static const int NUM_STEPS = 64;
static const float NUM_DELTA = 1.0 / 63.0f;
float4 main(VS_OUTPUT In) : SV_TARGET
{
	// SS�� sun���� dir�� ���Ѵ�.
	float2 dirToSun = (SunPos - In.UV);
	float lengthToSun = length(dirToSun);
	dirToSun /= lengthToSun;

	// Find the ray delta
	float deltaLen = min(MaxDeltaLen, lengthToSun * NUM_DELTA);
	float2 rayDelta = dirToSun * deltaLen;

	// Each step decay	
	float stepDecay = DistDecay * deltaLen;

	// Initial values
	float2 rayOffset = float2(0.0, 0.0);
	float decay = InitDecay;
	float rayIntensity = 0.0f;

	// Ray Trace �¾� ����
	for (int i = 0; i < NUM_STEPS; i++){
		// Sample at the current location
		float2 sampPos = In.UV + rayOffset;
		float fCurIntensity = OcclusionTex.Sample(LinearSampler, sampPos);
		//���� ������
		rayIntensity += fCurIntensity * decay;
		rayOffset += rayDelta;
		//���Ⱚ ����
		decay = saturate(decay - stepDecay);
	}

	return float4(rayIntensity, 0.0, 0.0, 0.0);
}