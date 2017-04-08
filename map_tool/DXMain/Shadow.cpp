#include "stdafx.h"
#include "Shadow.h"


CShadow::CShadow()
	: DXObject("shadow")
{
}


CShadow::~CShadow()
{
}

bool CShadow::Begin()
{
	ReleaseBuffer();
	//m_pShadowVPBuffer = CBuffer::CreateConstantBuffer(1, sizeof(XMFLOAT4X4), 0, BIND_VS | BIND_DS , NULL);
	m_pCamera = make_shared<CFreeCamera>();
	//------------------------------------------카메라 제작--------------------------------------
	m_pCamera->Begin();
	m_pCamera->GenerateProjectionMatrix(60.0f * XM_PI / 180.0f
		, float(GLOBALVALUEMGR->GetrcClient().right) / float(GLOBALVALUEMGR->GetrcClient().bottom)
		, 0.01f, 10000.0f);

	//viewprojection행렬 제작 / 광원
	XMVECTOR eye = { 500.0f, 500.0f, -500.0f, 0.0f };
	XMVECTOR at = { 500, 0.0f, 0, 0.0f };
	XMVECTOR up = { 0.0f, 1.0f, 0.0f, 0.0f };
	m_pCamera->SetLookAt(eye, at, up);

	D3D11_RASTERIZER_DESC descRasterizer;
	ZeroMemory(&descRasterizer, sizeof(D3D11_RASTERIZER_DESC));
	descRasterizer.FillMode = D3D11_FILL_SOLID;
	descRasterizer.CullMode = D3D11_CULL_NONE;
	GLOBALVALUEMGR->GetDevice()->CreateRasterizerState(&descRasterizer, &m_pd3dRSShader);

	return true;
}

bool CShadow::End()
{
	if (m_pCamera) {
		m_pCamera->End();
	}
	return true;
}

void CShadow::SetShaderState()
{
}

void CShadow::CleanShaderState()
{
}

void CShadow::UpdateShaderState()
{
}

ID3D11ShaderResourceView * CShadow::RenderShadowMap(shared_ptr<CCamera> pCamera, shared_ptr<CCamera>& pLightCam, CObjectRenderer* objRenderer)
{

	GLOBALVALUEMGR->GetDeviceContext()->ClearDepthStencilView(m_pd3ddsvShadow, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	ID3D11RasterizerState* pPrevRSState;
	GLOBALVALUEMGR->GetDeviceContext()->RSGetState(&pPrevRSState);
	GLOBALVALUEMGR->GetDeviceContext()->RSSetState(m_pd3dRSShader);


	m_pCamera->UpdateShaderState();
	m_pCamera->SetShaderState();

	float offset = 200.f;
	XMVECTOR eye = { 500.0f, 500.0f, -500.0f, 0.0f };
	XMVECTOR at = { 500, 0.0f, 0, 0.0f };
	XMVECTOR up = { 0.0f, 1.0f, 0.0f, 0.0f };
	m_pCamera->SetLookAt(eye, at, up);

	m_pCamera->SetViewport(0, 0, 4096, 4096, 0.0f, 1.0f);

	UPDATER->GetSpaceContainer()->PrepareRender(m_pCamera, RTAG_TERRAIN | RTAG_DYNAMIC_OBJECT | RTAG_STATIC_OBJECT);
	ID3D11RenderTargetView*	rtNULL = nullptr;
	GLOBALVALUEMGR->GetDeviceContext()->OMSetRenderTargets(1, &rtNULL, m_pd3ddsvShadow);
	//GLOBALVALUEMGR->GetDeviceContext()->PSSetShaderResources(m_TextureStartSlot, 1, &m_pd3dsrvShadow);
	objRenderer->ExcuteShadowRender(m_pCamera);


	pLightCam = m_pCamera;
	m_pCamera->SetViewport(0, 0, GLOBALVALUEMGR->GetrcClient().right, GLOBALVALUEMGR->GetrcClient().bottom, 0.0f, 1.0f);
	GLOBALVALUEMGR->GetDeviceContext()->RSSetState(pPrevRSState);
	return m_pd3dsrvShadow;
}

void CShadow::ResizeBuffer()
{
	ReleaseBuffer();

	D3D11_DEPTH_STENCIL_VIEW_DESC d3dDepthStencilViewDesc;
	ZeroMemory(&d3dDepthStencilViewDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	d3dDepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	d3dDepthStencilViewDesc.Texture2D.MipSlice = 0;
	d3dDepthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;

	D3D11_TEXTURE2D_DESC d3dTexture2DDesc;
	::ZeroMemory(&d3dTexture2DDesc, sizeof(D3D11_TEXTURE2D_DESC));
	d3dTexture2DDesc.Width = 4096;
	d3dTexture2DDesc.Height = 4096;
	d3dTexture2DDesc.MipLevels = 1;
	d3dTexture2DDesc.ArraySize = 1;
	d3dTexture2DDesc.SampleDesc.Count = 1;
	d3dTexture2DDesc.SampleDesc.Quality = 0;
	d3dTexture2DDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dTexture2DDesc.CPUAccessFlags = 0;
	d3dTexture2DDesc.MiscFlags = 0;
	d3dTexture2DDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
	d3dTexture2DDesc.Format = DXGI_FORMAT_R32_TYPELESS;

	D3D11_SHADER_RESOURCE_VIEW_DESC d3dSRVDesc;
	::ZeroMemory(&d3dSRVDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	d3dSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	d3dSRVDesc.Texture2D.MipLevels = 1;
	d3dSRVDesc.Format = DXGI_FORMAT_R32_FLOAT;
	//d3dTexture2DDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
	GLOBALVALUEMGR->GetDevice()->CreateTexture2D(&d3dTexture2DDesc, nullptr, &m_pd3dtxtShadow);
	GLOBALVALUEMGR->GetDevice()->CreateDepthStencilView(m_pd3dtxtShadow, &d3dDepthStencilViewDesc, &m_pd3ddsvShadow);
	GLOBALVALUEMGR->GetDevice()->CreateShaderResourceView(m_pd3dtxtShadow, &d3dSRVDesc, &m_pd3dsrvShadow);
}

void CShadow::ReleaseBuffer()
{
	if (m_pd3dtxtShadow) m_pd3dtxtShadow->Release();
	if (m_pd3dsrvShadow) m_pd3dsrvShadow->Release();
	if (m_pd3ddsvShadow) m_pd3ddsvShadow->Release();
}
