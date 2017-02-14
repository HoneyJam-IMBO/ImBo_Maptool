#pragma once
#include "DXObject.h"

struct CB_OCCLUSSION
{
	UINT nWidth;
	UINT nHeight;
	UINT pad[2];
};

struct CB_LIGHT_RAYS
{
	XMFLOAT2 vSunPos;
	float fInitDecay;
	float fDistDecay;
	XMFLOAT3 vRayColor;
	float fMaxDeltaLen;
};

class CSSLR : public DXObject{
public:
	bool Begin();
	virtual bool End();

	virtual void SetShaderState();
	virtual void CleanShaderState();

	virtual void UpdateShaderState();

	void Excute(shared_ptr<CCamera>pCamera, ID3D11RenderTargetView* pLightAccumRTV, ID3D11ShaderResourceView* pMiniDepthSRV, const XMVECTOR& vSunDir, const XMFLOAT3& vSunColor);
	void ResizeBuffer();
	void ReleaseBuffer();

private:
	// Prepare the occlusion map
	void PrepareOcclusion(ID3D11ShaderResourceView* pMiniDepthSRV);

	// Ray trace the occlusion map to generate the rays
	void RayTrace(shared_ptr<CCamera>pCamera, const XMFLOAT2& vSunPosSS, const XMFLOAT3& vSunColor);

	// Combine the rays with the scene
	void Combine(shared_ptr<CCamera>pCamera, ID3D11RenderTargetView* pLightAccumRTV);

	UINT m_nWidth{ 0 };
	UINT m_nHeight{ 0 };
	UINT m_nThreadGroup{ 0 };
	float m_fInitDecay{ 0.f };
	float m_fDistDecay{ 0.f };
	float m_fMaxDeltaLen{ 0.f };

	ID3D11Texture2D* m_pOcclusionTex;
	ID3D11UnorderedAccessView* m_pOcclusionUAV;
	ID3D11ShaderResourceView* m_pOcclusionSRV;
	CComputeShader* m_pMakeOcclussionComputeShader{ nullptr };
	shared_ptr<CTexture> m_pOcclussionTexture{ nullptr };

	CRenderContainer* m_pLayTraceRenderContainer{ nullptr };
	ID3D11Texture2D* m_pLightRaysTex;
	ID3D11RenderTargetView* m_pLightRaysRTV;
	ID3D11ShaderResourceView* m_pLightRaysSRV;
	shared_ptr<CTexture> m_pLightRaysTexture{ nullptr };
	
	CRenderContainer* m_pConmbineRenderContainer{ nullptr };

	// Shaders
	shared_ptr<CBuffer> m_pOcclusionCB;
	shared_ptr<CBuffer> m_pRayTraceCB;
	
	// Additive blend state to add the light rays on top of the scene lights
	ID3D11BlendState* m_pAdditiveBlendState;
public:
	CSSLR();
	virtual ~CSSLR();
};

