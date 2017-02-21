#include "PackGbuffer.hlsli"
#include "GetWorldNormal.hlsli"

texture2D gtxtBase : register(t0);
texture2D gtxtDetail : register(t1);
texture2D gtxtPicpos : register(t2);
texture2D gtxtBlendInfo : register(t3);
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

//get color
float4 cColor = gtxtBase.Sample(gssTerrain, input.texCoord);
float4 cDetailColor = gtxtDetail.Sample(gssTerrain, input.detailTexCoord);
cColor = cColor + cDetailColor;
//get blend info
float4 cBlendData = gtxtBlendInfo.Sample(gssTerrain, input.texCoord);//blend info
cColor.w = cBlendData.r;
//if (cColor.w == 0) discard;//이 코드가 ds에 있어야 한다.
//get color

cColor += RenderPickPos(input.texCoord);

//get world normal
float3 normalW = GetWorldNormal(input.tangentW, input.bitangentW, input.texCoord);
return (PackGBuffer((float3)cColor, normalW, gSpecIntensity, gSpecExp));
}