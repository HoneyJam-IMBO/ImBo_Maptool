#pragma once
#include "Object.h"
#include "Space.h"
#include "Camera.h"
#include "DirectionalLight.h"
//#include "SceneMain.h"
//#include "RenderContainerSeller.h"

class CScene;

class CSpaceContainer : public CObject {
public:
	void Begin();
	bool End();

	//space controller
	void Animate(float fTimeElapsed);
	void PrepareRender(shared_ptr<CCamera> pCamera);
	//animate�ϴٰ� �ش� ������ ��� ��ü �ӽ� �����
	void AddBlockObjectList(CGameObject* pObject);
	void AddObject(CGameObject* pObject);
	void RemoveObject(CGameObject* pObject);
	void RemoveObject(string name);
	void ClearBlockObjectList();
	int SearchSpace(XMVECTOR xmvPos);
	void AddSpace(UINT index, CSpace* pSpace);
	void ChangeSpaceData();
	//��� lay �浹 �˻�� ��ü �� ���� ���� ����� ��ü
	CGameObject* PickObject(XMVECTOR xmvWorldCameraStartPos, XMVECTOR xmvRayDir, float& distanse);
	//space controller

	//directional light
	void SetDirectionalLight(CDirectionalLight* pDirectionalLight) { m_pDirectionalLight = pDirectionalLight; }
	CDirectionalLight* GetDirectionalLight() { return m_pDirectionalLight; }
	//directional light

	static CSpaceContainer* CreateSpaceContainer(CScene* pScene, int size, int lv);
	void SetScene(CScene* pScene) { m_pScene = pScene; }
private:
	//directional light
	CDirectionalLight* m_pDirectionalLight{ nullptr };
	//directional light

	CScene* m_pScene{ nullptr };
	//space���� �����Ѵ�.
	CSpace** m_ppSpace{ nullptr };
	//���� Ʈ���� ��Ʈ��忡 �ش��ϴ� startSpace ����
	CSpace* m_pStartSpace{ nullptr };
	//block object list
	list<CGameObject*> m_lpBlockObject;

	//��ü ���� ũ��
	float m_size{ 0 };
	//������ ���� 
	float m_level{ 0 };
	//�� ���̵忡 �ִ� ������ ����
	int m_oneSideSpaceNum{ 0 };
	//���� �ϳ��� ũ��
	int m_oneSpaceSize{ 0 };
	//������ ����
	int m_nSpace{ 0 };

public:
	CSpaceContainer();
	~CSpaceContainer();

	//set get
	list<CGameObject*>& GetBlockObjectList() { return m_lpBlockObject; }
	CSpace* GetStartSpace() { return m_pStartSpace; }
	UINT GetSpaceNum() { return m_nSpace; }
	UINT GetSize() { return m_size; }
	UINT GetLevel() { return m_level; }
	UINT GetOneSideSpaceNum() { return m_oneSideSpaceNum; }
	UINT GetOneSpaceSize() { return m_oneSpaceSize; }

	int GetSpaceSize() { return m_size; }
	int GetSpaceLevel() { return m_level; }
	void SetSpaceSize(int size) { m_size = size; }
	void SetSpaceLevel(int lv) { m_level = lv; }
	//set get
};