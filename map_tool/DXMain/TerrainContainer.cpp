#include "stdafx.h"
#include "TerrainContainer.h"
#include "SpaceContainer.h"

#define TEXTURE_SIZE 257
void CTerrainContainer::Begin(LPCTSTR pHeightmapFileName, int nWidth, int nLength, float fHeightScale, CSpaceContainer* pSpaceContainer) {
	m_pSpaceContainer = pSpaceContainer;

	//global object 제작
	m_pGlobalTerrain = new CGlobalTerrain();
	float fOneSpaceSize = (float)m_pSpaceContainer->GetOneSpaceSize();
	float fOneSideSpaceNum = (float)m_pSpaceContainer->GetOneSideSpaceNum();
	//global object set, Update
	m_pGlobalTerrain->Begin(fOneSpaceSize, fOneSideSpaceNum, fHeightScale);

	m_pHeightMapBuffer = CBuffer::CreateConstantBuffer(TEXTURE_SIZE*TEXTURE_SIZE, sizeof(UINT), 1, BIND_DS);

	//터레인 제작
	//terrain
	CTerrain* pTerrain = nullptr;
	for (int j = 0; j < ONESIDE_SPACE_NUM; ++j) {
		for (int i = 0; i < ONESIDE_SPACE_NUM; ++i) {
			pTerrain = new CTerrain();
			pTerrain->Begin();
			//pTerrain->SetRenderContainer(pSeller);
			pTerrain->SetPosition(XMVectorSet(i*ONESPACE_SIZE, 0.f, j*ONESPACE_SIZE, 0.f));
			m_pSpaceContainer->AddObject(pTerrain);
		}
	}
	//terrain

	//height map data init
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
	if (m_pGlobalTerrain) delete m_pGlobalTerrain;

	return false;
}

void CTerrainContainer::Render(shared_ptr<CCamera> pCamera){
	//skybox
	GLOBALVALUEMGR->GetDeviceContext()->OMGetDepthStencilState(&m_pd3dTempDepthStencilState, &m_TempStencil);
	GLOBALVALUEMGR->GetDeviceContext()->OMSetDepthStencilState(m_pd3dDepthStencilState, 0);
	m_pSkyboxContainer->Render(pCamera);
	GLOBALVALUEMGR->GetDeviceContext()->OMSetDepthStencilState(m_pd3dTempDepthStencilState, m_TempStencil);
	//skybox

	//terrain
	m_pTerrainRenderContainer->Render(pCamera);
	//terrain
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

	float fBottomLeft = (float)m_pHeightMapImage[x + (z*m_nWidth)];
	float fBottomRight = (float)m_pHeightMapImage[(x + 1) + (z*m_nWidth)];
	float fTopLeft = (float)m_pHeightMapImage[x + ((z + 1)*m_nWidth)];
	float fTopRight = (float)m_pHeightMapImage[(x + 1) + ((z + 1)*m_nWidth)];

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

CTerrainContainer::CTerrainContainer() : CObject("terraincontainer") {
}

CTerrainContainer::~CTerrainContainer()
{
}

