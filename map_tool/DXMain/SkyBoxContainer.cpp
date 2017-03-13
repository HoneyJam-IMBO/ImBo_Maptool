#include "stdafx.h"
#include "SkyBoxContainer.h"
#include "SpaceContainer.h"

void CSkyBoxContainer::Begin() {
	TWBARMGR->AddBoolBar("TOOL_MODE", "SceneObject", "SkyBoxOn/Off", &m_bActive);

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
}

bool CSkyBoxContainer::End() {
	if (m_pd3dDepthStencilState)m_pd3dDepthStencilState->Release();
	if (m_pd3dTempDepthStencilState)m_pd3dTempDepthStencilState->Release();
	//global object
	if (m_pSkyBox) {
		m_pSkyBox->End();
		delete m_pSkyBox;
	}
	return true;
}
void CSkyBoxContainer::Update(shared_ptr<CCamera> pCamera, float fTimeElapsed) {
	if (m_bActive) {
		m_pSkyBox->SetCamera(pCamera);
		//skybox camera 동기화
		m_pSkyBox->Animate(fTimeElapsed);
		//sky box 등록
		m_pSkyBox->RegistToContainer();
		//registe to renderer
		//RENDERER->SetSkyBoxContainer(this);
		return;
	}
	//RENDERER->SetSkyBoxContainer(nullptr);
}
CSkyBoxContainer * CSkyBoxContainer::CreateSkyBoxContainer(LPCTSTR pSkyBoxName, UINT textureIndex, CSpaceContainer * pSpaceContainer){
	CSkyBoxContainer* pSkyBoxContainer = new CSkyBoxContainer();
	pSkyBoxContainer->SetSkyBoxName(pSkyBoxName);
	pSkyBoxContainer->CreateSkyBoxTexture(textureIndex);
	pSkyBoxContainer->SetSpaceContainer(pSpaceContainer);
	pSkyBoxContainer->Begin();
	return pSkyBoxContainer;
}
void CSkyBoxContainer::CreateSkyBoxTexture(UINT index){
	if (m_ptxtSkyBox) m_ptxtSkyBox->End();
	m_ptxtSkyBox = nullptr;

	//skybox
	_TCHAR pstrTextureNames[128];
	_stprintf_s(pstrTextureNames, _T("../../Assets/SkyBox_%d.dds"), index);
	string name{ "" }; name.assign(m_wsSkyBoxName.begin(), m_wsSkyBoxName.end());
	m_ptxtSkyBox = CTexture::CreateTexture(pstrTextureNames, RESOURCEMGR->GetSampler("DEFAULT"), PS_SLOT_SKYBOX, BIND_PS);
}
void CSkyBoxContainer::Render(shared_ptr<CCamera> pCamera) {
	if (m_bActive) {
		//skybox
		GLOBALVALUEMGR->GetDeviceContext()->OMGetDepthStencilState(&m_pd3dTempDepthStencilState, &m_TempStencil);
		GLOBALVALUEMGR->GetDeviceContext()->OMSetDepthStencilState(m_pd3dDepthStencilState, 0);
		m_ptxtSkyBox->SetShaderState();
		m_pSkyboxContainer->Render(pCamera);
		m_ptxtSkyBox->CleanShaderState();
		GLOBALVALUEMGR->GetDeviceContext()->OMSetDepthStencilState(m_pd3dTempDepthStencilState, m_TempStencil);
		//skybox
	}
	//m_pSkyboxContainer->ClearObjectList();
}
CSkyBoxContainer::CSkyBoxContainer() : CObject("skyboxcontainer") {
}

CSkyBoxContainer::~CSkyBoxContainer()
{
}

