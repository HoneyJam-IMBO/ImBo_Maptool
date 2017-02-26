#include "stdafx.h"
#include "SplattingInfoManager.h"

void TW_CALL SelectSplattingInfoButtonCallback(void * clientData) {
	CSplattingInfo* pData = (CSplattingInfo*)clientData;
	pData->GetSplattingManager()->SetCurIndex(pData->GetIndex());
	pData->CreateControllUI();
}

void CSplattingInfoManager::Begin(){
	m_pSplattingInfoBuffer = CBuffer::CreateConstantBuffer(1, sizeof(SPLATTING_INFO), 5, BIND_PS);
}

bool CSplattingInfoManager::End(){
	if (m_pDetailTextures)m_pDetailTextures->End();
	if (m_pBlendInfoTextures)m_pBlendInfoTextures->End();
	if (m_pSplattingInfoBuffer) m_pSplattingInfoBuffer->End();
	return true;
}

void CSplattingInfoManager::SetShaderState(){
	if(m_pDetailTextures)m_pDetailTextures->SetShaderState();
	if(m_pBlendInfoTextures)m_pBlendInfoTextures->SetShaderState();
	if (m_pSplattingInfoBuffer) m_pSplattingInfoBuffer->SetShaderState();
}

void CSplattingInfoManager::CleanShaderState(){
	if(m_pDetailTextures)m_pDetailTextures->CleanShaderState();
	if(m_pBlendInfoTextures)m_pBlendInfoTextures->CleanShaderState();
	if (m_pSplattingInfoBuffer) m_pSplattingInfoBuffer->CleanShaderState();
}

void CSplattingInfoManager::UpdateShaderState(){
	if (GLOBALVALUEMGR->GetToolMode() == TOOL_MODE_SPLATTING) {
		if (m_vSplattinfInfo.empty())return;

		m_vSplattinfInfo[m_nCurIndex]->UpdateShaderState();

		if (m_pDetailTextures)m_pDetailTextures->End();
		m_pDetailTextures = nullptr;
		if (m_pBlendInfoTextures)m_pBlendInfoTextures->End();
		m_pBlendInfoTextures = nullptr;

		//detail/ blending info texture제작
		_TCHAR ppstrBlendInfoTextureNames[MAX_SPLATTINGINFO_NUM][128];
		_TCHAR ppstrDetailTextureNames[MAX_SPLATTINGINFO_NUM][128];
		UINT index{ 0 };
		for (auto pSplattingInfo : m_vSplattinfInfo) {
			wsprintf(ppstrBlendInfoTextureNames[index], pSplattingInfo->GetBlendInfoTexturePath());
			wsprintf(ppstrDetailTextureNames[index++], pSplattingInfo->GetDetailTexturePath());
		}
		m_pDetailTextures = CTexture::CreateTexture(index, ppstrDetailTextureNames, RESOURCEMGR->GetSampler("DEFAULT"), 1, BIND_PS);;
		m_pBlendInfoTextures = CTexture::CreateTexture(index, ppstrBlendInfoTextureNames, RESOURCEMGR->GetSampler("DEFAULT"), 2, BIND_PS);

		SPLATTING_INFO* pData = (SPLATTING_INFO*)m_pSplattingInfoBuffer->Map();
		pData->nSplattingInfo = m_vSplattinfInfo.size();
		m_pSplattingInfoBuffer->Unmap();
	}
}

void CSplattingInfoManager::RemoveSplattingInfoByIndex(UINT index){
	if (m_vSplattinfInfo.empty())return;

	//remove
	vector<CSplattingInfo*>::iterator iter;
	for (iter = m_vSplattinfInfo.begin(); iter != m_vSplattinfInfo.end(); ++iter) {
		if ((*iter)->GetIndex() == index) {
			(*iter)->End();
			delete (*iter);
			break;
		}
	}
	if (iter == m_vSplattinfInfo.end()) return;
	m_vSplattinfInfo.erase(iter);

	//for 전부 돌면서 index 새로 set
	for (int i = 0; i < m_vSplattinfInfo.size(); ++i) {
		m_vSplattinfInfo[i]->SetIndex(i);
		m_vSplattinfInfo[i]->UpdateShaderState();
	}
	m_nCurIndex = 0;
	m_vSplattinfInfo[m_nCurIndex]->CreateControllUI();
	CreateSplattingListUI();
}

void CSplattingInfoManager::CreateSplattingInfo(WCHAR * pDetailTextureName){
	if (m_vSplattinfInfo.size() >= 10) return;
	CSplattingInfo* pSplattingInfo = CSplattingInfo::CreateSplattingInfo(this, pDetailTextureName);
	m_vSplattinfInfo.push_back(pSplattingInfo);
}

void CSplattingInfoManager::CreateSplattingListUI() {

	const char* barName{ "SPLATTING_INFO_LIST" };

	TWBARMGR->DeleteBar(barName);

	TWBARMGR->AddBar(barName);
	//set param
	TWBARMGR->SetBarSize(barName, 250, 300);
	TWBARMGR->SetBarPosition(barName, 750, 0);
	TWBARMGR->SetBarColor(barName, 255, 0, 255);
	TWBARMGR->SetBarContained(barName, true);
	TWBARMGR->SetBarMovable(barName, false);
	TWBARMGR->SetBarResizable(barName, false);
	//set param
	char menuName[64];
	for (auto pSplatting : m_vSplattinfInfo) {
		sprintf(menuName, "Splatting_%d", pSplatting->GetIndex());
		TWBARMGR->AddButtonCB(barName, "SPLATTING", menuName, SelectSplattingInfoButtonCallback, pSplatting);
	}
}


CSplattingInfoManager::CSplattingInfoManager()
{
}

CSplattingInfoManager::~CSplattingInfoManager()
{
}
