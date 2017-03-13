#include "stdafx.h"
#include "SpaceContainer.h"
#include "SceneMain.h"

void TW_CALL SCAdaptationButtonCallback(void* clientData) {
	CSceneMain* pScene = (CSceneMain*)clientData;
	pScene->ChangeSceneContainers();
}

void CSpaceContainer::Begin(){
	TWBARMGR->AddMinMaxBarRW("TOOL_MODE", "SpaceControll", "SpaceSize", &m_size, 256.f, 4096.f, 256.f);
	TWBARMGR->AddMinMaxBarRW("TOOL_MODE", "SpaceControll", "SpaceLevel", &m_level, 0.f, 4.f, 1.f);
	TWBARMGR->AddButtonCB("TOOL_MODE", "SpaceControll", "Adaptation", SCAdaptationButtonCallback, m_pScene);

	//�� ���̵忡 �ִ� ������ ����
	m_oneSideSpaceNum = static_cast<int>(pow(2, m_level));
	//���� �ϳ��� ũ��
	m_oneSpaceSize = m_size / m_oneSideSpaceNum;
	//������ ���� ����Ʈ���ϱ� ����
	m_nSpace = static_cast<int>(pow(m_oneSideSpaceNum, 2));

	//�����Ǿ��� leaf spaces
	m_ppSpace = new CSpace*[m_nSpace];
	
	//���� �Ҵ�.
	m_pStartSpace = new CSpace();
	m_pStartSpace->Begin(this, m_size, m_level, XMVectorSet(0.f, 0.f, 0.f, 0.f));

	//directional light
	m_pDirectionalLight = new CDirectionalLight;
	m_pDirectionalLight->Begin(DIRECTIONAL_AMBIENT_LIGHT{
		XMFLOAT4(1.0f, -1.0f, 1.0f, 0.0f),XMFLOAT4(0.1f, 0.1f, 0.1f, 0.0f) , XMFLOAT4(1.5f, 1.5f, 1.5f, 1.f),//dir
		XMFLOAT4(0.1f, 0.1f, 0.1f, 1.f), XMFLOAT4(0.1f, 0.1f, 0.1f, 1.f), XMFLOAT4(0.1f, 0.1f, 0.1f, 1.f), XMFLOAT4(5.1f, 5.1f, 5.1f, 1.f)//ambient
	});
	m_pDirectionalLight->SetPosition(XMVectorSet(m_size / 2.f, m_size, m_size / 2.f, 0.f));
	m_pDirectionalLight->Rotate(30.f, 0.f, 0.f);
}

bool CSpaceContainer::End(){
	for (auto pObject : m_lpBlockObject) {
		pObject->End();
		delete pObject;
	}

	//all space end
	//+ delete space pointer
	m_pStartSpace->End();

	//directional light
	if (m_pDirectionalLight) {
		m_pDirectionalLight->End();
		delete m_pDirectionalLight;
	}
	//directional light
	delete m_ppSpace;

	return false;
}

void CSpaceContainer::Animate(float fTimeElapsed){
	//directional light ���
	m_pDirectionalLight->RegistToContainer();

	//all space animate
	m_pStartSpace->Animate(fTimeElapsed);

	//Render �׸��׸��� space��� �� �ʱ�ȭ
	for (int i = 0; i < m_nSpace; ++i) {
		m_ppSpace[i]->SetbRender(false);
	}

	//animate ���Ŀ� �и��� block object�� �����Ѵ�.
	if (false == m_lpBlockObject.empty()) {//block object list�� empty�� �ƴ϶��
		for (auto pObject : m_lpBlockObject) {//��ü �ٽ� ��ġ
			AddObject(pObject);
		}
		m_lpBlockObject.clear();
	}
}

void CSpaceContainer::PrepareRender(shared_ptr<CCamera> pCamera){
	//directional light
	//RENDERER->SetDirectionalLight(m_pDirectionalLight);
	m_pStartSpace->PrepareRender(pCamera);
}

void CSpaceContainer::AddBlockObjectList(CGameObject * pObject){
	m_lpBlockObject.emplace_back(pObject);
}

void CSpaceContainer::AddObject(CGameObject * pObject){
	//���� index ���ϱ�
	int current_index = SearchSpace(pObject->GetPosition());


	if (current_index < 0) {
		current_index = 0;
		pObject->SetPosition(XMVectorSet(0.f, 0.f, 0.f, 0.f));
	}

	//�ش� ������ ��ü �߰�
	m_ppSpace[current_index]->AddObject(pObject);
	//��ü�� ����index ���
	pObject->SetSpaceIndex(current_index);
}

void CSpaceContainer::RemoveObject(CGameObject * pObject){
	for (int i = 0; i < m_nSpace; ++i) {
		m_ppSpace[i]->RemoveObject(pObject);
	}
}

