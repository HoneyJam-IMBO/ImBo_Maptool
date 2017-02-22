#include "stdafx.h"
#include "SplattingInfo.h"

void CSplattingInfo::Begin(){
	m_nIndex = m_pSplattingInfoManager->GetSplattingInfos().size();

	m_pBlendInfo = new Pixel24[BLEND_DATA_NUM];
	ZeroMemory(m_pBlendInfo, sizeof(Pixel24) * BLEND_DATA_NUM);
	//blend data init
	//�� ���� �ڽ��� index�� ���� �̸��� �޸��Ѵ�. �̴� �ε����� �ٲ�� 
	//���� �ٲ�� �ֵ��� �ڵ� ��ġ�� �س��ƾ� �ڴ�.
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
