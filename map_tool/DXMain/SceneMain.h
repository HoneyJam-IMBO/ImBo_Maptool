#pragma once

#include "DirectXFramework.h"
#include "Scene.h"

//seller
#include "RenderContainerSeller.h"

//object
#include "FreeCamera.h"
#include "ThirdPersonCamera.h"
#include "Bunny.h"
#include "TestCube.h"
//player
#include "Player.h"
//space
#include "SpaceContainer.h"
//terrain
#include "TerrainContainer.h"
//skybox
#include "SkyBoxContainer.h"
//light
#include "PointLight.h"
#include "SpotLight.h"
#include "CapsuleLight.h"
#include "DirectionalLight.h"

static XMFLOAT4 xmf4DirectionalLightColor;

class CSceneMain;
struct LoadFileStruct {
	CSceneMain* m_pScene;
	string Filename;
};

class CSceneMain :public CScene{

public:
	//-----------------------------------scene--------------------------
	virtual bool Begin();
	virtual bool End();

	virtual void Animate(float fTimeElapsed);

	virtual void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual void ProcessInput(float fTimeElapsed);


	CGameObject* PickObjectPointedByCursor(int xClient, int yClient);
	CTerrainContainer* GetTerrainContainer() { return m_pTerrainContainer; }

	//animation tool을 위한 함수
	void CreateControllObject(string path);
	void CreateLoadFileUI();
	void AddFBXAnimationInfo(string path);
	void CreateAddInfoUI();
	void ClearAllFBXObject();

	void ClearAllFBXUI();

	void CreateBaseTextureSelectUI();
	void CreateObjectPositioningUI();
	//변수
	vector<LoadFileStruct> m_LoadFileStruct;
	//fbx object
	CTestObject* m_pFBXObject{ nullptr };
	
	void ObjectPositioning();
//	void SetPositioningObject(CGameObject* pObject) { m_pPositioningObject = pObject; };
private:
	int m_MeshCnt{ 0 };
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

	//player
	CPlayer* m_pPlayer{ nullptr };
	//player
	
	//picking
	CGameObject* m_pPickingObject{ nullptr };

	//모든 생성 가능한 객체를 벡터로 미리 만들어 둔다.
	//이 안의 모든 객체를 버튼으로써 제작한다.
	vector<CGameObject*> m_vpObjectList;
public:
	CSceneMain(CDirectXFramework* pFrameWork);
	~CSceneMain();
};