#include "stdafx.h"
#include "ObjectRenderer.h"

bool CObjectRenderer::Begin(){
	for (auto RenderContainer : RCSELLER->GetTagRenderContainer()[tag::TAG_DYNAMIC_OBJECT]) {
		m_mObjectRenderContainer[RenderContainer.first] = RenderContainer.second;
	}
	
	for (auto RenderContainer : RCSELLER->GetTagRenderContainer()[tag::TAG_STATIC_OBJECT]) {
		m_mObjectRenderContainer[RenderContainer.first] = RenderContainer.second;
	}

	m_pTerrainRenderContainer = RCSELLER->GetTagRenderContainer()[tag::TAG_TERRAIN]["terrain"];
	m_pSkyBoxRenderContainer = RCSELLER->GetTagRenderContainer()[tag::TAG_SPACE]["skybox"];

	//skybox depth stencil
	D3D11_DEPTH_STENCIL_DESC descDepth;
	descDepth.DepthEnable = FALSE;
	descDepth.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	descDepth.DepthFunc = D3D11_COMPARISON_LESS;
	descDepth.StencilEnable = TRUE;
	descDepth.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	descDepth.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
	const D3D11_DEPTH_STENCILOP_DESC noSkyStencilOp = { D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_COMPARISON_EQUAL };
	descDepth.FrontFace = noSkyStencilOp;
	descDepth.BackFace = noSkyStencilOp;
	GLOBALVALUEMGR->GetDevice()->CreateDepthStencilState(&descDepth, &m_pd3dDepthStencilState);

	return true;
}

bool CObjectRenderer::End(){
	m_mObjectRenderContainer.clear();
	m_pTerrainRenderContainer = nullptr;
	m_pSkyBoxRenderContainer = nullptr;
	if (m_pd3dDepthStencilState)m_pd3dDepthStencilState->Release();
	if (m_pd3dTempDepthStencilState)m_pd3dTempDepthStencilState->Release();

	return true;
}

void CObjectRenderer::SetShaderState(){
	
}

void CObjectRenderer::CleanShaderState(){
	for (auto pRenderContainer : m_mObjectRenderContainer) {
		pRenderContainer.second->ClearObjectList();
	}

	m_pTerrainRenderContainer->ClearObjectList();
	m_pSkyBoxRenderContainer->ClearObjectList();
}

void CObjectRenderer::UpdateShaderState(){

}

void CObjectRenderer::Excute(shared_ptr<CCamera> pCamera){
	//scene�� ��� Part�� rendercontainer�ȿ� part list Render!

	GLOBALVALUEMGR->GetDeviceContext()->OMGetDepthStencilState(&m_pd3dTempDepthStencilState, &m_TempStencil);
	GLOBALVALUEMGR->GetDeviceContext()->OMSetDepthStencilState(m_pd3dDepthStencilState, 0);
	m_pSkyBoxRenderContainer->Render(pCamera);
	GLOBALVALUEMGR->GetDeviceContext()->OMSetDepthStencilState(m_pd3dTempDepthStencilState, m_TempStencil);

	m_pTerrainRenderContainer->Render(pCamera);

	for (auto pRenderContainer : m_mObjectRenderContainer) {
		pRenderContainer.second->Render(pCamera);
	}
	
	CleanShaderState();
}

CObjectRenderer::CObjectRenderer() : DXObject("objectrenderer"){

}

CObjectRenderer::~CObjectRenderer(){

}
