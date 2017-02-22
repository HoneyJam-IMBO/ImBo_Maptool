#include "stdafx.h"
#include "StempManager.h"

void CStempManager::Begin(){
	m_pPicposRenderInfoBuffer = CBuffer::CreateConstantBuffer(1, sizeof(TERRAIN_PICPOS_RENDER_INFO), 4, BIND_PS);
	m_pPicposRenderInfo = new TERRAIN_PICPOS_RENDER_INFO;
}

bool CStempManager::End(){
	if (m_pPicposRenderInfo) delete m_pPicposRenderInfo;
	if (m_pPicposRenderInfo) m_pPicposRenderInfoBuffer->End();
	
	for (auto pStemp : m_vStemp) {
		delete pStemp;
	}
	m_vStemp.clear();

	return true;
}

void CStempManager::SetShaderState(){
	m_pPicposRenderInfoBuffer->SetShaderState();
	m_vStemp[m_nCurStemp]->SetShaderState();
}

void CStempManager::CleanShaderState(){
	m_pPicposRenderInfoBuffer->CleanShaderState();
	m_vStemp[m_nCurStemp]->CleanShaderState();
}

void CStempManager::UpdateShaderState(){
	TERRAIN_PICPOS_RENDER_INFO* pPicposRenderInfo = (TERRAIN_PICPOS_RENDER_INFO*)m_pPicposRenderInfoBuffer->Map();
	pPicposRenderInfo->PickPos.x = (m_pPicposRenderInfo->PickPos.x);
	pPicposRenderInfo->PickPos.y = 1 - (m_pPicposRenderInfo->PickPos.y);
	pPicposRenderInfo->Extent = m_pPicposRenderInfo->Extent;
	pPicposRenderInfo->ToolMode = (UINT)GLOBALVALUEMGR->GetToolMode();
	m_pPicposRenderInfoBuffer->Unmap();
}

void CStempManager::IncreaseTerrain(Pixel24 * pHeightMapData){
	m_vStemp[m_nCurStemp]->IncreaseTerrain(pHeightMapData, m_pPicposRenderInfo->PickPos, m_pPicposRenderInfo->Extent);
}

void CStempManager::DecreaseTerrain(Pixel24* pHeightMapData){
	m_vStemp[m_nCurStemp]->DecreaseTerrain(pHeightMapData, m_pPicposRenderInfo->PickPos, m_pPicposRenderInfo->Extent);
}

void CStempManager::SetTerrain(Pixel24 * pHeightMapData){
	m_vStemp[m_nCurStemp]->SetTerrain(pHeightMapData, m_pPicposRenderInfo->PickPos, m_pPicposRenderInfo->Extent);
}

void CStempManager::SetCurStempIndex(int index){
	if (index >= m_vStemp.size()) return;
	m_nCurStemp = index;
}

void CStempManager::CreateStemp(wstring name){
	CStemp* pStemp = CStemp::CreateStemp(name, this);
	m_vStemp.push_back(pStemp);
}

CStempManager::CStempManager() : DXObject("stempmanager"){

}

CStempManager::~CStempManager(){

}
