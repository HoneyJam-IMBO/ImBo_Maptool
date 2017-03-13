#include "stdafx.h"
#include "Scene.h"
#include "DirectXFramework.h"

bool CScene::End(){
	//space
	if (m_pSpaceContainer) {
		m_pSpaceContainer->End();
		delete m_pSpaceContainer;
	}
	//terrain
	if (m_pTerrainContainer) {
		m_pTerrainContainer->End();
		delete m_pTerrainContainer;
	}
	//skybox
	if (m_pSkyBoxContainer) {
		m_pSkyBoxContainer->End();
		delete m_pSkyBoxContainer;
	}
	return true;
}

void CScene::Animate(float fTimeElapsed){
	//-----------------------------------space------------------------------
	m_pSpaceContainer->Animate(fTimeElapsed);
	m_pSpaceContainer->PrepareRender(m_pCamera);
	if (m_pTerrainContainer)m_pTerrainContainer->Update(m_pCamera);
	if (m_pSkyBoxContainer)m_pSkyBoxContainer->Update(m_pCamera, fTimeElapsed);
	//-----------------------------------space------------------------------
}
void CScene::ChangeSceneContainers() {
	if (m_pSpaceContainer)m_pSpaceContainer->ChangeSpaceData();
	if (m_pTerrainContainer) m_pTerrainContainer->ChangeSpaceData();
	//if(m_pSkyBoxContainer)m_pSkyBoxContainer->ChangeSpaceData();
}

CScene * CScene::CreateScene(string name, CDirectXFramework* pFramework){
	CScene* pScene = new CScene(name);
	pScene->SetFrameWork(pFramework);
	CSpaceContainer* pSpaceContainer = CSpaceContainer::CreateSpaceContainer(pScene, 512, 2);
	pScene->SetSpaceContainer(pSpaceContainer);
	pScene->SetTerrainContainer(CTerrainContainer::CreateTerrainContainer(L"Temp", 256, 256, 0.5, pSpaceContainer, true));
	pScene->SetSkyBoxContainer(CSkyBoxContainer::CreateSkyBoxContainer(L"Temp", 0, pSpaceContainer));

	//pScene->Begin();
	//framework에서 begin을 해주니까 여기서는 해주지 않는다.

	return pScene;
	//return nullptr;
}