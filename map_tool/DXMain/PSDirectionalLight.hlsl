
//#include "Directional_AmbientLight.hlsli"
#include "UnpackGBuffer.hlsli"
#include "CalcDirectionalLight.hlsli"

Texture2D<float> AOTex : register(t4);
sampler LinearSampler : register(s0);

struct VS_TEXTURED_OUTPUT {
	float4 position : SV_POSITION;
	float2 cpPos : TEXCOORD0;
};

float4 main(VS_TEXTURED_OUTPUT input) : SV_Target{

	float2 uv = input.position.xy;
	SURFACE_DATA gbd = UnpackGBuffer(uv);

	//SURFACE_DATA gbd = UnpackGBuffer(input.texCoord);

	//데이터를 재질 구조체로 변환
Material mat;
mat.normal = gbd.Normal.xyz;
mat.diffuseColor.xyz = gbd.Color;
mat.diffuseColor.w = 1.0f;//완전 불투명

mat.specExp = g_SpecPowerRange.x + g_SpecPowerRange.y * gbd.SpecPow;
mat.specIntensity = gbd.SpecInt;

//월드 위치 복원
float3 positionW = CalcWorldPos(input.cpPos, gbd.LinearDepth);
float ao = AOTex.Sample(LinearSampler, uv.xy);

//엠비언트 및 디렉셔널 라이트 비중 계산
float4 finalColor;
finalColor.xyz = CalcAmbient(mat.normal, mat.diffuseColor.xyz) * ao;
finalColor.xyz += CalcDirectional(positionW, mat);
//finalColor.xyz = mat.normal;

finalColor.w = 1.0f;

return finalColor;
}