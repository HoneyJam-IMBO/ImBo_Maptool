#include "UnpackGBuffer.hlsli"

texture2D gtxtViewer : register(t0);
sampler gssViewer : register(s0);

struct VS_TEXTURED_OUTPUT {
	float4 position : SV_POSITION;
	float2 texCoord : TEXCOORD0;
};

float4 main(VS_TEXTURED_OUTPUT input) : SV_Target{
	float4 cColor = gtxtViewer.Sample(gssViewer, input.texCoord);
	return cColor;

	float depth = cColor.x;
	float2 cpPos = (input.texCoord * 2.f) - 1.f;//0-1 -> -1~1
	float3 location3 = float3(cpPos, 0);
	//float depth = gtxtViewer.Load(location3).x;
	float Zn = 0.01f;
	float Zf = 10000.f;
	float Q = (Zf / (Zn - Zf));
	//float linearDepth = -(Q*Zn) / (depth - Q);
	float linearDepth = ConvertDepthToLinear(depth);
	//float3 positionW = CalcWorldPos(location3.xy, linearDepth);
	float4 finalColor;
	finalColor = float4(1.0 - saturate(linearDepth / 10), 1.0 - saturate(linearDepth / 25), 1.0 - saturate(linearDepth / 1000.0), 0.0);
	//오호 이게 xyz에 있는 10 25 1000이 제한 거리네 오호 신기해랑 맞네 ㅂㄷㅂㄷ ㅋㅋㅋㅋㅋㅋ
	//제한거리! 이거 깊이 텍스쳐 그릴때 써먹어도 될듯
	//finalColor = float4(positionW, 0.0);
	//finalColor = float4(0.f, 0.f, 1.0 - saturate(linearDepth / 200.0), 0.0);
	return finalColor;
	return float4(linearDepth, linearDepth, linearDepth, 1.f);

	
	//cColor.r /= 1000;
	//cColor.r += 0.05;
}