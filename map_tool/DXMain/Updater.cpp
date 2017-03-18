
#include "stdafx.h"
#include "Updater.h"

bool CUpdater::Begin() {
	//--------------------------------------space-------------------------------------
	//space
	UPDATER->SetSpaceContainer(CSpaceContainer::CreateSpaceContainer(512, 2));
	//terrain
	UPDATER->SetTerrainContainer(CTerrainContainer::CreateTerrainContainer(L"Temp", 256, 256, 0.5, UPDATER->GetSpaceContainer(), true));
	//skybox
	UPDATER->SetSkyBoxContainer(CSkyBoxContainer::CreateSkyBoxContainer(L"Temp", 0, UPDATER->GetSpaceContainer()));
	//-------------------------------------space-------------------------------------
	return true;
}

bool CUpdater::End() {
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
	//space
	if (m_pSpaceContainer) {
		m_pSpaceContainer->End();
		delete m_pSpaceContainer;
	}
	return true;
}

void CUpdater::PreUpdate(float fTimeElapsed){
	m_pCamera->Update(fTimeElapsed);
}

void CUpdater::Update(float fTimeElapsed) {
	//-----------------------------------space------------------------------
	m_pSpaceContainer->Animate(fTimeElapsed);
	m_pSpaceContainer->PrepareRender(m_pCamera);
	if (m_pTerrainContainer)m_pTerrainContainer->Update(m_pCamera);//이건 사실 할 필요가 없는것.
	if (m_pSkyBoxContainer)m_pSkyBoxContainer->Update(m_pCamera, fTimeElapsed);
	//-----------------------------------space------------------------------
}

void CUpdater::PhisicsUpdate(float fTimeElapsed){
}

void CUpdater::ChangeSceneContainers() {
	if (m_pSpaceContainer)m_pSpaceContainer->ChangeSpaceData();
	if (m_pTerrainContainer) m_pTerrainContainer->ChangeSpaceData();
	//if(m_pSkyBoxContainer)m_pSkyBoxContainer->ChangeSpaceData();
}

CUpdater::CUpdater() :CSingleTonBase<CUpdater>("updatersingleton") {

}

CUpdater::~CUpdater() {

}
