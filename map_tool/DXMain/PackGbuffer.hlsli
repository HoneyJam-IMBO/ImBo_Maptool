

//specpowr 정규화 
static const float2 g_SpecPowerRange = { 0.1, 250.0 };


struct PS_GBUFFER_OUT {
	float4 ColorSpecInt : SV_TARGET0;
	float4 Normal : SV_TARGET1;
	float4 SpecPow : SV_TARGET2;	
};

PS_GBUFFER_OUT PackGBuffer(float3 BaseColor, float3 Normal, float SpecIntensity, float SpecPower) {
	PS_GBUFFER_OUT Out = (PS_GBUFFER_OUT)0;

	//스펙큘러 파워 정규화
	float SpecPowerNorm = (SpecPower - g_SpecPowerRange.x) / g_SpecPowerRange.y;

	//GBuffer 구조체에 데이터 패킹
	Out.ColorSpecInt = float4(BaseColor.rgb, SpecIntensity);
	Out.Normal = float4(Normal.xyz * 0.5 + 0.5, 1.f);
	//Out.Normal_Depth = float4(Normal.xyz, Depth);

	//specPow값 패킹
	Out.SpecPow.w = SpecPowerNorm;
	//Out.PositionW_SpecPow.xyz = PositionW;

	
	return Out;
}