#pragma once

#include "SingleTon.h"

//
#include "DirectionalLight.h"
#include "TerrainContainer.h"
#include "ObjectRenderer.h"
#include "AORenderer.h"
#include "LightRenderer.h"
#include "BloomDownScale.h"
#include "Bloom.h"
#include "Blur.h"
#include "PostProcessingFinalPass.h"
#include "SSLR.h"

class CRenderer : public CSingleTonBase<CRenderer> {

public:
	bool Begin();
	bool End();

	void Render(shared_ptr<CCamera> pCamera);
	void PostProcessing(shared_ptr<CCamera> pCamera);
	void Update(float fTimeElapsed);

	void SetForwardRenderTargets();
	void SetMainRenderTargetView();
	void SetRenderTargetViews(UINT nRenderTarget, ID3D11RenderTargetView** pd3dRTVs, ID3D11DepthStencilView* pd3ddsvDepthStencil);

	bool CreateSwapChain();
	bool CreateRenderTargetView();
	void ReleaseForwardRenderTargets();
	void ClearDepthStencilView(ID3D11DepthStencilView* pDepthStencilView);

	bool ResizeBuffer();
	void SetTerrainContainer(CTerrainContainer* pTerrainContainer) { m_pTerrainContainer = pTerrainContainer; }
	void SetDirectionalLight(CDirectionalLight* pDirectionalLight) { m_pDirectionalLIght = pDirectionalLight; }
private:
	//SSAO 관련 변수
	float m_SSAO_OffsetRadius{ 20.f };
	float m_SSAO_Radius{ 1000.f };
	//SSAO 관련 변수

	//BLOOM 관련 변수
	float m_fBloomThreshold{ 2.0f };//블룸할 색 구할때 강조값
	float m_fMiddleGrey = { 0.0025f };//중간 회색
	float m_fWhite = { 1.5f };//가장 밝은 색
	float m_fBloomScale{ 0.1f };//블룸 강조값 
	//BLOOM 관련 변수

	//SSLR관련 변수
	//이건 scene에서 delete해줄 변수
	CDirectionalLight* m_pDirectionalLIght{ nullptr };
	//SSLR관련 변수

	IDXGISwapChain			*	m_pdxgiSwapChain{ nullptr };
	ID3D11RenderTargetView	*	m_pd3dRenderTargetView{ nullptr };
	
	ID3D11DepthStencilState* m_pd3dDepthStencilState{ nullptr };
	ID3D11DepthStencilView		*m_pd3ddsvReadOnlyDepthStencil{ nullptr };

	//---------------------------deferred rendering----------------------
	ID3D11Texture2D			 *m_pd3dtxtDepthStencil{ nullptr };
	ID3D11Texture2D			 *m_pd3dtxtColorSpecInt{ nullptr };//0
	ID3D11Texture2D			 *m_pd3dtxtNormal{ nullptr };//1
	ID3D11Texture2D			 *m_pd3dtxtSpecPow{ nullptr };//2


	ID3D11ShaderResourceView *m_pd3dsrvDepthStencil{ nullptr };
	ID3D11ShaderResourceView *m_pd3dsrvColorSpecInt{ nullptr };//0
	ID3D11ShaderResourceView *m_pd3dsrvNormal{ nullptr };//1
	ID3D11ShaderResourceView *m_pd3dsrvSpecPow{ nullptr };//2

	ID3D11DepthStencilView	 *m_pd3ddsvDepthStencil{ nullptr };
	ID3D11RenderTargetView   *m_pd3drtvColorSpecInt{ nullptr };//0
	ID3D11RenderTargetView   *m_pd3drtvNormal{ nullptr };//1
	ID3D11RenderTargetView   *m_pd3drtvSpecPow{ nullptr };//2

	vector<shared_ptr<CTexture>> m_vObjectLayerResultTexture;
	vector<shared_ptr<CTexture>> m_vLightLayerResultTexture;

	//--------------------------light render target----------------
	ID3D11Texture2D			 *m_pd3dtxtLight{ nullptr };
	ID3D11ShaderResourceView *m_pd3dsrvLight{ nullptr };
	ID3D11RenderTargetView   *m_pd3drtvLight{ nullptr };
	//--------------------------light render target----------------
	//-------------------------layer-------------------------
	CTerrainContainer* m_pTerrainContainer{ nullptr };
	CObjectRenderer* m_pObjectRenderer{ nullptr };
	CAORenderer* m_pAORenderer{ nullptr };
	CLightRenderer* m_pLightRenderer{ nullptr };
	CBloomDownScale * m_pBloomDownScale{ nullptr };
	CBloom* m_pBloom{ nullptr };
	CBlur* m_p16to1Blur{ nullptr };
	CBlur* m_p4to1Blur{ nullptr };
	CPostProcessingFinalPass* m_pPostProcessingFinalPass{ nullptr };
	CSSLR* m_pSSLR{ nullptr };
	//-------------------------layer-------------------------
public:
	CRenderer();
	virtual ~CRenderer();
};
