
#include "stdafx.h"
#include "Renderer.h"

bool CRenderer::Begin() {
	D3D11_DEPTH_STENCIL_DESC descDepth;
	descDepth.DepthEnable = TRUE;
	descDepth.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	descDepth.DepthFunc = D3D11_COMPARISON_LESS;
	descDepth.StencilEnable = TRUE;
	descDepth.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	descDepth.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
	const D3D11_DEPTH_STENCILOP_DESC stencilMarkOp = { D3D11_STENCIL_OP_REPLACE, D3D11_STENCIL_OP_REPLACE, D3D11_STENCIL_OP_REPLACE, D3D11_COMPARISON_ALWAYS };
	descDepth.FrontFace = stencilMarkOp;
	descDepth.BackFace = stencilMarkOp;
	GLOBALVALUEMGR->GetDevice()->CreateDepthStencilState(&descDepth, &m_pd3dDepthStencilState);
	if (false == CreateSwapChain()) return false;

	//layer
	m_pObjectRenderer = new CObjectRenderer();
	m_pObjectRenderer->Begin();
	m_pAORenderer = new CAORenderer();
	m_pAORenderer->Begin();

	m_pLightRenderer = new CLightRenderer();
	m_pLightRenderer->Begin();

	//post processing
	m_pBloomDownScale = new CBloomDownScale();
	m_pBloomDownScale->Begin();
	m_pBloom = new CBloom();
	m_pBloom->Begin();
	m_p16to1Blur = new CBlur();
	m_p16to1Blur->Begin();
	m_p4to1Blur = new CBlur();
	m_p4to1Blur->Begin();
	m_pPostProcessingFinalPass = new CPostProcessingFinalPass();
	m_pPostProcessingFinalPass->Begin();
	m_pSSLR = new CSSLR();
	m_pSSLR->Begin();
	//layer

	
	// render target과 depth-stencil buffer 생성/ deferred texture 생성
	if (!CreateRenderTargetView()) {
		MessageBox(GLOBALVALUEMGR->GethWnd(), TEXT("RenderTarget이나 Depth-Stencil 버퍼 생성이 실패했습니다. 프로그램을 종료합니다."), TEXT("프로그램 구동 실패"), MB_OK);
		return false;
	}
	return true;
}

bool CRenderer::End() {
	if (m_pdxgiSwapChain) m_pdxgiSwapChain->Release();
	if (m_pd3dRenderTargetView) m_pd3dRenderTargetView->Release();
	if (m_pd3ddsvReadOnlyDepthStencil) m_pd3ddsvReadOnlyDepthStencil->Release();
	
	ReleaseForwardRenderTargets();

	//layer
	if (m_pObjectRenderer) {
		m_pObjectRenderer->End();
		delete m_pObjectRenderer;
	}
	if (m_pAORenderer) {
		m_pAORenderer->End();
		delete m_pAORenderer;
	}
	if (m_pLightRenderer) {
		m_pLightRenderer->End();
		delete m_pLightRenderer;
	}
	if (m_pBloomDownScale) {
		m_pBloomDownScale->End();
		delete m_pBloomDownScale;
	}
	if (m_pBloom) {
		m_pBloom->End();
		delete m_pBloom;
	}
	if (m_p16to1Blur) {
		m_p16to1Blur->End();
		delete m_p16to1Blur;
	}
	if (m_p4to1Blur) {
		m_p4to1Blur->End();
		delete m_p4to1Blur;
	}
	if (m_pPostProcessingFinalPass) {
		m_pPostProcessingFinalPass->End();
		delete m_pPostProcessingFinalPass;
	}
	if (m_pSSLR) {
		m_pSSLR->End();
		delete m_pSSLR;
	}
	return true;
}
 
