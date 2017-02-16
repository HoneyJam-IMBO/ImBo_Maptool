#pragma once
#include "Object.h"
#include "Terrain.h"

struct TERRAIN_GLOBAL_VALUE {
	float OneSpaceSizeRcp{ 0 };// 1/하나의 공간의 크기
	float OneSideSpaceNumRcp{ 0 };// 1/한의 사이드에 있는 공간 수 
	float OneSideSpaceNum{ 0 };//1 / 한의 사이드에 있는 공간 수 
	float HeightScale{ 0 };//높이값 scale
};
struct TERRAIN_PICPOS_RENDER_INFO {
	XMFLOAT2 PickPos{ 0.f, 0.f };//pick pos/ space_size;
	float RenderSize{ 10.f/SPACE_SIZE };//원의 r/space_size;
	UINT pad;
};

class CSpaceContainer;

class CTerrainContainer : public CObject {
public:
	void Begin(LPCTSTR pHeightmapFileName, int nWidth, int nLength, float HeightScale, CSpaceContainer* pSpaceContainer);
	bool End();

	void Render(shared_ptr<CCamera> pCamera);
	float GetHeight(XMVECTOR xmvPos);

	void SetPicpos(float x, float y);
	void SetRenderRadius(float r);

	void Update(shared_ptr<CCamera> pCamera);
	CGameObject* PickObjects(XMVECTOR xmvWorldCameraStartPos, XMVECTOR xmvRayDir, float& distance);
	void ReadyHeightMap();
	void SetBufferInfo();
	void RisedByPickPos();
private:
	TERRAIN_GLOBAL_VALUE* m_pGlobalTerrainData{ nullptr };
	TERRAIN_PICPOS_RENDER_INFO* m_pPicposRenderInfo{ nullptr };

	shared_ptr<CBuffer> m_pGlobalTerrainBuffer{ nullptr };
	shared_ptr<CBuffer> m_pPicposRenderInfoBuffer{ nullptr };
	shared_ptr<CTexture> m_pPicposTexture{ nullptr };


	ID3D11RasterizerState* m_pd3dSpaceRSState{ nullptr };
	ID3D11RasterizerState* m_pd3dTempRSState{ nullptr };
	ID3D11DepthStencilState* m_pd3dDepthStencilState{ nullptr };
	ID3D11DepthStencilState* m_pd3dTempDepthStencilState{ nullptr };
	UINT m_TempStencil{ 0 };

	CSpaceContainer* m_pSpaceContainer{ nullptr };
	//CGlobalTerrain* m_pGlobalTerrain{ nullptr };
	CRenderContainer* m_pTerrainRenderContainer{ nullptr };
	CRenderContainer* m_pSkyboxContainer{ nullptr };


	int m_nWidth{ 0 };
	int m_nLength{ 0 };
	BYTE* m_pHeightMapImage{ nullptr };
	XMFLOAT3 m_xmf3Scale;

	Pixel* m_pHeightData{ nullptr };
	shared_ptr<CTexture> m_pHeightMapTexture{ nullptr };
	vector<CTerrain*> m_vpTerrain;
public:
	CTerrainContainer();
	~CTerrainContainer();
};