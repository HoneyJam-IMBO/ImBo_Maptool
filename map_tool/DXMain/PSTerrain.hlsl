#include "PackGbuffer.hlsli"
#include "GetWorldNormal.hlsli"

texture2D gtxtBase : register(t0);
texture2D gtxtDetail : register(t1);
texture2D gtxtPicpos : register(t2);
sampler gssTerrain : register(s0);
sampler gssPicpos : register(s2);

struct Material {
	float3 normal;
	float4 diffuseColor;
	float specExp;
	float specIntensity;
};

cbuffer gMaterialInfo : register(b3) {
	float4 gMaterialColor : packoffset(c0);
	float gSpecExp : packoffset(c1.x);
	float gSpecIntensity : packoffset(c1.y);
}
cbuffer gPicposRenderInfo : register(b4) {
	float2 gPickpos : packoffset(c0);
	float gRenderRadius : packoffset(c0.z);
}

struct DS_OUT {
	float4 position : SV_POSITION;
	float3 positionW : POSITION;
	float2 texCoord : TEXCOORD;
	float2 detailTexCoord : TEXCOORD1;
	float3 tangentW : TANGENT;
	float3 bitangentW : BITANGET;
};

PS_GBUFFER_OUT main(DS_OUT input)
{
	PS_GBUFFER_OUT output = (PS_GBUFFER_OUT)0;

//get color
float4 cColor = gtxtBase.Sample(gssTerrain, input.texCoord);
float4 cDetailColor = gtxtDetail.Sample(gssTerrain, input.detailTexCoord);
cColor = cColor + cDetailColor;

//picpos render
float2 minPos = gPickpos - float2(gRenderRadius, gRenderRadius);
float2 maxPos = gPickpos + float2(gRenderRadius, gRenderRadius);
bool x = ((input.texCoord.x > minPos.x) & (input.texCoord.x < maxPos.x));
bool y = ((input.texCoord.y > minPos.y) & (input.texCoord.y < maxPos.y));
if (x & y) {//���� uv ��ǥ�� ���� ���� ���� �ִٸ�
	cColor = gtxtPicpos.Sample(gssPicpos, input.texCoord);
}
//picpos render

//calc depth
float Depth = input.position.z / input.position.w;

//get world normal
float3 normalW = GetWorldNormal(input.tangentW, input.bitangentW, input.texCoord);
//float3 BaseColor, float3 Normal, float3 PositionW, float SpecIntensity, float SpecPower, float Depth
//return (PackGBuffer(cColor, float3(0, 1, 0), input.positionW, gSpecIntensity, gSpecExp, Depth));
return (PackGBuffer((float3)cColor, normalW, gSpecIntensity, gSpecExp));
}