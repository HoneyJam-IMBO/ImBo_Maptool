#include "stdafx.h"
#include "ObjectRenderer.h"

bool CObjectRenderer::Begin(){
	for (int i = 0; i < object_id::OBJECT_END; ++i) {
		object_id id = (object_id)i;
		m_mRenderContainer.insert(pairRenderContainer(id, RCSELLER->GetRenderContainer(id)));
	}

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
	m_mRenderContainer.clear();
	if (m_pd3dDepthStencilState)m_pd3dDepthStencilState->Release();
	if (m_pd3dTempDepthStencilState)m_pd3dTempDepthStencilState->Release();

	return true;
}

void CObjectRenderer::SetShaderState(){
	
}

void CObjectRenderer::CleanShaderState(){
	object_id id = object_id::OBJECT_END;
	//scene의 모든 Part의 rendercontainer안에 part list Clear!
	for (int i = 0; i < object_id::OBJECT_END; ++i) {
		id = (object_id)i;
		m_mRenderContainer[id]->ClearObjectList();
	}
}

void CObjectRenderer::UpdateShaderState(){

}

void CObjectRenderer::Excute(shared_ptr<CCamera> pCamera){
	object_id id = object_id::OBJECT_END;
	//scene의 모든 Part의 rendercontainer안에 part list Render!

	GLOBALVALUEMGR->GetDeviceContext()->OMGetDepthStencilState(&m_pd3dTempDepthStencilState, &m_TempStencil);
	GLOBALVALUEMGR->GetDeviceContext()->OMSetDepthStencilState(m_pd3dDepthStencilState, 0);
	m_mRenderContainer[object_id::OBJECT_SKYBOX]->Render(pCamera);
	GLOBALVALUEMGR->GetDeviceContext()->OMSetDepthStencilState(m_pd3dTempDepthStencilState, m_TempStencil);

	m_mRenderContainer[object_id::OBJECT_TERRAIN]->Render(pCamera);

	for (int i = 3; i < object_id::OBJECT_END; ++i) {
		id = (object_id)i;
		m_mRenderContainer[id]->Render(pCamera);
	}

	CleanShaderState();
}

CObjectRenderer::CObjectRenderer() : DXObject("objectrenderer"){

}

CObjectRenderer::~CObjectRenderer(){

}
