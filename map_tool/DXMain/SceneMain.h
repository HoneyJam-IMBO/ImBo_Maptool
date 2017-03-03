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

	//animation tool�� ���� �Լ�
	void CreateControllObject(string path);
	void CreateLoadFileUI();
	void AddFBXAnimationInfo(string path);
	void CreateAddInfoUI();
	void ClearAllFBXObject();

	void ClearAllFBXUI();

	void CreateBaseTextureSelectUI();
	void CreateObjectPositioningUI();
	//����
	vector<LoadFileStruct> m_LoadFileStruct;
	//fbx object
	CTestObject* m_pFBXObject{ nullptr };
	
	void ObjectPositioning();
//	void SetPositioningObject(CGameObject* pObject) { m_pPositioningObject = pObject; };
private:
	int m_MeshCnt{ 0 };
	//framework
	CDirectXFramework* m_pFrameWork{ nullptr };

	//��¥ ī�޶�� framework�� �ִ�.
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

	//��� ���� ������ ��ü�� ���ͷ� �̸� ����� �д�.
	//�� ���� ��� ��ü�� ��ư���ν� �����Ѵ�.
	vector<CGameObject*> m_vpObjectList;
public:
	CSceneMain(CDirectXFramework* pFrameWork);
	~CSceneMain();
};