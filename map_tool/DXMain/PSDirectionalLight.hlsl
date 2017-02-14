
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

	//�����͸� ���� ����ü�� ��ȯ
Material mat;
mat.normal = gbd.Normal.xyz;
mat.diffuseColor.xyz = gbd.Color;
mat.diffuseColor.w = 1.0f;//���� ������

mat.specExp = g_SpecPowerRange.x + g_SpecPowerRange.y * gbd.SpecPow;
mat.specIntensity = gbd.SpecInt;

//���� ��ġ ����
float3 positionW = CalcWorldPos(input.cpPos, gbd.LinearDepth);
float ao = AOTex.Sample(LinearSampler, uv.xy);

//�����Ʈ �� �𷺼ų� ����Ʈ ���� ���
float4 finalColor;
finalColor.xyz = CalcAmbient(mat.normal, mat.diffuseColor.xyz) * ao;
finalColor.xyz += CalcDirectional(positionW, mat);
//finalColor.xyz = mat.normal;

finalColor.w = 1.0f;

return finalColor;
}