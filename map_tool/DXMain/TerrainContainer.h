#pragma once
#include "Object.h"
#include "Terrain.h"
//#include "RenderContainerSeller.h"
#include "GlobalTerrain.h"

class CSpaceContainer;

class CTerrainContainer : public CObject {
public:
	void Begin(LPCTSTR pHeightmapFileName, int nWidth, int nLength, float HeightScale, CSpaceContainer* pSpaceContainer);
	bool End();

	void Render(shared_ptr<CCamera> pCamera);
	float GetHeight(XMVECTOR xmvPos);
private:
	ID3D11RasterizerState* m_pd3dSpaceRSState{ nullptr };
	ID3D11RasterizerState* m_pd3dTempRSState{ nullptr };
	ID3D11DepthStencilState* m_pd3dDepthStencilState{ nullptr };
	ID3D11DepthStencilState* m_pd3dTempDepthStencilState{ nullptr };
	UINT m_TempStencil{ 0 };

	CSpaceContainer* m_pSpaceContainer{ nullptr };
	CGlobalTerrain* m_pGlobalTerrain{ nullptr };
	CRenderContainer* m_pTerrainRenderContainer{ nullptr };
	CRenderContainer* m_pSkyboxContainer{ nullptr };


	int m_nWidth{ 0 };
	int m_nLength{ 0 };
	BYTE* m_pHeightMapImage{ nullptr };
	XMFLOAT3 m_xmf3Scale;

	shared_ptr<CBuffer> m_pHeightMapBuffer{ nullptr };
public:
	CTerrainContainer();
	~CTerrainContainer();
};