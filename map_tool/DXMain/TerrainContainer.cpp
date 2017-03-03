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

void CTerrainContainer::Begin() {
	
	TWBARMGR->AddButtonCB("TOOL_MODE", "STEMP_MODEVIEW", "SetStemp", SMSetButtonCallback, this);
	TWBARMGR->AddButtonCB("TOOL_MODE", "STEMP_MODEVIEW", "IndeStemp", SMIndeButtonCallback, this);

	m_pSplattingInfoManager = new CSplattingInfoManager();
	m_pSplattingInfoManager->Begin();
	//m_pSplattingInfoManager->CreateSplattingInfo(L"../slide.bmp", L"../../Assets/Detail_Texture_9.jpg");

	m_pStempManager = new CStempManager();
	//모든 stemp제작
	m_pStempManager->Begin();

	//global object 제작
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
	m_pGlobalTerrainData->HeightScale = m_xmf3Scale.y;
	
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

	
	//RS
	D3D11_RASTERIZER_DESC descRasterizer;
	ZeroMemory(&descRasterizer, sizeof(D3D11_RASTERIZER_DESC));
	//descRasterizer.FillMode = D3D11_FILL_SOLID;
	descRasterizer.FillMode = D3D11_FILL_WIREFRAME;
	descRasterizer.CullMode = D3D11_CULL_NONE;
	GLOBALVALUEMGR->GetDevice()->CreateRasterizerState(&descRasterizer, &m_pd3dSpaceRSState);
	//RS

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
	return false;
}

