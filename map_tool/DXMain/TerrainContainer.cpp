#include "stdafx.h"
#include "TerrainContainer.h"
#include "SpaceContainer.h"

#define TEXTURE_SIZE 257
void TW_CALL SMSetButtonCallback(void * clientData) {
	CTerrainContainer* pData = (CTerrainContainer*)clientData;
	pData->SetStempMode(STEMP_MODE_SET);
}
void TW_CALL SMIndeButtonCallback(void * clientData) {
	CTerrainContainer* pData = (CTerrainContainer*)clientData;
	pData->SetStempMode(STEMP_MODE_INDE);
}

void CTerrainContainer::Begin(LPCTSTR pHeightmapFileName, int nWidth, int nLength, float fHeightScale, CSpaceContainer* pSpaceContainer) {
	
	TWBARMGR->AddButtonCB("TOOL_MODE", "STEMP_MODEVIEW", "SetStemp", SMSetButtonCallback, this);
	TWBARMGR->AddButtonCB("TOOL_MODE", "STEMP_MODEVIEW", "IndeStemp", SMIndeButtonCallback, this);

	m_pSplattingInfoManager = new CSplattingInfoManager();
	m_pSplattingInfoManager->Begin();
	//m_pSplattingInfoManager->CreateSplattingInfo(L"../slide.bmp", L"../../Assets/Detail_Texture_9.jpg");

	m_pStempManager = new CStempManager();
	//��� stemp����
	m_pStempManager->Begin();

	m_pSpaceContainer = pSpaceContainer;
	//global object ����
	//m_pGlobalTerrain = new CGlobalTerrain();
	////global object set, Update
	//m_pGlobalTerrain->Begin(fOneSpaceSize, fOneSideSpaceNum, fHeightScale);
	m_pGlobalTerrainBuffer = RESOURCEMGR->GetGlobalBuffer("TerrainGB");
	
	float fOneSpaceSize = (float)m_pSpaceContainer->GetOneSpaceSize();
	float fOneSideSpaceNum = (float)m_pSpaceContainer->GetOneSideSpaceNum();
	m_pGlobalTerrainData = new TERRAIN_GLOBAL_VALUE;
	m_pGlobalTerrainData->OneSideSpaceNum = fOneSideSpaceNum;
	m_pGlobalTerrainData->OneSideSpaceNumRcp = 1.0f / fOneSideSpaceNum;
	m_pGlobalTerrainData->OneSpaceSizeRcp = 1.0f / fOneSpaceSize;
	m_pGlobalTerrainData->HeightScale = fHeightScale;

	//texture ����
	m_pHeightData = new Pixel24[(nWidth) * (nLength)];
	ZeroMemory(m_pHeightData, sizeof(Pixel24) * (nWidth) * (nLength));
	//height map data init
	EXPORTER->MakeBitmap24(L"../TempHeightmap.bmp", m_pHeightData, nWidth, nLength);
	m_pHeightMapTexture = CTexture::CreateTexture(L"../TempHeightmap.bmp", RESOURCEMGR->GetSampler("TerrainHeightMap"), 1, BIND_DS);
	m_pBaseTexture = CTexture::CreateTexture(L"../../Assets/default.jpg", RESOURCEMGR->GetSampler("DEFAULT"), 0);

	//�ͷ��� ����
	//terrain
	CTerrain* pTerrain = nullptr;
	for (int j = 0; j < ONESIDE_SPACE_NUM; ++j) {
		for (int i = 0; i < ONESIDE_SPACE_NUM; ++i) {
			pTerrain = new CTerrain();
			pTerrain->Begin();
			pTerrain->SetTerrainContainer(this);
			pTerrain->SetPosition(XMVectorSet(i*ONESPACE_SIZE, 0.f, j*ONESPACE_SIZE, 0.f));
			m_pSpaceContainer->AddObject(pTerrain);
			m_vpTerrain.push_back(pTerrain);
		}
	}
	//terrain

	m_nWidth = nWidth;
	m_nLength = nLength;
	m_xmf3Scale = XMFLOAT3(static_cast<float>(m_pSpaceContainer->GetSize() / (m_nWidth)),
		fHeightScale, static_cast<float>(m_pSpaceContainer->GetSize()/(m_nLength)));

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

	//RS
	D3D11_RASTERIZER_DESC descRasterizer;
	ZeroMemory(&descRasterizer, sizeof(D3D11_RASTERIZER_DESC));
	//descRasterizer.FillMode = D3D11_FILL_SOLID;
	descRasterizer.FillMode = D3D11_FILL_WIREFRAME;
	descRasterizer.CullMode = D3D11_CULL_NONE;
	GLOBALVALUEMGR->GetDevice()->CreateRasterizerState(&descRasterizer, &m_pd3dSpaceRSState);
	//RS

	m_pSkyboxContainer = RCSELLER->GetRenderContainer(object_id::OBJECT_SKYBOX);
	m_pTerrainRenderContainer = RCSELLER->GetRenderContainer(object_id::OBJECT_TERRAIN);
	//height map data init
}

