#include "stdafx.h"
#include "SplattingInfo.h"

void CSplattingInfo::Begin(){
	m_nIndex = m_pSplattingInfoManager->GetSplattingInfos().size();

	m_pBlendInfo = new Pixel24[BLEND_DATA_NUM];
	ZeroMemory(m_pBlendInfo, sizeof(Pixel24) * BLEND_DATA_NUM);
	//blend data init
	//이 때에 자신의 index에 따라 이름을 달리한다. 이는 인덱스가 바뀌면 
	//따라서 바뀔수 있도록 자동 장치를 해놓아야 겠다.
	//EXPORTER->MakeBitmap24(L"../TempBlendInfo1.bmp", m_pBlendInfo, BLEND_WIDTH, BLEND_HEIGHT);
	m_pBlendInfo = IMPORTER->ReadBitmap24(m_pBlendInfoTexturePath);
	//m_pBlendInfoTexture = CTexture::CreateTexture(L"../slide.bmp", RESOURCEMGR->GetSampler("DEFAULT"), 2, BIND_PS);
}

bool CSplattingInfo::End(){
	
	return true;
}

void CSplattingInfo::SetShaderState(){
	
}

void CSplattingInfo::CleanShaderState(){
	
}

void CSplattingInfo::UpdateShaderState(){
	//height map data init
	EXPORTER->MakeBitmap24(m_pBlendInfoTexturePath, m_pBlendInfo, BLEND_WIDTH, BLEND_HEIGHT);
}

CSplattingInfo * CSplattingInfo::CreateSplattingInfo(CSplattingInfoManager* pSplattingManager, const WCHAR* pBlendInfoTextureName, const WCHAR * pDetailTextureName){
	CSplattingInfo* pSplattingInfo = new CSplattingInfo;

	pSplattingInfo->SetSplattingManager(pSplattingManager);
	pSplattingInfo->SetBlendInfoTextuePath(pBlendInfoTextureName);
	pSplattingInfo->SetDetailTextuePath(pDetailTextureName);
	
	pSplattingInfo->Begin();
	return pSplattingInfo;
}

void CSplattingInfo::IncreaseBlendinginfo(CStempManager* pStempManager){
	pStempManager->IncreaseTerrain(m_pBlendInfo);
}

void CSplattingInfo::DecreaseBlendinginfo(CStempManager* pStempManager){
	pStempManager->DecreaseTerrain(m_pBlendInfo);
}

void CSplattingInfo::SetBlendinginfo(CStempManager* pStempManager){
	pStempManager->SetTerrain(m_pBlendInfo);
}

CSplattingInfo::CSplattingInfo() : DXObject("splattinginfo")
{
}

CSplattingInfo::~CSplattingInfo()
{
}