void CSpaceContainer::RemoveObject(string name){
	for (int i = 0; i < m_nSpace; ++i) {
		m_ppSpace[i]->RemoveObject(name);
	}
}

void CSpaceContainer::ClearBlockObjectList() {
	m_lpBlockObject.clear();
}

int CSpaceContainer::SearchSpace(XMVECTOR xmvPos){
	XMFLOAT3 xmf3Pos;
	XMStoreFloat3(&xmf3Pos, xmvPos);
	int x = static_cast<int>(xmf3Pos.x);
	//int y = xmf3Pos.y;
	int z = static_cast<int>(xmf3Pos.z);


	//�ڽ��� ��ġ�� �̿��� �ε����� ����
	int indexX = (int)x / m_oneSpaceSize;
	//int indexY = (int)y / m_oneSpaceSize;
	int indexZ = (int)z / m_oneSpaceSize;



	int index = indexX + (indexZ * m_oneSideSpaceNum);

	//���� ó�� ���� ��
	if (index < 0 || index >= m_nSpace) {
		return -1;
		//return 0;
	}
	if (x < 0 || z < 0 ||
		x > m_size || z > m_size) {
		return -1;
		//return 0;
	}
	//���� ó�� ���� ��


	return index;
}

void CSpaceContainer::AddSpace(UINT index, CSpace * pSpace){
	m_ppSpace[index] = pSpace;
}

void CSpaceContainer::ChangeSpaceData(){
	//directional light
	m_pDirectionalLight->SetPosition(XMVectorSet(m_size / 2.f, m_size, m_size / 2.f, 0.f));

	//1. space���� ��� ��ü �ӽ� ����
	vector<CGameObject*> m_vTempObjects;//�ӽ� ��ü ����
	for (int i = 0; i < m_nSpace; ++i) {//��� space�� ���ؼ�
		for (auto data : m_ppSpace[i]->GetmlpObject()) {//object map�� �����ͼ�
			for (auto pObject : data.second) {//�ش� vector ���� ��� object��
				m_vTempObjects.push_back(pObject);//�ӽ� ����
			}
		}
	}
	
	//2. space delete/ not END()
	delete m_pStartSpace;
	m_pStartSpace = nullptr;
	if (m_nSpace != 1) {
		for (int i = 0; i < m_nSpace; ++i) {
			delete m_ppSpace[i];
		}
	}
	delete m_ppSpace;
	//3. ���ο� space����
	m_oneSideSpaceNum = static_cast<int>(pow(2, m_level));
	m_oneSpaceSize = m_size / m_oneSideSpaceNum;
	m_nSpace = static_cast<int>(pow(m_oneSideSpaceNum, 2));
	m_ppSpace = new CSpace*[m_nSpace];
	m_pStartSpace = new CSpace();
	m_pStartSpace->Begin(this, m_size, m_level, XMVectorSet(0.f, 0.f, 0.f, 0.f));

	//4. �ӽð�ü �� ��ġ
	for (auto pObject : m_vTempObjects) {
		AddObject(pObject);
	}
}

CGameObject * CSpaceContainer::PickObject(XMVECTOR xmvWorldCameraStartPos, XMVECTOR xmvRayDir, float& distanse){
	float fHitDistance = FLT_MAX;
	float fNearHitDistance = FLT_MAX;
	CGameObject* pHitObj = nullptr;
	CGameObject* pNearObj = nullptr;

	//�ڽ��� ��� leaf space�� ���ؼ� �˻�
	for (int i = 0; i < m_nSpace; ++i) {
		if (m_ppSpace[i]->GetbRender()) {//���� �׸��� �׸��� space���
			pHitObj = m_ppSpace[i]->PickObject(xmvWorldCameraStartPos, xmvRayDir, fHitDistance);
			if (fNearHitDistance > fHitDistance) {
				fNearHitDistance = fHitDistance;
				pNearObj = pHitObj;
			}
		}
	}

	if (m_pDirectionalLight->CheckPickObject(xmvWorldCameraStartPos, xmvRayDir, fHitDistance)) {
		if (fNearHitDistance > fHitDistance) {
			pNearObj = m_pDirectionalLight;
		}
	}

	return pNearObj;
}

CSpaceContainer * CSpaceContainer::CreateSpaceContainer(CScene* pScene, int size, int lv){
	CSpaceContainer* pSpaceContainer = new CSpaceContainer();
	pSpaceContainer->SetSpaceSize(size);
	pSpaceContainer->SetSpaceLevel(lv);
	pSpaceContainer->SetScene(pScene);
	pSpaceContainer->Begin();
	return pSpaceContainer;
}

CSpaceContainer::CSpaceContainer() : CObject("spacecontainer"){

}

CSpaceContainer::~CSpaceContainer(){

}

