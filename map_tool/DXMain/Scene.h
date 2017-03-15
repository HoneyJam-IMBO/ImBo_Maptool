#pragma once

#include "DXObject.h"
#include "GameObject.h"

//space
#include "SpaceContainer.h"
//terrain
#include "TerrainContainer.h"
//skybox
#include "SkyBoxContainer.h"

class CDirectXFramework;

class CScene{

public:
	//begin end
	virtual bool Begin() { return true; };
	virtual bool Begin(string path);
	virtual bool End();
	//begin end
	//animate
	virtual void Animate(float fTimeElapsed);
	//animate
	//input
	virtual void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) {};
	virtual void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) {};
	virtual void ProcessInput(float fTimeElapsed) {};
	//input
	//pick
	virtual CGameObject* PickObjectPointedByCursor(int xClient, int yClient) { return nullptr; };
	//pick
	//create scene
	static CScene* CreateScene(string name, CDirectXFramework* pFramework);
	//create scene
	void ChangeSceneContainers();//scene creater func
protected:
	string m_sName;

	//framework
	CDirectXFramework* m_pFrameWork{ nullptr };
	//진짜 카메라는 framework에 있다.
	shared_ptr<CCamera> m_pCamera{ nullptr };
	//space
	CSpaceContainer* m_pSpaceContainer{ nullptr };
	//space
	//terrain
	CTerrainContainer* m_pTerrainContainer{ nullptr };
	//terrain
	//skybox
	CSkyBoxContainer* m_pSkyBoxContainer{ nullptr };
	//skybox

	//picking
	CGameObject* m_pPickingObject{ nullptr };

	//space
	float m_space_size{ 512 };
	float m_space_lv{ 2 };
	float m_fHeightScale{ 1.f };
	//ssao
	float m_fSSAORadius{ 0.f };
	float m_fSSAOOffsetRadius{ 0.f };
	//bloom
	float m_fBLOOMThreshold{ 2.f };
	float m_fBLOOMMiddleGrey{ 0.01f };
	float m_fBLOOMWhite{ 2.0f };
	float m_fBLOOMScale{ 2.0f };
	//sslr
	bool m_bSSLROnOff;
	float m_fSSLROffsetSunPos{ -200.f };
	float m_fSSLRMaxSunDist{ 1000.f };
	float m_fSSLRInitDecay{ 0.05f };
	float m_fSSLRDistDecay{ 0.05f };
	float m_fSSLRMaxDeltaLen{ 0.05f };

private:

public:
	CScene::CScene(string name) { m_sName = name; }
	~CScene() {}

	//set get
	//ssao
	void SetSSAORadius(float fSSAORadius) { m_fSSAORadius = fSSAORadius; };
	float GetSSAORadius() { return m_fSSAORadius; }
	void SetSSAOOffsetRadius(float SSAOOffsetRadius) { m_fSSAOOffsetRadius = SSAOOffsetRadius; }
	float GetSSAOOffsetRadius() { return m_fSSAOOffsetRadius; }
	//bloom
	void SetBLOOMThreshold(float BLOOMThreshold) { m_fBLOOMThreshold = BLOOMThreshold; }
	float GetBLOOMThreshold() { return m_fBLOOMThreshold; }
	void SetBLOOMMiddleGrey(float BLOOMMiddleGrey) { m_fBLOOMMiddleGrey = BLOOMMiddleGrey; }
	float GetBLOOMMiddleGrey() { return m_fBLOOMMiddleGrey; }
	void SetBLOOMWhite(float BLOOMWhith) { m_fBLOOMWhite = BLOOMWhith; }
	float GetBLOOMWhite() { return m_fBLOOMWhite; }
	void SetBLOOMScale(float BLOOMScale) { m_fBLOOMScale = BLOOMScale; }
	float GetBLOOMScale() { return m_fBLOOMScale; }
	//sslr
	void SetSSLROnOff(bool SSLROnOff) { m_bSSLROnOff = SSLROnOff; }
	bool GetSSLROnOff() { return m_bSSLROnOff; }
	void SetSSLROffsetSunPos(float SSLROffsetSunPos) { m_fSSLROffsetSunPos = SSLROffsetSunPos; }
	float GetSSLROffsetSunPos() { return m_fSSLROffsetSunPos; }
	void SetSSLRMaxSunDist(float SSLRMaxSunDist) { m_fSSLRMaxSunDist = SSLRMaxSunDist; }
	float GetSSLRMaxSunDist() { return m_fSSLRMaxSunDist; }
	void SetSSLRInitDecay(float SSLRInitDecay) { m_fSSLRInitDecay = SSLRInitDecay; }
	float GetSSLRInitDecay() { return m_fSSLRInitDecay; }
	void SetSSLRDistDecay(float SSLRDistDecay) { m_fSSLRDistDecay = SSLRDistDecay; }
	float GetSSLRDistDecay() { return m_fSSLRDistDecay; }
	void SetSSLRMaxDeltaLen(float fSSLRMaxDeltaLen) { m_fSSLRMaxDeltaLen = fSSLRMaxDeltaLen; }
	float GetSSLRMaxDeltaLen() { return m_fSSLRMaxDeltaLen; }

	void SetTerrainContainer(CTerrainContainer* pTerrainContainer) { m_pTerrainContainer = pTerrainContainer; }
	CTerrainContainer* GetTerrainContainer() { return m_pTerrainContainer; }
	void SetSpaceContainer(CSpaceContainer* pSpaceContainer) { m_pSpaceContainer = pSpaceContainer; }
	CSpaceContainer* GetSpaceContainer() { return m_pSpaceContainer; }
	void SetSkyBoxContainer(CSkyBoxContainer* pSkyBoxContainer) { m_pSkyBoxContainer = pSkyBoxContainer; }
	CSkyBoxContainer* GetSkyBoxContainer() { return m_pSkyBoxContainer; }

	void SetFrameWork(CDirectXFramework* pFramework) { m_pFrameWork = pFramework; }
	CDirectXFramework* GetFrameWork() { return m_pFrameWork; }
	//set get
};
