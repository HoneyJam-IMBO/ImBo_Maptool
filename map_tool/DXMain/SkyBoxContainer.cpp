#include "stdafx.h"
#include "SkyBoxContainer.h"
#include "SpaceContainer.h"

void CSkyBoxContainer::Begin() {
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
	m_pSkyboxContainer = RCSELLER->GetRenderContainer(object_id::OBJECT_SKYBOX);
	//height map data init
	
	//skybox
	m_pSkyBox = new CSkyBox();
	m_pSkyBox->Begin();	
	//skybox
	//directional light
	m_pDirectionalLight = new CDirectionalLight;
	m_pDirectionalLight->Begin(DIRECTIONAL_AMBIENT_LIGHT{
		XMFLOAT4(1.0f, -1.0f, 1.0f, 0.0f),XMFLOAT4(0.1f, 0.1f, 0.1f, 0.0f) , XMFLOAT4(1.5f, 1.5f, 1.5f, 1.f),//dir
		XMFLOAT4(0.1f, 0.1f, 0.1f, 1.f), XMFLOAT4(0.1f, 0.1f, 0.1f, 1.f), XMFLOAT4(0.1f, 0.1f, 0.1f, 1.f), XMFLOAT4(5.1f, 5.1f, 5.1f, 1.f)//ambient
	});
	m_pDirectionalLight->SetPosition(XMVectorSet(SPACE_SIZE / 2.f, SPACE_SIZE, SPACE_SIZE / 2.f, 0.f));
	m_pDirectionalLight->Rotate(30.f, 0.f, 0.f);
	RENDERER->SetDirectionalLight(m_pDirectionalLight);

}

bool CSkyBoxContainer::End() {
	if (m_pd3dDepthStencilState)m_pd3dDepthStencilState->Release();
	if (m_pd3dTempDepthStencilState)m_pd3dTempDepthStencilState->Release();
	//global object
	if (m_pSkyBox) {
		m_pSkyBox->End();
		delete m_pSkyBox;
	}
	if (m_pDirectionalLight) {
		m_pDirectionalLight->End();
		delete m_pDirectionalLight;
	}
	return true;
}
void CSkyBoxContainer::Update(shared_ptr<CCamera> pCamera, float fTimeElapsed) {
	m_pSkyBox->SetCamera(pCamera);
	//skybox camera 동기화
	m_pSkyBox->Animate(fTimeElapsed);
	//sky box 등록
	m_pSkyBox->RegistToContainer();

	//directional light 등록
	m_pDirectionalLight->RegistToContainer();
}
CSkyBoxContainer * CSkyBoxContainer::CreateSkyBoxContainer(){
	CSkyBoxContainer* pSkyBoxContainer = new CSkyBoxContainer;
	pSkyBoxContainer->Begin();
	return pSkyBoxContainer;
}
void CSkyBoxContainer::Render(shared_ptr<CCamera> pCamera) {
	//skybox
	GLOBALVALUEMGR->GetDeviceContext()->OMGetDepthStencilState(&m_pd3dTempDepthStencilState, &m_TempStencil);
	GLOBALVALUEMGR->GetDeviceContext()->OMSetDepthStencilState(m_pd3dDepthStencilState, 0);
	m_pSkyboxContainer->Render(pCamera);
	GLOBALVALUEMGR->GetDeviceContext()->OMSetDepthStencilState(m_pd3dTempDepthStencilState, m_TempStencil);
	//skybox
}
CSkyBoxContainer::CSkyBoxContainer() : CObject("skyboxcontainer") {
}

CSkyBoxContainer::~CSkyBoxContainer()
{
}

