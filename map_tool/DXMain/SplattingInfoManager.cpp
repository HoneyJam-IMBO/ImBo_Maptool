#include "stdafx.h"
#include "SplattingInfoManager.h"

void CSplattingInfoManager::Begin(){

}

bool CSplattingInfoManager::End(){
	if (m_pDetailTextures)m_pDetailTextures->End();
	if (m_pBlendInfoTextures)m_pBlendInfoTextures->End();
	return true;
}

void CSplattingInfoManager::SetShaderState(){
	if(m_pDetailTextures)m_pDetailTextures->SetShaderState();
	if(m_pBlendInfoTextures)m_pBlendInfoTextures->SetShaderState();
}

void CSplattingInfoManager::CleanShaderState(){
	if(m_pDetailTextures)m_pDetailTextures->CleanShaderState();
	if(m_pBlendInfoTextures)m_pBlendInfoTextures->CleanShaderState();
}

void CSplattingInfoManager::UpdateShaderState(){
	m_vSplattinfInfo[m_nCurIndex]->UpdateShaderState();

	if(m_pDetailTextures)m_pDetailTextures->End();
	m_pDetailTextures = nullptr;
	if(m_pBlendInfoTextures)m_pBlendInfoTextures->End();
	m_pBlendInfoTextures = nullptr;

	if (m_vSplattinfInfo.empty())return;
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
}

void CSplattingInfoManager::RemoveSplattingInfoByIndex(UINT index){
	//remove
	vector<CSplattingInfo*>::iterator iter;
	for (iter = m_vSplattinfInfo.begin(); iter != m_vSplattinfInfo.end(); ++iter) {
		if ((*iter)->GetIndex() == index) {
			break;
		}
	}
	if (iter == m_vSplattinfInfo.end()) return;
	m_vSplattinfInfo.erase(iter);

	//for 전부 돌면서 index 새로 set
	for (int i = 0; i < m_vSplattinfInfo.size(); ++i) {
		m_vSplattinfInfo[i]->SetIndex(i);
	}
}

void CSplattingInfoManager::CreateSplattingInfo(WCHAR * pBlendInfoTextureName, WCHAR * pDetailTextureName){
	if (m_vSplattinfInfo.size() > 10) return;
	CSplattingInfo* pSplattingInfo = CSplattingInfo::CreateSplattingInfo(this, pBlendInfoTextureName, pDetailTextureName);
	m_vSplattinfInfo.push_back(pSplattingInfo);
}

void CSplattingInfoManager::IncreaseBlendinginfo(CStempManager* pStempManager) {
	m_vSplattinfInfo[m_nCurIndex]->IncreaseBlendinginfo(pStempManager);
}

void CSplattingInfoManager::DecreaseBlendinginfo(CStempManager* pStempManager) {
	m_vSplattinfInfo[m_nCurIndex]->DecreaseBlendinginfo(pStempManager);
}

void CSplattingInfoManager::SetBlendinginfo(CStempManager* pStempManager) {
	m_vSplattinfInfo[m_nCurIndex]->SetBlendinginfo(pStempManager);
}


CSplattingInfoManager::CSplattingInfoManager()
{
}

CSplattingInfoManager::~CSplattingInfoManager()
{
}
