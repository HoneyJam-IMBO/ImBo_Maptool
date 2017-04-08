#include "UnpackGBuffer.hlsli"

texture2D gtxtViewer : register(t0);

SamplerState gssWRAP_LINEAR : register(s0);
SamplerState gssWRAP_POINT : register(s1);
SamplerState gssCLAMP_LINEAR : register(s2);
SamplerState gssCLAMP_POINT : register(s3);

struct VS_TEXTURED_OUTPUT {
	float4 position : SV_POSITION;
	float2 texCoord : TEXCOORD0;
};

float4 main(VS_TEXTURED_OUTPUT input) : SV_Target{
	float4 cColor = gtxtViewer.Sample(gssWRAP_LINEAR, input.texCoord);
	//return cColor;

	float depth = cColor.x;
	float2 cpPos = input.texCoord * float2(2.0, -2.0) - float2(1.0, -1.0);
	//float2 cpPos = (input.texCoord * 2.f) - 1.f;//0-1 -> -1~1
	float3 location3 = float3(cpPos, 0);
	//float depth = gtxtViewer.Load(location3).x;
	float Zn = 0.01;
	float Zf = 1000.0;
	float Q = (Zf / (Zn - Zf));
	//float linearDepth = -(Q*Zn) / (depth - Q);
	float linearDepth = ConvertDepthToLinear(depth);
	linearDepth *= 0.001;
	//float linearDepth = (-0.01f/ (1000.f - 0.01f))/ (depth - (1000.f / (1000.f - 0.01f)));
	//float3 positionW = CalcWorldPos(location3.xy, linearDepth);
	return float4(linearDepth, linearDepth, linearDepth, 1.0);
	float4 finalColor;
	finalColor = float4(1.0 - saturate(linearDepth / 10), 1.0 - saturate(linearDepth / 25), 1.0 - saturate(linearDepth / 1000.0), 0.0);
	//��ȣ �̰� xyz�� �ִ� 10 25 1000�� ���� �Ÿ��� ��ȣ �ű��ض� �³� �������� ������������
	//���ѰŸ�! �̰� ���� �ؽ��� �׸��� ��Ծ �ɵ�
	//finalColor = float4(positionW, 0.0);
	//finalColor = float4(0.f, 0.f, 1.0 - saturate(linearDepth / 200.0), 0.0);
	//return finalColor;
	return float4(linearDepth, linearDepth, linearDepth, 1.0);


	//cColor.r /= 1000;
	//cColor.r += 0.05;
}