void CRenderer::Render(shared_ptr<CCamera> pCamera) {
	// Store the previous depth state
	ID3D11DepthStencilState* pPrevDepthState;
	UINT nPrevStencil;
	GLOBALVALUEMGR->GetDeviceContext()->OMGetDepthStencilState(&pPrevDepthState, &nPrevStencil);

	ClearDepthStencilView(m_pd3ddsvDepthStencil);
	SetForwardRenderTargets();//gbuff가 될 rtv/ dsv set
	GLOBALVALUEMGR->GetDeviceContext()->OMSetDepthStencilState(m_pd3dDepthStencilState, 1);

	//object render
	if(m_pTerrainContainer) m_pTerrainContainer->Render(pCamera);
	m_pObjectRenderer->Excute(pCamera);
	//debuge
	if (INPUTMGR->GetDebugMode()) {
		DEBUGER->DebugRender(pCamera);
	}
	SetRenderTargetViews(1, &m_pd3drtvLight, m_pd3ddsvReadOnlyDepthStencil);
	//m_pFrameWork->SetMainRenderTargetView();
	for (auto texture : m_vObjectLayerResultTexture) {
		texture->SetShaderState();
	}	
	ID3D11ShaderResourceView* pAmbientOcclution = m_pAORenderer->Excute(pCamera, 20, 1000);
	pAmbientOcclution  = m_p4to1Blur->Excute(pAmbientOcclution);
	GLOBALVALUEMGR->GetDeviceContext()->PSSetShaderResources(4, 1, &pAmbientOcclution);
	//ambient occulution 과정

	m_pLightRenderer->Excute(pCamera);

	for (auto texture : m_vObjectLayerResultTexture) {
		texture->CleanShaderState();
	}
	
	//ClearDepthStencilView(m_pd3dDepthStencilView);

	//진짜 rtv set! rtv만 set하구 dsv는 set하지 않는다. 

	////sslr
	//D3D11_VIEWPORT oldvp;
	//UINT num = 1;
	//GLOBALVALUEMGR->GetDeviceContext()->RSGetViewports(&num, &oldvp);
	//ID3D11RasterizerState* pPrevRSState;
	//GLOBALVALUEMGR->GetDeviceContext()->RSGetState(&pPrevRSState);
	//
	//m_pSSLR->Excute(pCamera, m_pd3drtvLight, pAmbientOcclution, XMVectorSet(-1,-1,-1,0), XMFLOAT3(0.1f, 0.1f, 0.1f));
	//
	//// Restore the states
	//GLOBALVALUEMGR->GetDeviceContext()->RSSetViewports(num, &oldvp);
	//GLOBALVALUEMGR->GetDeviceContext()->RSSetState(pPrevRSState);
	//if(pPrevRSState)pPrevRSState->Release();
	//SSLR

	SetMainRenderTargetView();
	m_vLightLayerResultTexture[0]->SetShaderState();
	PostProcessing(pCamera);


	for (auto texture : m_vLightLayerResultTexture) {
		texture->CleanShaderState();
	}
	
	if (INPUTMGR->GetDebugMode()) {
		ID3D11Buffer* pGBufferUnpackingBuffer = pCamera->GetGBufferUnpackingBuffer();
		GLOBALVALUEMGR->GetDeviceContext()->PSSetConstantBuffers(PS_UNPACKING_SLOT, 1, &pGBufferUnpackingBuffer);

		//if(testBotton){
		//DEBUGER->AddTexture(XMFLOAT2(100, 100), XMFLOAT2(250, 250), m_pd3dsrvColorSpecInt);
		//DEBUGER->AddTexture(XMFLOAT2(100, 250), XMFLOAT2(250, 400), m_pd3dsrvNormal);
		//DEBUGER->AddTexture(XMFLOAT2(100, 400), XMFLOAT2(250, 550), m_pd3dsrvLight);
		//DEBUGER->AddTexture(XMFLOAT2(100, 100), XMFLOAT2(500, 500), m_pd3dsrvDepthStencil);
		//debug
		//DEBUGER->AddTexture(XMFLOAT2(250, 100), XMFLOAT2(500, 250), pAmbientOcclution);
		//DEBUGER->AddTexture(XMFLOAT2(0, 0), XMFLOAT2(1000, 500), pAmbientOcclution);

		//이건 꼭 여기서 해줘야함.

		DEBUGER->RenderTexture();
		DEBUGER->RenderText();
		DEBUGER->ClearDebuger();
	}
	else {
		DEBUGER->ClearDebuger();
	}

	//	//ui
	TWBARMGR->Render();

	//present
	m_pdxgiSwapChain->Present(0, 0);

	// Restore the previous depth state
	GLOBALVALUEMGR->GetDeviceContext()->OMSetDepthStencilState(pPrevDepthState, nPrevStencil);
	if (pPrevDepthState) pPrevDepthState->Release();
}
void CRenderer::Update(float fTimeElapsed) {
	m_pBloomDownScale->SetAdaptation(fTimeElapsed);
}
void CRenderer::PostProcessing(shared_ptr<CCamera> pCamera) {
	m_pBloomDownScale->Excute(pCamera, 2.0f);
	ID3D11ShaderResourceView* pBloomImage = m_pBloom->Excute(pCamera);
	pBloomImage = m_p16to1Blur->Excute(pBloomImage);

	GLOBALVALUEMGR->GetDeviceContext()->PSSetShaderResources(2, 1, &pBloomImage);

	//rtv에 풀스크린 드로우 
	float fMiddleGrey = 0.0025f;
	float fWhite = 1.5f;
	float fBloomScale = 0.1f;
	m_pPostProcessingFinalPass->Excute(pCamera, fMiddleGrey, fWhite, fBloomScale);

	//적응을 하기위한 이전avgLum과 지금 계산한 avgLum을 교환
	m_pBloomDownScale->SwapAdaptationBuff();
}
void CRenderer::ClearDepthStencilView(ID3D11DepthStencilView* pDepthStencilView) {
	GLOBALVALUEMGR->GetDeviceContext()->ClearDepthStencilView(pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}
void CRenderer::SetForwardRenderTargets() {
	ID3D11RenderTargetView *pd3dRTVs[RENDER_TARGET_NUMBER] = { m_pd3drtvColorSpecInt, m_pd3drtvNormal, m_pd3drtvSpecPow };
	//float fClearColor[4] = { xmf4Xolor.x, xmf4Xolor.y, xmf4Xolor.z, xmf4Xolor.w };
	float fClearColor[4] = { 0.f, 0.f, 0.f, 0.f };
	if (m_pd3drtvColorSpecInt) GLOBALVALUEMGR->GetDeviceContext()->ClearRenderTargetView(m_pd3drtvColorSpecInt, fClearColor);
	if (m_pd3drtvNormal) GLOBALVALUEMGR->GetDeviceContext()->ClearRenderTargetView(m_pd3drtvNormal, fClearColor);
	if (m_pd3drtvSpecPow) GLOBALVALUEMGR->GetDeviceContext()->ClearRenderTargetView(m_pd3drtvSpecPow, fClearColor);

	SetRenderTargetViews(RENDER_TARGET_NUMBER, pd3dRTVs, m_pd3ddsvDepthStencil);
}
void CRenderer::SetMainRenderTargetView() {
	GLOBALVALUEMGR->GetDeviceContext()->OMSetRenderTargets(1, &m_pd3dRenderTargetView, nullptr);
}
void CRenderer::SetRenderTargetViews(UINT nRenderTarget, ID3D11RenderTargetView** pd3dRTVs, ID3D11DepthStencilView* pd3ddsvDepthStencil) {
	GLOBALVALUEMGR->GetDeviceContext()->OMSetRenderTargets(nRenderTarget, pd3dRTVs, pd3ddsvDepthStencil);
}


bool CRenderer::CreateSwapChain()
{
	// Swap Chain Description 구조체
	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
#pragma region [DXGI_SWAP_CHAIN_DESC 초기화]
	// 구조체 비우기
	::ZeroMemory(&dxgiSwapChainDesc, sizeof(dxgiSwapChainDesc));
	// BufferCount : 후면 버퍼의 수를 지정
	dxgiSwapChainDesc.BufferCount = 2;

	// BufferDesc : 후면 버퍼의 디스플레이 형식을 지정
	{
		// Width : 버퍼의 가로 크기를 픽셀로 지정
		dxgiSwapChainDesc.BufferDesc.Width = GLOBALVALUEMGR->GetrcClient().right;
		// Height : 버퍼의 세로 크기를 픽셀로 지정
		dxgiSwapChainDesc.BufferDesc.Height = GLOBALVALUEMGR->GetrcClient().bottom;
		// Format : 후면 버퍼 픽셀 형식
		/// DirectX 11-1(Chap 01)-Device, p.49 참조
		dxgiSwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
		// RefreshRate : 화면 갱신 비율을 Hz 단위로 지정
		{
			// Denominator : 분모
			dxgiSwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
			// Numerator : 분자
			dxgiSwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
		}
		// ScanlineOrdering : scan line 그리기 모드 지정(기본 0)
		//	DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED        (0) : 스캔 라인 순서를 지정하지 않음	
		//	DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE        (1) : 프로그레시브 스캔 라인 순서 지정
		//	DXGI_MODE_SCANLINE_ORDER_UPPER_FIELD_FIRST  (2) : 상위 필드로 이미지 생성
		//	DXGI_MODE_SCANLINE_ORDER_LOWER_FIELD_FIRST  (3) : 하위 필드로 이미지 생성
		dxgiSwapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	}

	// BufferUsage : 후면 버퍼에 대한 표면 사용 방식과 CPU의 접근 방법 지정
	//	DXGI_USAGE_SHADER_INPUT				: shader 의 입력으로 사용
	//	DXGI_USAGE_RENDER_TARGET_OUTPUT		: render target으로 사용
	//	DXGI_USAGE_BACK_BUFFER         		: back buffer 로 사용
	//	DXGI_USAGE_SHARED              		: 공유 목적
	//	DXGI_USAGE_READ_ONLY           		: 읽기 전용
	//	DXGI_USAGE_DISCARD_ON_PRESENT  		: DXGI 내부 전용 사용(사용자가 사용하지 않음)
	//	DXGI_USAGE_UNORDERED_ACCESS    		: 무순서화 접근
	dxgiSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	// OutputWindow : 출력 윈도우의 핸들을 지정(반드시 유효해야 함)
	dxgiSwapChainDesc.OutputWindow = GLOBALVALUEMGR->GethWnd();

	// SampleDesc : 다중 샘플링의 품질을 지정
	// CheckMultisampleQualityLevels 함수를 사용하여 다중 샘플링 가능 여부를 확인한 뒤에 값 변경
	{
		// Count : 픽셀 당 샘플 개수
		//	1  : 다중 샘플링을 하지 않음
		//	2~ : 해당 수만큼의 다중 샘플링
		dxgiSwapChainDesc.SampleDesc.Count = 1;
		// Quality : 품질 레벨
		// 0 : 다중 샘플링을 하지 않음
		dxgiSwapChainDesc.SampleDesc.Quality = 0;
	}

	// Windowed : 윈도우 모드 또는 전체 화면 모드 지정 ~ TRUE  | 창 모드 
	//												   ~ FALSE | 전체 화면
	dxgiSwapChainDesc.Windowed = TRUE;

	// Flags : Swap Chain 동작에 대한 선택 사항을 지정
	//	DXGI_SWAP_CHAIN_FLAG_NONPREROTATED		(1) : 전체 화면 모드에서 전면 버퍼의 내용을 화면으로 옮길 때 자동 회전하지 않음
	//	DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH	(2) : 응용 프로그램이 디스플레이 모드를 변경할 수 있음
	//	DXGI_SWAP_CHAIN_FLAG_GDI_COMPATIBLE		(4) : 응용 프로그램이 GDI 로 랜더링 할 수 있음. 후면 버퍼에 GetDC() 사용 가능
	dxgiSwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH/*2*/;

	// SwapEffect : Swaping을 처리하는 선택사항을 지정(기본 : 0)
	//	DXGI_SWAP_EFFECT_DISCARD		(0) : 버퍼 내용을 폐기
	//	DXGI_SWAP_EFFECT_SEQUENTIAL		(1) : 순차 복사
	// DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL	(2) : Flip 순차 복사
	dxgiSwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	// Result Handle 입니다. 장치가 성공적으로 생성도는지 검사합니다.
	HRESULT hResult = S_OK;
	IDXGIDevice *pdxgiDevice = NULL;
	if (FAILED(hResult = GLOBALVALUEMGR->GetDevice()->QueryInterface(__uuidof(IDXGIDevice), (LPVOID*)&pdxgiDevice)))
	{
		MessageBox(GLOBALVALUEMGR->GethWnd(), TEXT("DXGI Device 객체를 반환받지 못했습니다. 프로그램을 종료합니다."), TEXT("프로그램 구동 실패"), MB_OK);
		return(false);
	}
	// DXGI Factory 인스턴스를 DXGIFactory에서 받습니다.
	//IDXGIFactory *pdxgiFactory = NULL;
	IDXGIFactory *pdxgiFactory = NULL;

	UINT udxgiFlag = 0;
#ifdef _DEBUG
	udxgiFlag |= DXGI_CREATE_FACTORY_DEBUG;
#endif
	//if (FAILED(hResult = CreateDXGIFactory3(udxgiFlag, __uuidof(IDXGIFactory3), (LPVOID*)&pdxgiFactory)))
	if (FAILED(hResult = CreateDXGIFactory(/*udxgiFlag,*/ __uuidof(IDXGIFactory), (LPVOID*)&pdxgiFactory)))
	{
		MessageBox(GLOBALVALUEMGR->GethWnd(), TEXT("DXGIFactory에서의 객체 생성이 실패했습니다. 프로그램을 종료합니다."), TEXT("프로그램 구동 실패"), MB_OK);
		return(false);
	}
	//  SwapChain 을 생성합니다
	if (FAILED(hResult = pdxgiFactory->CreateSwapChain(pdxgiDevice, &dxgiSwapChainDesc, &m_pdxgiSwapChain)))
	{
		MessageBox(GLOBALVALUEMGR->GethWnd(), TEXT("SwapChain 인스턴스 생성이 실패했습니다. 프로그램을 종료합니다."), TEXT("프로그램 구동 실패"), MB_OK);
		return(false);
	}

	//	// Direct2D : Direct2D 인스턴스를 생성합니다.
	//	if (!CreateD2D1Device(pdxgiDevice))
	//	{
	//		MessageBox(m_hWnd, TEXT("Direct2D 인스턴스 생성이 실패했습니다. 프로그램을 종료합니다."), TEXT("프로그램 구동 실패"), MB_OK);
	//		return(false);
	//	}

	// 할당받은 COM 객체를 반환합니다.
	if (pdxgiDevice) pdxgiDevice->Release();
	if (pdxgiFactory) pdxgiFactory->Release();
	return true;
}

bool CRenderer::CreateRenderTargetView() {

	HRESULT hResult = S_OK;
	ID3D11Texture2D *pd3dBackBuffer{ nullptr };
	if (FAILED(hResult = m_pdxgiSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID *)&pd3dBackBuffer))) return(false);
	if (FAILED(hResult = GLOBALVALUEMGR->GetDevice()->CreateRenderTargetView(pd3dBackBuffer, NULL, &m_pd3dRenderTargetView))) return(false);
	if (pd3dBackBuffer) pd3dBackBuffer->Release();
	{
		//----------------------------------------Resource Desc-----------------------------------------//
		D3D11_SHADER_RESOURCE_VIEW_DESC d3dSRVDesc;
		::ZeroMemory(&d3dSRVDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
		d3dSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		d3dSRVDesc.Texture2D.MipLevels = 1;
		//d3dSRVDesc.Format = DXGI_FORMAT_R32_FLOAT;
		//----------------------------------------Resource Desc-----------------------------------------//
		//----------------------------------------TextUre Desc-----------------------------------------//
		D3D11_TEXTURE2D_DESC d3dTexture2DDesc;
		::ZeroMemory(&d3dTexture2DDesc, sizeof(D3D11_TEXTURE2D_DESC));
		d3dTexture2DDesc.Width = GLOBALVALUEMGR->GetrcClient().right;
		d3dTexture2DDesc.Height = GLOBALVALUEMGR->GetrcClient().bottom;
		d3dTexture2DDesc.MipLevels = 1;
		d3dTexture2DDesc.ArraySize = 1;
		d3dTexture2DDesc.SampleDesc.Count = 1;
		d3dTexture2DDesc.SampleDesc.Quality = 0;
		d3dTexture2DDesc.Usage = D3D11_USAGE_DEFAULT;
		d3dTexture2DDesc.CPUAccessFlags = 0;
		d3dTexture2DDesc.MiscFlags = 0;
		d3dTexture2DDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		//d3dTexture2DDesc.Format = DXGI_FORMAT_R32_TYPELESS;
		//----------------------------------------TextUre Desc-----------------------------------------//
		//----------------------------------------Render Desc-----------------------------------------//
		D3D11_RENDER_TARGET_VIEW_DESC d3dRTVDesc;
		::ZeroMemory(&d3dRTVDesc, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));
		d3dRTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		d3dRTVDesc.Texture2D.MipSlice = 0;
		//----------------------------------------TextUre Desc-----------------------------------------//

		// Create the depth stencil view 
		D3D11_DEPTH_STENCIL_VIEW_DESC d3dDepthStencilViewDesc;
		ZeroMemory(&d3dDepthStencilViewDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
		d3dDepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		d3dDepthStencilViewDesc.Texture2D.MipSlice = 0;

		ReleaseForwardRenderTargets();

		//--------------------------------------Scene0 DSV Create-----------------------------------------//
		d3dTexture2DDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
		d3dDepthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		d3dSRVDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		d3dTexture2DDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
		GLOBALVALUEMGR->GetDevice()->CreateTexture2D(&d3dTexture2DDesc, nullptr, &m_pd3dtxtDepthStencil);
		GLOBALVALUEMGR->GetDevice()->CreateDepthStencilView(m_pd3dtxtDepthStencil, &d3dDepthStencilViewDesc, &m_pd3ddsvDepthStencil);
		GLOBALVALUEMGR->GetDevice()->CreateShaderResourceView(m_pd3dtxtDepthStencil, &d3dSRVDesc, &m_pd3dsrvDepthStencil);
		//real depth stencil
		d3dDepthStencilViewDesc.Flags = D3D11_DSV_READ_ONLY_DEPTH | D3D11_DSV_READ_ONLY_STENCIL;
		if (FAILED(hResult = GLOBALVALUEMGR->GetDevice()->CreateDepthStencilView(m_pd3dtxtDepthStencil, &d3dDepthStencilViewDesc, &m_pd3ddsvReadOnlyDepthStencil))) return(false);
		//--------------------------------------Scene0 DSV Create-----------------------------------------//
		
		d3dTexture2DDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		d3dTexture2DDesc.Format = d3dSRVDesc.Format = d3dRTVDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;

		//--------------------------------------Scene1 RTV Create-----------------------------------------//
		GLOBALVALUEMGR->GetDevice()->CreateTexture2D(&d3dTexture2DDesc, nullptr, &m_pd3dtxtColorSpecInt);
		GLOBALVALUEMGR->GetDevice()->CreateShaderResourceView(m_pd3dtxtColorSpecInt, &d3dSRVDesc, &m_pd3dsrvColorSpecInt);
		GLOBALVALUEMGR->GetDevice()->CreateRenderTargetView(m_pd3dtxtColorSpecInt, &d3dRTVDesc, &m_pd3drtvColorSpecInt);
		//--------------------------------------Scene1 RTV Create-----------------------------------------//

		//--------------------------------------Scene2 RTV Create-----------------------------------------//
		GLOBALVALUEMGR->GetDevice()->CreateTexture2D(&d3dTexture2DDesc, nullptr, &m_pd3dtxtNormal);
		GLOBALVALUEMGR->GetDevice()->CreateRenderTargetView(m_pd3dtxtNormal, &d3dRTVDesc, &m_pd3drtvNormal);
		GLOBALVALUEMGR->GetDevice()->CreateShaderResourceView(m_pd3dtxtNormal, &d3dSRVDesc, &m_pd3dsrvNormal);
		//--------------------------------------Scene2 RTV Create-----------------------------------------//

		//--------------------------------------Scene3 RTV Create-----------------------------------------//
		GLOBALVALUEMGR->GetDevice()->CreateTexture2D(&d3dTexture2DDesc, nullptr, &m_pd3dtxtSpecPow);
		GLOBALVALUEMGR->GetDevice()->CreateRenderTargetView(m_pd3dtxtSpecPow, &d3dRTVDesc, &m_pd3drtvSpecPow);
		GLOBALVALUEMGR->GetDevice()->CreateShaderResourceView(m_pd3dtxtSpecPow, &d3dSRVDesc, &m_pd3dsrvSpecPow);
		//--------------------------------------Scene3 RTV Create-----------------------------------------//

		//자기 texture set -> sampler set위함
		//---------------------make texture---------------------
		//texture set to light rendercontainer
		//make sampler
		shared_ptr<CSampler> pSampler = RESOURCEMGR->GetSampler("DEFAULT");

		ID3D11ShaderResourceView *pd3dSRV = { m_pd3dsrvDepthStencil};
		UINT Slot = { 0 };
		UINT BindFlag = { BIND_PS | BIND_CS };
		shared_ptr<CTexture> pTexture = CTexture::CreateTexture(pd3dSRV, pSampler, Slot, BindFlag);
		m_vObjectLayerResultTexture.push_back(pTexture);

		pd3dSRV = { m_pd3dsrvColorSpecInt };
		Slot = { 1 };
		BindFlag = { BIND_PS | BIND_CS };
		pTexture = CTexture::CreateTexture(pd3dSRV, pSampler, Slot, BindFlag);
		m_vObjectLayerResultTexture.push_back(pTexture);

		pd3dSRV = { m_pd3dsrvNormal };
		Slot = { 2 };
		BindFlag = { BIND_PS | BIND_CS };
		pTexture = CTexture::CreateTexture(pd3dSRV, pSampler, Slot, BindFlag);
		m_vObjectLayerResultTexture.push_back(pTexture);
		
		pd3dSRV = { m_pd3dsrvSpecPow };
		Slot = { 3 };
		BindFlag = { BIND_PS | BIND_CS };
		pTexture = CTexture::CreateTexture(pd3dSRV, pSampler, Slot, BindFlag);
		m_vObjectLayerResultTexture.push_back(pTexture);
		//---------------------make texture---------------------


		//light texture제작
		GLOBALVALUEMGR->GetDevice()->CreateTexture2D(&d3dTexture2DDesc, nullptr, &m_pd3dtxtLight);
		GLOBALVALUEMGR->GetDevice()->CreateRenderTargetView(m_pd3dtxtLight, &d3dRTVDesc, &m_pd3drtvLight);
		GLOBALVALUEMGR->GetDevice()->CreateShaderResourceView(m_pd3dtxtLight, &d3dSRVDesc, &m_pd3dsrvLight);

		//make texture
		UINT LightTexSlot = { 0 };
		UINT LightTexBindFlag = { BIND_PS | BIND_CS };
		//make sampler
		shared_ptr<CSampler> pLightTexSampler;
		UINT LightTexSamplerBindFlag = { BIND_PS | BIND_CS };
		UINT LightTexSamplerSlot = { 0 };
		pLightTexSampler = CSampler::CreateSampler(LightTexSamplerSlot, LightTexSamplerBindFlag);
		pTexture = CTexture::CreateTexture(m_pd3dsrvLight, pLightTexSampler, LightTexSlot, LightTexBindFlag);
		m_vLightLayerResultTexture.push_back(pTexture);
		//light texture제작
	}
	m_pAORenderer->ResizeBuffer();
	m_pBloomDownScale->ResizeBuffer();
	m_pBloom->ResizeBuffer();
	m_p16to1Blur->ResizeBuffer(4);
	m_p4to1Blur->ResizeBuffer(2);
	m_pSSLR->ResizeBuffer();

	return true;
}

