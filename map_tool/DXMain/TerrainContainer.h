#pragma once
#include "Object.h"
#include "Terrain.h"
#include "StempManager.h"
#include "SplattingInfoManager.h"

enum STEMP_MODE {
	STEMP_MODE_SET,
	STEMP_MODE_INDE
};

struct TERRAIN_GLOBAL_VALUE {
	float OneSpaceSizeRcp{ 0 };// 1/하나의 공간의 크기
	float OneSideSpaceNumRcp{ 0 };// 1/한의 사이드에 있는 공간 수 
	float OneSideSpaceNum{ 0 };//1 / 한의 사이드에 있는 공간 수 
	float HeightScale{ 0 };//높이값 scale
};
class CSpaceContainer;

class CTerrainContainer : public CObject {
public:
	void Begin();
	bool End();

	void Render(shared_ptr<CCamera> pCamera);
	float GetHeight(XMVECTOR xmvPos);
	float GetHeight(XMFLOAT2 xmf2Pos);

	void SetPicpos(float x, float y);
	void SetRenderRadius(float r);

	void Update(shared_ptr<CCamera> pCamera);
	CGameObject* PickObjects(XMVECTOR xmvWorldCameraStartPos, XMVECTOR xmvRayDir, float& distance);
	void ReadyHeightMap();
	void SetBufferInfo();
	void IncreasePickPosHeight();
	void DecreasePickPosHeight();
	void SetPickPosHeight();

	void SetStempMode(STEMP_MODE mode) { m_StempMode = mode; }
	void SetBaseTexture(wstring path);

	void CreateSplattingInfo();
	CSplattingInfoManager* GetSplattingInfoManager() { return m_pSplattingInfoManager; }
	CStempManager* GetStempManager() { return m_pStempManager; }

	XMFLOAT2 GetCurPickPos() { return m_xmf2CurPickPos; }

	void CreateNormalMap();

	static CTerrainContainer* CreateTerrainContainer(LPCTSTR pTerrainName, int nWidth, int nLength, float HeightScale, CSpaceContainer* pSpaceContainer, bool isTool = false);
	void SetTerrainWidth(int nWidth) { m_nWidth = nWidth; }
	void SetTerrainLength(int nLength) { m_nLength = nLength; }
	void SetHeightScale(float scale);
	float GetHeightScale() { return m_xmf3Scale.y; }
	void SetTerrainScale(XMFLOAT3 xmf3Scale) { m_xmf3Scale = xmf3Scale; }
	void SetSpaceContainer(CSpaceContainer* pSpaceContainer) { m_pSpaceContainer = pSpaceContainer; }
	CSpaceContainer* GetSpaceContainer() { return m_pSpaceContainer; }
	void SetTerrainName(wstring name) { m_wsTerrainName = name; }
	wstring GetTerrainName() { return m_wsTerrainName; }
	void SetIsTool(bool b) { m_bIsTool = b; }
	bool GetIsTool() { return m_bIsTool; }
	void CreateResetTextures(LPCTSTR pTerrainName);
	void CreateTerrainTextures(LPCTSTR pTerrainName);
	void CreateTerrainMesh(CSpaceContainer* pSpaceContainer);

	void ChangeSpaceData();

	void SetActive(bool b) { m_bActive = b; }
private:
	bool m_bIsTool{ false };
	bool m_bActive{ true };
	wstring m_wsTerrainName;
	XMFLOAT2 m_xmf2CurPickPos{ 0.f, 0.f };
	STEMP_MODE m_StempMode{ STEMP_MODE_SET };

	CStempManager * m_pStempManager{ nullptr };
	TERRAIN_GLOBAL_VALUE* m_pGlobalTerrainData{ nullptr };
	shared_ptr<CBuffer> m_pGlobalTerrainBuffer{ nullptr };

	ID3D11RasterizerState* m_pd3dSpaceRSState{ nullptr };
	ID3D11RasterizerState* m_pd3dTempRSState{ nullptr };
	
	CSpaceContainer* m_pSpaceContainer{ nullptr };
	//CGlobalTerrain* m_pGlobalTerrain{ nullptr };
	CRenderContainer* m_pTerrainRenderContainer{ nullptr };
	
	int m_nWidth{ 0 };
	int m_nLength{ 0 };
	XMFLOAT3 m_xmf3Scale;

	Pixel24* m_pHeightData{ nullptr };
	Pixel24* m_pNormalData{ nullptr };
	shared_ptr<CTexture> m_pHeightMapTexture{ nullptr };
	shared_ptr<CTexture> m_pBaseTexture{ nullptr };
	shared_ptr<CTexture> m_pNormalTexture{ nullptr };
	vector<CTerrain*> m_vpTerrain;//pick object를 위한 녀석
	//안의 terrain들은 space에서 관리되고 사라지니까 end 및 delete할 필요가 없다.

	//splatting infos
	CSplattingInfoManager* m_pSplattingInfoManager{ nullptr };
	
public:
	CTerrainContainer();
	~CTerrainContainer();
};