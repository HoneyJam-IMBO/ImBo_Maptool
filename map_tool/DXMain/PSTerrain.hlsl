#include "PackGbuffer.hlsli"
#include "GetWorldNormal.hlsli"

//나중에 cBuffer로 바꿀 것임
#define SPLATTING_NUM 1

texture2D gtxtBase : register(t0);
texture2D gtxtDetail : register(t1);
texture2D gtxtBlendInfo : register(t2);
texture2D gtxtPicpos : register(t3);
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
	uint gMode : packoffset(c0.w);
}

struct DS_OUT {
	float4 position : SV_POSITION;
	float3 positionW : POSITION;
	float2 texCoord : TEXCOORD;
	float2 detailTexCoord : TEXCOORD1;
	float3 tangentW : TANGENT;
	float3 bitangentW : BITANGET;
};
float4 RenderPickPos(float2 texCoord) {
	//picpos render
	float2 minPos = gPickpos - float2(gRenderRadius, gRenderRadius);
	float2 maxPos = gPickpos + float2(gRenderRadius, gRenderRadius);
	bool x = ((texCoord.x > minPos.x) & (texCoord.x < maxPos.x));
	bool y = ((texCoord.y > minPos.y) & (texCoord.y < maxPos.y));

	float4 cColor = float4(0.f, 0.f, 0.f, 0.f);
	if (x & y) {//나의 uv 좌표가 원의 범위 내에 있다면
		float2 PickPosUV = float2(1 - ((texCoord.x - minPos.x) / (gRenderRadius * 2)), 1 - ((texCoord.y - minPos.y) / (gRenderRadius * 2)));
		cColor = gtxtPicpos.Sample(gssPicpos, PickPosUV);
	}
	//picpos render
	return cColor;
}
PS_GBUFFER_OUT main(DS_OUT input)
{
	PS_GBUFFER_OUT output = (PS_GBUFFER_OUT)0;

	//set base color
	float4 cBaseTexColor = gtxtBase.Sample(gssTerrain, input.texCoord);
	float4 cDetailTexColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
	float cDetailAlpha = 1.0f;

	float4 cCurrentTexColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
	float4 cPreviousTexColor = cBaseTexColor;

	//	인덱스
	float CurrentTextureIndex = 0;
	cCurrentTexColor = gtxtDetail.Sample(gssTerrain, input.detailTexCoord);//get detail
	//cCurrentTexColor = saturate((cPreviousTexColor * 0.5f) + (cCurrentTexColor * 0.5f)); 
	cDetailAlpha = gtxtBlendInfo.Sample(gssTerrain, input.texCoord).x;
	cDetailTexColor = lerp(cPreviousTexColor, cCurrentTexColor, cDetailAlpha);
	cPreviousTexColor = cDetailTexColor;
	//set base color

	//set splatting detail color
	//while (CurrentTextureIndex < gvTerrainTextureIndex.a)
	//{
	//	cCurrentTexColor = gtxtDetail.Sample(gssTerrain, input.detailTexCoord);
	//	cDetailAlpha = gtxtBlendInfo.Sample(gssTerrain, input.texCoordBase).x;
	//	cDetailTexColor = lerp(cPreviousTexColor, cCurrentTexColor, cDetailAlpha);
	//	cPreviousTexColor = cDetailTexColor;
	//}

	float4 cColor = saturate(cDetailTexColor);

	if (gMode == 0 || gMode == 1) {
		cColor += RenderPickPos(input.texCoord);
	}
	//set splatting detail color
	
	
	//get world normal
	float3 normalW = GetWorldNormal(input.tangentW, input.bitangentW, input.texCoord);
	return (PackGBuffer((float3)cColor, normalW, gSpecIntensity, gSpecExp));
}