void CTerrainContainer::Render(shared_ptr<CCamera> pCamera){
	ReadyHeightMap();
	SetBufferInfo();
	m_pBaseTexture->SetShaderState();
	m_pHeightMapTexture->SetShaderState();
	m_pNormalTexture->SetShaderState();
	m_pStempManager->SetShaderState();
	m_pGlobalTerrainBuffer->SetShaderState();
	/////////////////////////////////////////이부분을 루프돌것임
	m_pSplattingInfoManager->SetShaderState();//splatting
	m_pTerrainRenderContainer->Render(pCamera);//render
	m_pSplattingInfoManager->CleanShaderState();//splatting
	/////////////////////////////////////////
	m_pBaseTexture->CleanShaderState();
	m_pHeightMapTexture->CleanShaderState();
	m_pNormalTexture->SetShaderState();
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

	//test
	DEBUGER->AddTexture(XMFLOAT2(500, 150), XMFLOAT2(750, 300), m_pNormalTexture->GetShaderResourceView());
	DEBUGER->AddTexture(XMFLOAT2(500, 150), XMFLOAT2(750, 300), m_pNormalTexture->GetShaderResourceView());
	if (INPUTMGR->KeyDown(VK_R)) {
		CreateNormalMap();
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
	WCHAR path[256];
	wsprintf(path, L"../%sHeightMap.bmp", m_wsTerrainName.c_str());//name
	EXPORTER->MakeBitmap24(path, m_pHeightData, m_nWidth, m_nLength);
	m_pHeightMapTexture = CTexture::CreateTexture(path, RESOURCEMGR->GetSampler("TerrainHeightMap"), 1, BIND_DS);
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

void CTerrainContainer::CreateNormalMap(){
	//1. bitmap 좌표계의 height다.
	//2. 그래서 world 좌표계 즉 x z가 반대로 되는 세계로 불러들인다.
	//3. height는 그대로이다. 높이 값은 bitmap 세계에 있으니까 그냥 읽어도 됨
	//4. normal을 계산한다.
	//끝
	m_pNormalTexture->End();

	//calculate normal
	XMVECTOR xmvSumNormal = XMVectorSet(0.f,0.f,0.f,0.f);
	UINT nNormal{ 0 };
	for (int i = 0; i < m_nWidth; ++i) {
		for (int j = 0; j < m_nLength; ++j) {
			xmvSumNormal = XMVectorSet(0.f, 0.f, 0.f, 0.f);
			int xIndex = i;
			int yIndex = j;
			if ((xIndex -1) > 0 && (yIndex +1) < 255) {
				XMVECTOR p0 = XMVectorSet(256 - (xIndex), m_pHeightData[(xIndex) + (m_nLength * yIndex)].r, 256 - yIndex, 0.f);
				XMVECTOR p1 = XMVectorSet(256 - (xIndex - 1), m_pHeightData[(xIndex - 1) + (m_nLength * yIndex)].r, 256 - yIndex, 0.f);
				XMVECTOR p2 = XMVectorSet(256 - (xIndex), m_pHeightData[(xIndex)+(m_nLength * (yIndex + 1))].r, 256 - (yIndex + 1), 0.f);
				XMVECTOR edge1 = p1 - p0;
				XMVECTOR edge2 = p2 - p0;
				xmvSumNormal += XMVector3Normalize(XMVector3Cross(edge1, edge2));
				nNormal++;
			}
			if ((xIndex +1) < 255 && (yIndex +1) < 255) {
				XMVECTOR p0 = XMVectorSet(256 - (xIndex), m_pHeightData[(xIndex)+(m_nLength * yIndex)].r, 256 - yIndex, 0.f);
				XMVECTOR p1 = XMVectorSet(256 - (xIndex), m_pHeightData[(xIndex)+(m_nLength * (yIndex + 1))].r, 256 - (yIndex + 1), 0.f);
				XMVECTOR p2 = XMVectorSet(256 - (xIndex + 1), m_pHeightData[(xIndex + 1) + (m_nLength * yIndex)].r, 256 - yIndex, 0.f);
				XMVECTOR edge1 = p1 - p0;
				XMVECTOR edge2 = p2 - p0;
				xmvSumNormal += XMVector3Normalize(XMVector3Cross(edge1, edge2));
				nNormal++;
			}
			if (0 < (xIndex -1) && 0 < (yIndex -1)) {
				XMVECTOR p0 = XMVectorSet(256 - xIndex, m_pHeightData[(xIndex)+(m_nLength * yIndex)].r, 256 - yIndex, 0.f);
				XMVECTOR p1 = XMVectorSet(256 - xIndex, m_pHeightData[(xIndex)+(m_nLength * (yIndex - 1))].r, 256 - (yIndex - 1), 0.f);
				XMVECTOR p2 = XMVectorSet(256 - (xIndex - 1), m_pHeightData[(xIndex - 1) + (m_nLength * yIndex)].r, 256 - yIndex, 0.f);
				XMVECTOR edge1 = p1 - p0;
				XMVECTOR edge2 = p2 - p0;
				xmvSumNormal += XMVector3Normalize(XMVector3Cross(edge1, edge2));
				nNormal++;
			}
			if ((xIndex +1) < 255 && 0 < (yIndex -1)) {
				XMVECTOR p0 = XMVectorSet(256 - xIndex, m_pHeightData[(xIndex)+(m_nLength * yIndex)].r, 256 - yIndex, 0.f);
				XMVECTOR p1 = XMVectorSet(256 - (xIndex + 1), m_pHeightData[(xIndex + 1) + (m_nLength * yIndex)].r, 256 - yIndex, 0.f);
				XMVECTOR p2 = XMVectorSet(256 - xIndex, m_pHeightData[(xIndex)+(m_nLength * (yIndex - 1))].r, 256 - (yIndex - 1), 0.f);
				XMVECTOR edge1 = p1 - p0;
				XMVECTOR edge2 = p2 - p0;
				xmvSumNormal += XMVector3Normalize(XMVector3Cross(edge1, edge2));
				nNormal++;
			}
			XMVECTOR xmvAvgNormal = XMVector3Normalize(xmvSumNormal / nNormal);//-1 1
			xmvAvgNormal += XMVectorSet(1.f,1.f,1.f,0.f);
			xmvAvgNormal /= 2.f;//0-1

			xmvAvgNormal *= 255;//0-255
			XMFLOAT3 xmf3AvgNormal;
			XMStoreFloat3(&xmf3AvgNormal, xmvAvgNormal);
			m_pNormalData[xIndex + (yIndex * 256)].r = xmf3AvgNormal.x;
			m_pNormalData[xIndex + (yIndex * 256)].g = xmf3AvgNormal.y;
			m_pNormalData[xIndex + (yIndex * 256)].b = xmf3AvgNormal.z;
		}
	}
	//height map data init
	WCHAR path[256];
	wsprintf(path, L"../%sNormalMap.bmp", m_wsTerrainName.c_str());//name
	EXPORTER->MakeBitmap24(path, m_pNormalData, m_nWidth, m_nLength);
	m_pNormalTexture = CTexture::CreateTexture(path, RESOURCEMGR->GetSampler("TerrainNormal"), PS_SLOT_NORMALMAP, BIND_PS);

}

CTerrainContainer * CTerrainContainer::CreateTerrainContainer(LPCTSTR pTerrainName, int nWidth, int nLength, float fHeightScale, CSpaceContainer * pSpaceContainer, bool isTool){
	CTerrainContainer* pTerrainContainer = new CTerrainContainer;

	pTerrainContainer->SetSpaceContainer(pSpaceContainer);
	pTerrainContainer->SetTerrainWidth(nWidth);
	pTerrainContainer->SetTerrainLength(nLength);
	pTerrainContainer->SetTerrainScale(XMFLOAT3(static_cast<float>(pSpaceContainer->GetSize() / (nWidth)),
		fHeightScale, static_cast<float>(pSpaceContainer->GetSize() / (nLength))));
	pTerrainContainer->SetTerrainName(pTerrainName);
	if (isTool) {
		//make texture and use
		pTerrainContainer->CreateResetTextures(pTerrainName);
	}
	else {
		//just read texture
		pTerrainContainer->CreateTerrainTextures(pTerrainName);
	}

	pTerrainContainer->Begin();
	return pTerrainContainer;
}

void CTerrainContainer::CreateTerrainTextures(LPCTSTR pTerrainName){
	WCHAR path[256];
	wsprintf(path, L"../%sHeightMap.bmp", pTerrainName);//name
	m_pHeightData = IMPORTER->ReadBitmap24(path);//heightmap
	m_pHeightMapTexture = CTexture::CreateTexture(path, RESOURCEMGR->GetSampler("TerrainHeightMap"), 1, BIND_DS);
	wsprintf(path, L"../%sNormalMap.bmp", pTerrainName);
	m_pNormalData = IMPORTER->ReadBitmap24(path);//nomalmap
	m_pNormalTexture = CTexture::CreateTexture(path, RESOURCEMGR->GetSampler("TerrainNormal"), PS_SLOT_NORMALMAP, BIND_PS);
	m_pBaseTexture = CTexture::CreateTexture(L"../../Assets/default.jpg", RESOURCEMGR->GetSampler("DEFAULT"), 0);
}

void CTerrainContainer::CreateResetTextures(LPCTSTR pTerrainName){
	//texture 제작
	//height map data init
	WCHAR path[256];
	wsprintf(path, L"../%sHeightMap.bmp", pTerrainName);//name
	m_pHeightData = new Pixel24[(m_nWidth) * (m_nLength)];//pixel data
	ZeroMemory(m_pHeightData, sizeof(Pixel24) * (m_nWidth) * (m_nLength));
	EXPORTER->MakeBitmap24(path, m_pHeightData, m_nWidth, m_nLength);
	m_pHeightMapTexture = CTexture::CreateTexture(path, RESOURCEMGR->GetSampler("TerrainHeightMap"), 1, BIND_DS);
	m_pBaseTexture = CTexture::CreateTexture(L"../../Assets/default.jpg", RESOURCEMGR->GetSampler("DEFAULT"), 0);
	//normal
	wsprintf(path, L"../%sNormalMap.bmp", pTerrainName);
	m_pNormalData = new Pixel24[(m_nWidth) * (m_nLength)];
	ZeroMemory(m_pNormalData, sizeof(Pixel24) * m_nWidth * m_nLength);
	EXPORTER->MakeBitmap24(path, m_pNormalData, m_nWidth, m_nLength);
	m_pNormalTexture = CTexture::CreateTexture(path, RESOURCEMGR->GetSampler("TerrainNormal"), PS_SLOT_NORMALMAP, BIND_PS);
	CreateNormalMap();
}

CTerrainContainer::CTerrainContainer() : CObject("terraincontainer") {
}

CTerrainContainer::~CTerrainContainer()
{
}