void CRenderer::ReleaseForwardRenderTargets() {
	//texture end
	m_vObjectLayerResultTexture.clear();
	m_vLightLayerResultTexture.clear();

	if (m_pd3dtxtColorSpecInt) m_pd3dtxtColorSpecInt->Release();//0
	m_pd3dtxtColorSpecInt = nullptr;

	if (m_pd3dtxtNormal) m_pd3dtxtNormal->Release();//1
	m_pd3dtxtNormal = nullptr;

	if (m_pd3dtxtSpecPow) m_pd3dtxtSpecPow->Release();//2
	m_pd3dtxtSpecPow = nullptr;

	if (m_pd3dtxtDepthStencil) m_pd3dtxtDepthStencil->Release();
	m_pd3dtxtDepthStencil = nullptr;

	if (m_pd3dsrvColorSpecInt) m_pd3dsrvColorSpecInt->Release();//0
	m_pd3dsrvColorSpecInt = nullptr;

	if (m_pd3dsrvNormal) m_pd3dsrvNormal->Release();//1
	m_pd3dsrvNormal = nullptr;

	if (m_pd3dsrvSpecPow) m_pd3dsrvSpecPow->Release();//2
	m_pd3dsrvSpecPow = nullptr;

	if (m_pd3dsrvDepthStencil) m_pd3dsrvDepthStencil->Release();
	m_pd3dsrvDepthStencil = nullptr;

	if (m_pd3drtvColorSpecInt) m_pd3drtvColorSpecInt->Release();//0
	m_pd3drtvColorSpecInt = nullptr;

	if (m_pd3drtvNormal) m_pd3drtvNormal->Release();//1
	m_pd3drtvNormal = nullptr;

	if (m_pd3drtvSpecPow) m_pd3drtvSpecPow->Release();//2
	m_pd3drtvSpecPow = nullptr;

	if (m_pd3ddsvDepthStencil) m_pd3ddsvDepthStencil->Release();
	m_pd3ddsvDepthStencil = nullptr;

	if (m_pd3drtvLight) m_pd3drtvLight->Release();
	m_pd3drtvLight = nullptr;


}


bool CRenderer::ResizeBuffer() {
	if (m_pd3dRenderTargetView) m_pd3dRenderTargetView->Release();
	if (m_pd3ddsvReadOnlyDepthStencil) m_pd3ddsvReadOnlyDepthStencil->Release();

	if (FAILED(m_pdxgiSwapChain->ResizeBuffers(1, 0, 0, DXGI_FORMAT_R16G16B16A16_FLOAT, 0)))
		MessageBox(nullptr, TEXT(""), TEXT(""), MB_OK);

	//resize rtv size, deferred texture size
	CreateRenderTargetView();

	return true;
}

CRenderer::CRenderer() :CSingleTonBase<CRenderer>("rendereringleton") {

}

CRenderer::~CRenderer() {

}
