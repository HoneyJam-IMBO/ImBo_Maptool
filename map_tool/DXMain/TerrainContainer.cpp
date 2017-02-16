#include "stdafx.h"
#include "TerrainContainer.h"
#include "SpaceContainer.h"

#define TEXTURE_SIZE 257
void CTerrainContainer::Begin(LPCTSTR pHeightmapFileName, int nWidth, int nLength, float fHeightScale, CSpaceContainer* pSpaceContainer) {
	m_pSpaceContainer = pSpaceContainer;

	//global object 제작
	//m_pGlobalTerrain = new CGlobalTerrain();
	////global object set, Update
	//m_pGlobalTerrain->Begin(fOneSpaceSize, fOneSideSpaceNum, fHeightScale);
	m_pPicposTexture = RESOURCEMGR->GetTexture("PICPOS");
	m_pGlobalTerrainBuffer = RESOURCEMGR->GetGlobalBuffer("TerrainGB");
	m_pPicposRenderInfoBuffer = CBuffer::CreateConstantBuffer(1, sizeof(TERRAIN_PICPOS_RENDER_INFO), 4, BIND_PS);

	float fOneSpaceSize = (float)m_pSpaceContainer->GetOneSpaceSize();
	float fOneSideSpaceNum = (float)m_pSpaceContainer->GetOneSideSpaceNum();
	m_pGlobalTerrainData = new TERRAIN_GLOBAL_VALUE;
	m_pGlobalTerrainData->OneSideSpaceNum = fOneSideSpaceNum;
	m_pGlobalTerrainData->OneSideSpaceNumRcp = 1.0f / fOneSideSpaceNum;
	m_pGlobalTerrainData->OneSpaceSizeRcp = 1.0f / fOneSpaceSize;
	m_pGlobalTerrainData->HeightScale = fHeightScale;
	m_pHeightData = new Pixel[(nWidth - 1) * (nLength - 1)];
	ZeroMemory(m_pHeightData, sizeof(Pixel) * (nWidth - 1) * (nLength - 1));
	//height map data init
	EXPORTER->MakeBitmap(L"../bitmap.bmp", m_pHeightData, nWidth-1, nLength-1);
	m_pHeightMapTexture = CTexture::CreateTexture(L"../bitmap.bmp", RESOURCEMGR->GetSampler("TerrainHeightMap"), 1, BIND_DS);

	m_pPicposRenderInfo = new TERRAIN_PICPOS_RENDER_INFO;
	//터레인 제작
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
	m_xmf3Scale = XMFLOAT3(static_cast<float>(m_pSpaceContainer->GetSize() / (m_nWidth-1)),
		fHeightScale, static_cast<float>(m_pSpaceContainer->GetSize()/(m_nLength - 1)));

	BYTE *pHeightMapImage = new BYTE[m_nWidth * m_nLength];

	HANDLE hFile = ::CreateFile(pHeightmapFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_READONLY, NULL);
	DWORD dwBytesRead;
	::ReadFile(hFile, pHeightMapImage, (m_nWidth * m_nLength), &dwBytesRead, NULL);
	::CloseHandle(hFile);

	m_pHeightMapImage = new BYTE[m_nWidth * m_nLength];
	for (int y = 0; y < m_nLength; y++)
	{
		for (int x = 0; x < m_nWidth; x++)
		{
			m_pHeightMapImage[x + ((m_nLength - 1 - y)*m_nWidth)] = pHeightMapImage[x + (y*m_nWidth)];
		}
	}

	if (pHeightMapImage) delete[] pHeightMapImage;

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
	//if (m_pGlobalTerrain) delete m_pGlobalTerrain;
	if (m_pGlobalTerrainData) delete m_pGlobalTerrainData;
	if (m_pPicposRenderInfo) delete m_pPicposRenderInfo;
	if (m_pPicposRenderInfo) m_pPicposRenderInfoBuffer->End();
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

	m_pGlobalTerrainBuffer->SetShaderState();
	m_pPicposRenderInfoBuffer->SetShaderState();
	m_pPicposTexture->SetShaderState();
	m_pTerrainRenderContainer->Render(pCamera);
	m_pGlobalTerrainBuffer->CleanShaderState();
	m_pPicposRenderInfoBuffer->CleanShaderState();
	m_pPicposTexture->SetShaderState();
	//terrain
	m_pHeightMapTexture->CleanShaderState();
}

float CTerrainContainer::GetHeight(XMVECTOR xmvPos){
	XMFLOAT3 xmf3Pos;
	XMStoreFloat3(&xmf3Pos, xmvPos);
	float fx = xmf3Pos.x;
	float fz = xmf3Pos.z;

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
	float posX = x / SPACE_SIZE;
	float posY = (y / SPACE_SIZE);
	m_pPicposRenderInfo->PickPos = XMFLOAT2(posX, posY);
}

void CTerrainContainer::SetRenderRadius(float r){
	m_pPicposRenderInfo->RenderSize = r / SPACE_SIZE;
}