bool CTerrainContainer::End() {
	if (m_pHeightData) delete[] m_pHeightData;
	if (m_pBaseTexture) m_pBaseTexture->End();
	if (m_pHeightMapTexture) m_pHeightMapTexture->End();

	//if (m_pGlobalTerrain) delete m_pGlobalTerrain;

	if (m_pGlobalTerrainData) delete m_pGlobalTerrainData;
	if (m_pStempManager) {
		m_pStempManager->End();
		delete m_pStempManager;
	}

	if (m_pSplattingInfoManager) {
		m_pSplattingInfoManager->End();
		delete m_pSplattingInfoManager;
	}
	m_vpTerrain.clear();

	if(m_pd3dSpaceRSState)m_pd3dSpaceRSState->Release();
	if(m_pd3dTempRSState)m_pd3dTempRSState->Release();
	if(m_pd3dDepthStencilState)m_pd3dDepthStencilState->Release();
	if(m_pd3dTempDepthStencilState)m_pd3dTempDepthStencilState->Release();
	return false;
}

void CTerrainContainer::Render(shared_ptr<CCamera> pCamera){
	//skybox
	GLOBALVALUEMGR->GetDeviceContext()->OMGetDepthStencilState(&m_pd3dTempDepthStencilState, &m_TempStencil);
	GLOBALVALUEMGR->GetDeviceContext()->OMSetDepthStencilState(m_pd3dDepthStencilState, 0);
	m_pSkyboxContainer->Render(pCamera);
	GLOBALVALUEMGR->GetDeviceContext()->OMSetDepthStencilState(m_pd3dTempDepthStencilState, m_TempStencil);
	//skybox
	
	//map tool ready
	ReadyHeightMap();
	SetBufferInfo();
	//map tool ready

	m_pBaseTexture->SetShaderState();
	m_pHeightMapTexture->SetShaderState();
	m_pStempManager->SetShaderState();
	m_pGlobalTerrainBuffer->SetShaderState();
	/////////////////////////////////////////�̺κ��� ����������
	m_pSplattingInfoManager->SetShaderState();//splatting
	m_pTerrainRenderContainer->Render(pCamera);//render
	m_pSplattingInfoManager->CleanShaderState();//splatting
	/////////////////////////////////////////
	m_pBaseTexture->CleanShaderState();
	m_pHeightMapTexture->CleanShaderState();
	m_pStempManager->CleanShaderState();
	m_pGlobalTerrainBuffer->CleanShaderState();
}

float CTerrainContainer::GetHeight(XMVECTOR xmvPos){
	XMFLOAT3 xmf3Pos;
	XMStoreFloat3(&xmf3Pos, xmvPos);
	float fx = xmf3Pos.x;
	float fz = xmf3Pos.z;

	return GetHeight(XMFLOAT2(fx, fz));
}

