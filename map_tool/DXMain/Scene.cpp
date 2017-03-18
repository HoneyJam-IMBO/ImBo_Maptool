#include "stdafx.h"
#include "Scene.h"
#include "DirectXFramework.h"

bool CScene::Begin(string path){


	return true;
}

bool CScene::End(){
	
	return true;
}

void CScene::Animate(float fTimeElapsed){

}

CScene * CScene::CreateScene(string name, CDirectXFramework* pFramework){
	CScene* pScene = new CScene(name);
	pScene->SetFrameWork(pFramework);
	CSpaceContainer* pSpaceContainer = CSpaceContainer::CreateSpaceContainer(512, 2);
	UPDATER->SetSpaceContainer(pSpaceContainer);
	UPDATER->SetTerrainContainer(CTerrainContainer::CreateTerrainContainer(L"Temp", 256, 256, 0.5, pSpaceContainer, true));
	UPDATER->SetSkyBoxContainer(CSkyBoxContainer::CreateSkyBoxContainer(L"Temp", 0, pSpaceContainer));

	//pScene->Begin();
	//framework���� begin�� ���ִϱ� ���⼭�� ������ �ʴ´�.

	return pScene;
	//return nullptr;
}