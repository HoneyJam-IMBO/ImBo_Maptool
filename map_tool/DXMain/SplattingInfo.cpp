#include "stdafx.h"
#include "SplattingInfo.h"

void CSplattingInfo::Begin(){
	m_pBlendInfo = new Pixel24[BLEND_DATA_NUM];
	ZeroMemory(m_pBlendInfo, sizeof(Pixel24) * BLEND_DATA_NUM);
	//blend data init
	//이 때에 자신의 index에 따라 이름을 달리한다. 이는 인덱스가 바뀌면 
	//따라서 바뀔수 있도록 자동 장치를 해놓아야 겠다.
	//EXPORTER->MakeBitmap24(L"../TempBlendInfo1.bmp", m_pBlendInfo, BLEND_WIDTH, BLEND_HEIGHT);
	m_pBlendInfo = IMPORTER->ReadBitmap24(L"../slide.bmp");
	m_pBlendInfoTexture = CTexture::CreateTexture(L"../slide.bmp", RESOURCEMGR->GetSampler("DEFAULT"), 3, BIND_PS);
}

bool CSplattingInfo::End(){
	if(m_pBlendInfoTexture)m_pBlendInfoTexture->End();
	if(m_pBaseTexture)m_pBaseTexture->End();
	if(m_pDetailTexture)m_pDetailTexture->End();
	return true;
}

void CSplattingInfo::SetShaderState(){
	m_pBlendInfoTexture->SetShaderState();
	m_pBaseTexture->SetShaderState();
	m_pDetailTexture->SetShaderState();
}

void CSplattingInfo::CleanShaderState(){
	m_pBlendInfoTexture->CleanShaderState();
	m_pBaseTexture->CleanShaderState();
	m_pDetailTexture->CleanShaderState();
}

void CSplattingInfo::UpdateShaderState(){
	//m_pBlendInfoTexture->End();
	//
	////height map data init
	//EXPORTER->MakeBitmap24(L"../TempBlendInfo1.bmp", m_pBlendInfo, BLEND_WIDTH, BLEND_HEIGHT);
	//m_pBlendInfoTexture = CTexture::CreateTexture(L"../TempBlendInfo1.bmp", RESOURCEMGR->GetSampler("DEFAULT"), 1, BIND_DS);
}

CSplattingInfo * CSplattingInfo::CreateSplattingInfo(WCHAR * BaseTexture, WCHAR * DetailTexture){
	CSplattingInfo* pSplattingInfo = new CSplattingInfo;
	//pSplattingInfo->SetBlendInfoTexture(CTexture::CreateTexture(BlendTexture, RESOURCEMGR->GetSampler("DEFAULT")));
	pSplattingInfo->SetBaseTexture(CTexture::CreateTexture(BaseTexture, RESOURCEMGR->GetSampler("DEFAULT"), 0));
	pSplattingInfo->SetDetailTexture(CTexture::CreateTexture(DetailTexture, RESOURCEMGR->GetSampler("DEFAULT"),1));
	pSplattingInfo->Begin();

	return pSplattingInfo;
}

CSplattingInfo::CSplattingInfo() : DXObject("splattinginfo")
{
}

CSplattingInfo::~CSplattingInfo()
{
}