float CTerrainContainer::GetHeight(XMFLOAT2 xmf2Pos){
	float fx = SPACE_SIZE - xmf2Pos.x;
	float fz = SPACE_SIZE - xmf2Pos.y;

	fx = fx / m_xmf3Scale.x;
	fz = fz / m_xmf3Scale.z;
	if ((fx < 0.0f) || (fz < 0.0f) || (fx >= m_nWidth) || (fz >= m_nLength)) return(0.0f);

	int x = (int)fx;
	int z = (int)fz;
	float fxPercent = fx - x;
	float fzPercent = fz - z;

	//float fBottomLeft = (float)m_pHeightMapImage[x + (z*m_nWidth)];
	//float fBottomRight = (float)m_pHeightMapImage[(x + 1) + (z*m_nWidth)];
	//float fTopLeft = (float)m_pHeightMapImage[x + ((z + 1)*m_nWidth)];
	//float fTopRight = (float)m_pHeightMapImage[(x + 1) + ((z + 1)*m_nWidth)];
	float fBottomLeft = (float)m_pHeightData[x + (z*m_nWidth)].r;
	float fBottomRight = (float)m_pHeightData[(x + 1) + (z*m_nWidth)].r;
	float fTopLeft = (float)m_pHeightData[x + ((z + 1)*m_nWidth)].r;
	float fTopRight = (float)m_pHeightData[(x + 1) + ((z + 1)*m_nWidth)].r;

	if (fzPercent < (1.0f - fxPercent))
		fTopRight = fTopLeft + (fBottomRight - fBottomLeft);
	else
		fBottomLeft = fTopLeft + (fBottomRight - fTopRight);

#ifdef _WITH_APPROXIMATE_OPPOSITE_CORNER
	if (bReverseQuad)
	{
		if (fzPercent >= fxPercent)
			fBottomRight = fBottomLeft + (fTopRight - fTopLeft);
		else
			fTopLeft = fTopRight + (fBottomLeft - fBottomRight);
	}
	else
	{
		if (fzPercent < (1.0f - fxPercent))
			fTopRight = fTopLeft + (fBottomRight - fBottomLeft);
		else
			fBottomLeft = fTopLeft + (fBottomRight - fTopRight);
	}
#endif
	float fTopHeight = fTopLeft * (1 - fxPercent) + fTopRight * fxPercent;
	float fBottomHeight = fBottomLeft * (1 - fxPercent) + fBottomRight * fxPercent;
	float fHeight = fBottomHeight * (1 - fzPercent) + fTopHeight * fzPercent;

	return(fHeight * m_xmf3Scale.y);
}

void CTerrainContainer::SetPicpos(float x, float y){
	m_xmf2CurPickPos.x = x;
	m_xmf2CurPickPos.y = y;
	m_pStempManager->SetPickPos(XMFLOAT2(x / SPACE_SIZE, y / SPACE_SIZE));
}

void CTerrainContainer::SetRenderRadius(float r){
	m_pStempManager->SetExtent(r / SPACE_SIZE);
}

void CTerrainContainer::Update(shared_ptr<CCamera> pCamera) {

	if (!pCamera) return;
	POINT p = INPUTMGR->GetMousePoint();

	//Get screen pos -> Camera pos
	XMFLOAT4X4 xmf4x4Projection;
	XMStoreFloat4x4(&xmf4x4Projection, pCamera->GetProjectionMtx());
	D3D11_VIEWPORT d3dViewport = pCamera->GetViewport();

	//�� �̰� ȭ���� ���� ���� ī�޶� ��ǥ���� �༮�̰�
	XMFLOAT3 xmf3PickPosition;
	xmf3PickPosition.x = (((2.0f * p.x) / d3dViewport.Width) - 1) / xmf4x4Projection._11;
	xmf3PickPosition.y = -(((2.0f * p.y) / d3dViewport.Height) - 1) / xmf4x4Projection._22;
	xmf3PickPosition.z = 1.0f;

	XMVECTOR xmvPickPosition;
	xmvPickPosition = XMLoadFloat3(&xmf3PickPosition);
	XMMATRIX xmMtxViewInverse;
	xmMtxViewInverse = XMMatrixInverse(nullptr, pCamera->GetViewMtx());
	//picking pos�� camera inverse�� �������� �̰� picking pos�� world pos!
	xmvPickPosition = XMVector3TransformCoord(xmvPickPosition, xmMtxViewInverse);
	XMVECTOR xmvRayDir = xmvPickPosition - pCamera->GetPosition();

	CGameObject* pNearestObject = NULL;
	float fHitDistance = FLT_MAX;
	float fNearDistance = FLT_MAX;
	pNearestObject = PickObjects(pCamera->GetPosition(), XMVector3Normalize(xmvRayDir), fHitDistance);
	fNearDistance = fHitDistance;
	
	int mode = m_StempMode;
	if (mode == STEMP_MODE_SET) {
		if (INPUTMGR->MouseLeftDown() || INPUTMGR->MouseRightDown()) {
			SetPickPosHeight();
		}
	}
	else if(mode == STEMP_MODE_INDE){
		if (INPUTMGR->MouseLeftDown()) {
			IncreasePickPosHeight();
		}
		else if (INPUTMGR->MouseRightDown()) {
			DecreasePickPosHeight();
		}
	}

	m_pStempManager->UpdateShaderState();
	m_pSplattingInfoManager->UpdateShaderState();
	return;
}

