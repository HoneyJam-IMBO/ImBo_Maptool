
//#include "Directional_AmbientLight.hlsli"
#include "UnpackGBuffer.hlsli"
#include "CalcDirectionalLight.hlsli"

Texture2D<float> AOTex : register(t4);
Texture2D<float> ShadowMap : register(t5);

SamplerState gssWRAP_LINEAR : register(s0);
SamplerState gssWRAP_POINT : register(s1);
SamplerState gssCLAMP_LINEAR : register(s2);
SamplerState gssCLAMP_POINT : register(s3);
//SamplerComparisonState gssSHADOW : register(s4);
SamplerState gssSHADOW : register(s4);

cbuffer LightVPCBuffer : register(b4)
{
	matrix gmtxLightVP;
};

struct VS_TEXTURED_OUTPUT {
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD0;
};

float2 texOffset(int u, int v)
{
	return float2(u * 1.0f / 4096, v * 1.0f / 4096);
}
float ShaderPCF(float3 position, float cosTheta) {
	float4 posShadowMap = mul(float4(position, 1.f), gmtxLightVP);
	float3 UVD = posShadowMap.xyz / posShadowMap.w;

	UVD.xy = 0.5 * UVD.xy + 0.5;
	UVD.y = 1 - UVD.y;

	float LightDepth = ShadowMap.Sample(gssSHADOW, UVD.xy).x;
	float fCamposDepth = (UVD.z);

	float value = 0.0000003;//d
	float offset = 0.000000105;//d
	float bias = value;
	bias = value*tan(acos(cosTheta)); // cosTheta is dot( n,l ), clamped between 0 and 1
	bias = clamp(bias, value - offset, value + offset);
	
	float nHalfSamples = 4;//d
	float sum = 0;
	if (LightDepth < fCamposDepth - bias) {
		for (float y = -nHalfSamples; y < nHalfSamples + 1.0; y += 1.0) {
			for (float x = -nHalfSamples; x < nHalfSamples + 1.0; x += 1.0) {
				LightDepth = ShadowMap.Sample(gssSHADOW, UVD.xy + float2(x,y)).x;
				if (LightDepth < fCamposDepth - value) {
					sum += 0.5;
				}
			}
		}
		return sum / 16.f;
	}
	return 1.f;
}


float4 main(VS_TEXTURED_OUTPUT input) : SV_Target{

	SURFACE_DATA gbd = UnpackGBuffer_Tex(input.uv);

//데이터를 재질 구조체로 변환
Material mat;
mat.normal = gbd.Normal.xyz;
mat.diffuseColor.xyz = gbd.Color;
mat.diffuseColor.w = 1.0f;//완전 불투명

mat.specExp = g_SpecPowerRange.x + g_SpecPowerRange.y * gbd.SpecPow;
mat.specIntensity = gbd.SpecInt;

//월드 위치 복원
float2 cpPos = input.uv * float2(2.0, -2.0) - float2(1.0, -1.0);
float3 positionW = CalcWorldPos(cpPos, gbd.LinearDepth);
float ao = AOTex.Sample(gssCLAMP_LINEAR, input.uv);

//엠비언트 및 디렉셔널 라이트 비중 계산
float4 finalColor;
finalColor.xyz = CalcAmbient(mat.normal, mat.diffuseColor.xyz) * ao;
finalColor.xyz += CalcDirectional(positionW, mat);
//finalColor.xyz = mat.normal;

float3 dir = float3(0, 0, 0) - DirToLight;
float shadowAtt = ShaderPCF(positionW, dot(mat.normal, dir));
finalColor.rgb *= shadowAtt;
finalColor.a = 1.0f;
return finalColor;


//float depth = gbd.LinearDepth;
//float depth = ShadowMap.Sample(gssWRAP_LINEAR, input.uv); //shadowAtt;
//float depth = ConvertDepthToLinear(shadowAtt) * 0.0001;
//return float4(depth, depth, depth, 1.f);
//float depth = gbd.depth;
//float3 cameraPos = ClacCameraPos(cpPos, depth);
//float3 worldPos = CalcWorldPos(cpPos, depth);
//float linearDepth = gbd.LinearDepth;
//return float4(linearDepth, linearDepth, linearDepth, 1.f);
//return float4(cpPos, gbd.depth, 1.f);
//return float4(cameraPos, 1.f);
//return float4(worldPos, 1.f);
//return finalColor;
}