void CTerrainContainer::Update(shared_ptr<CCamera> pCamera){

	if (!pCamera) return;
	POINT p = INPUTMGR->GetMousePoint();

	//Get screen pos -> Camera pos
	XMFLOAT4X4 xmf4x4Projection;
	XMStoreFloat4x4(&xmf4x4Projection, pCamera->GetProjectionMtx());
	D3D11_VIEWPORT d3dViewport = pCamera->GetViewport();

	//음 이건 화면을 찍은 점의 카메라 좌표계의 녀석이고
	XMFLOAT3 xmf3PickPosition;
	xmf3PickPosition.x = (((2.0f * p.x) / d3dViewport.Width) - 1) / xmf4x4Projection._11;
	xmf3PickPosition.y = -(((2.0f * p.y) / d3dViewport.Height) - 1) / xmf4x4Projection._22;
	xmf3PickPosition.z = 1.0f;

	XMVECTOR xmvPickPosition;
	xmvPickPosition = XMLoadFloat3(&xmf3PickPosition);
	XMMATRIX xmMtxViewInverse;
	xmMtxViewInverse = XMMatrixInverse(nullptr, pCamera->GetViewMtx());
	//picking pos에 camera inverse를 곱했으니 이건 picking pos의 world pos!
	xmvPickPosition = XMVector3TransformCoord(xmvPickPosition, xmMtxViewInverse);
	XMVECTOR xmvRayDir = xmvPickPosition - pCamera->GetPosition();

	CGameObject* pNearestObject = NULL;
	float fHitDistance = FLT_MAX;
	float fNearDistance = FLT_MAX;
	pNearestObject = PickObjects(pCamera->GetPosition(), XMVector3Normalize(xmvRayDir), fHitDistance);
	fNearDistance = fHitDistance;

	if (INPUTMGR->MouseLeftDown()) {
		RisedByPickPos();
	}
	return;
}

CGameObject * CTerrainContainer::PickObjects(XMVECTOR xmvWorldCameraStartPos, XMVECTOR xmvRayDir, float & distance){
	float fHitDistance = FLT_MAX;
	distance = fHitDistance;
	float fNearHitDistance = FLT_MAX;
	CGameObject* pObj = nullptr;
	//자신의 모든 객체에 대해서 검사
	for (auto pObject : m_vpTerrain) {
		if (pObject->CheckPickObject(xmvWorldCameraStartPos, xmvRayDir, fHitDistance)) {//ray와 충돌했다면
			if (fNearHitDistance > fHitDistance) {//이전의 가장 가까운 녀석과 비교
				distance = fHitDistance;//더 가까우면 가장 가까운 객체 변경
				return pObject;
			}
		}
	}
	//return pObj;
}

void CTerrainContainer::ReadyHeightMap(){
	m_pHeightMapTexture->End();
	
	//height map data init
	EXPORTER->MakeBitmap(L"../bitmap.bmp", m_pHeightData, m_nWidth - 1, m_nLength - 1);
	m_pHeightMapTexture = CTexture::CreateTexture(L"../bitmap.bmp", RESOURCEMGR->GetSampler("TerrainHeightMap"), 1, BIND_DS);

	m_pHeightMapTexture->SetShaderState();
}

void CTerrainContainer::SetBufferInfo(){
	TERRAIN_GLOBAL_VALUE* pData = (TERRAIN_GLOBAL_VALUE*)m_pGlobalTerrainBuffer->Map();
	//pData = m_pGlobalTerrainData;
	pData->OneSideSpaceNum = m_pGlobalTerrainData->OneSideSpaceNum;
	pData->OneSideSpaceNumRcp = m_pGlobalTerrainData->OneSideSpaceNumRcp;
	pData->OneSpaceSizeRcp = m_pGlobalTerrainData->OneSpaceSizeRcp;
	pData->HeightScale = m_pGlobalTerrainData->HeightScale;
	m_pGlobalTerrainBuffer->Unmap();

	
	TERRAIN_PICPOS_RENDER_INFO* pPicposRenderInfo = (TERRAIN_PICPOS_RENDER_INFO*)m_pPicposRenderInfoBuffer->Map();
	pPicposRenderInfo->PickPos.x = m_pPicposRenderInfo->PickPos.x;
	pPicposRenderInfo->PickPos.y = 1 - m_pPicposRenderInfo->PickPos.y;
	pPicposRenderInfo->RenderSize = m_pPicposRenderInfo->RenderSize;
	m_pPicposRenderInfoBuffer->Unmap();
}

void CTerrainContainer::RisedByPickPos(){
	XMFLOAT2 pickPos = m_pPicposRenderInfo->PickPos;
	int x = (1 - pickPos.x) * (m_nWidth - 2);
	int y = (1 - pickPos.y) * (m_nLength - 2);

	//ZeroMemory(m_pHeightData, sizeof(Pixel) * (m_nWidth - 1) * (m_nLength - 1));
	for (int i = -5; i < 5; ++i) {
		for (int j = -5; j < 5; ++j) {
			int index = ((x + i) + (y + j)*(m_nLength - 1));
			if (index < 0) continue;
			if (index >(m_nWidth - 1) * (m_nLength - 1)) continue;
			//m_pHeightData[index].r = m_pHeightData[index].r + 1;
			m_pHeightData[index].g = m_pHeightData[index].g + 1;
			m_pHeightData[index].b = m_pHeightData[index].b + 1;
		}
	}
}

CTerrainContainer::CTerrainContainer() : CObject("terraincontainer") {
}

CTerrainContainer::~CTerrainContainer()
{
}