CGameObject * CTerrainContainer::PickObjects(XMVECTOR xmvWorldCameraStartPos, XMVECTOR xmvRayDir, float & distance){
	float fHitDistance = FLT_MAX;
	distance = fHitDistance;
	float fNearHitDistance = FLT_MAX;
	CGameObject* pObj = nullptr;
	//�ڽ��� ��� ��ü�� ���ؼ� �˻�
	for (auto pObject : m_vpTerrain) {
		if (pObject->CheckPickObject(xmvWorldCameraStartPos, xmvRayDir, fHitDistance)) {//ray�� �浹�ߴٸ�
			if (fNearHitDistance > fHitDistance) {//������ ���� ����� �༮�� ��
				distance = fHitDistance;//�� ������ ���� ����� ��ü ����
				return pObject;
			}

		}
	}
	//return pObj;
}

void CTerrainContainer::ReadyHeightMap(){
	m_pHeightMapTexture->End();
	
	//height map data init
	EXPORTER->MakeBitmap24(L"../TempHeightmap.bmp", m_pHeightData, m_nWidth, m_nLength);
	m_pHeightMapTexture = CTexture::CreateTexture(L"../TempHeightmap.bmp", RESOURCEMGR->GetSampler("TerrainHeightMap"), 1, BIND_DS);
}

void CTerrainContainer::SetBufferInfo(){
	TERRAIN_GLOBAL_VALUE* pData = (TERRAIN_GLOBAL_VALUE*)m_pGlobalTerrainBuffer->Map();
	//pData = m_pGlobalTerrainData;
	pData->OneSideSpaceNum = m_pGlobalTerrainData->OneSideSpaceNum;
	pData->OneSideSpaceNumRcp = m_pGlobalTerrainData->OneSideSpaceNumRcp;
	pData->OneSpaceSizeRcp = m_pGlobalTerrainData->OneSpaceSizeRcp;
	pData->HeightScale = m_pGlobalTerrainData->HeightScale;
	m_pGlobalTerrainBuffer->Unmap();
}

void CTerrainContainer::IncreasePickPosHeight(){
	if (GLOBALVALUEMGR->GetToolMode() == TOOL_MODE_TERRAIN) {
		m_pStempManager->IncreaseTerrain(m_pHeightData);
	}
	else if (GLOBALVALUEMGR->GetToolMode() == TOOL_MODE_SPLATTING) {
		if (m_pSplattingInfoManager->GetSplattingInfos().empty()) return;
		m_pStempManager->IncreaseTerrain(m_pSplattingInfoManager->GetCurSplattingInfo()->GetBlendInfo());
	}
}

void CTerrainContainer::DecreasePickPosHeight(){
	if (GLOBALVALUEMGR->GetToolMode() == TOOL_MODE_TERRAIN) {
		m_pStempManager->DecreaseTerrain(m_pHeightData);
	}
	else if (GLOBALVALUEMGR->GetToolMode() == TOOL_MODE_SPLATTING) {
		if (m_pSplattingInfoManager->GetSplattingInfos().empty()) return;
		m_pStempManager->DecreaseTerrain(m_pSplattingInfoManager->GetCurSplattingInfo()->GetBlendInfo());
	}
}

void CTerrainContainer::SetPickPosHeight(){
	if (GLOBALVALUEMGR->GetToolMode() == TOOL_MODE_TERRAIN) {
		m_pStempManager->SetTerrain(m_pHeightData);
	}
	else if (GLOBALVALUEMGR->GetToolMode() == TOOL_MODE_SPLATTING) {
		if (m_pSplattingInfoManager->GetSplattingInfos().empty()) return;
		m_pStempManager->SetTerrain(m_pSplattingInfoManager->GetCurSplattingInfo()->GetBlendInfo());
	}
}

void CTerrainContainer::SetBaseTexture(wstring path){
	if (m_pBaseTexture)m_pBaseTexture->End();
	m_pBaseTexture = nullptr;

	m_pBaseTexture = CTexture::CreateTexture(path, RESOURCEMGR->GetSampler("DEFAULT"), 0);
}

void CTerrainContainer::CreateSplattingInfo(){
	//m_pSplattingInfoManager->CreateSplattingInfo(L"../outputdata/SplattingBlendInfo/BlendInfo0.bmp", L"../../Assets/Detail_Texture_9.jpg");
	m_pSplattingInfoManager->CreateSplattingInfo(L"../../Assets/Detail_Texture/Detail_Texture_Default.jpg");
	//m_pSplattingInfoManager->CreateSplattingInfo(L"../outputdata/SplattingBlendInfo/BlendInfo0.bmp", L"../../Assets/default.jpg");
}

CTerrainContainer::CTerrainContainer() : CObject("terraincontainer") {
}

CTerrainContainer::~CTerrainContainer()
{
}

