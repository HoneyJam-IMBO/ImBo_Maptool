#include "stdafx.h"
#include "ResourceManager.h"

bool CResourceManager::Begin(){
	CreateTextures();
	CreateRenderShaders();
	CreateBuffers();
	CreateGlobalBuffers();
	CreateMaterials();
	CreateMeshs();
	CreateAnimaters();
	CreateGJMResources();
	CreateFBXResources();

	return true;
}

bool CResourceManager::End(){
	ReleaseTextures();
	ReleaseRenderShaders();
	ReleaseBuffers();
	ReleaseGlobalBuffers();
	ReleaseMaterials();
	ReleaseMeshs();
	ReleaseAnimaters();

	return true;
}

void CResourceManager::CreateTextures() {
	//
	//texture
	shared_ptr<CSampler> pSampler;
	//skybox
	int nIndex = 0;
	_TCHAR pstrTextureNames[128];
	_stprintf_s(pstrTextureNames, _T("../../Assets/SkyBox_%d.dds"), nIndex);
	//make sampler
	
	CreateSampler("CLAMPSAMPLER", 0, BIND_PS, D3D11_TEXTURE_ADDRESS_WRAP, D3D11_FILTER_MIN_MAG_MIP_LINEAR,
		D3D11_COMPARISON_ALWAYS, 0, D3D11_FLOAT32_MAX);

	pSampler = CreateSampler("DEFAULT", PS_TEXTURE_SAMPLER, BIND_PS);
	CreateTexture("SkyBox", pstrTextureNames, pSampler, PS_SLOT_SKYBOX, BIND_PS);
	CreateTexture("DEFAULT", _T("../../Assets/default.jpg"), pSampler, PS_TEXTURE, BIND_PS);
	CreateTexture("PICPOS", _T("../../Assets/default.jpg"), pSampler, 2, BIND_PS);

	//terrain heightmap
	//make sampler;
	pSampler = CreateSampler("TerrainHeightMap", PS_TEXTURE_SAMPLER, BIND_DS, D3D11_TEXTURE_ADDRESS_CLAMP);
	CreateTexture("TerrainHeightMap", _T("../../Assets/HeightMap.bmp"), pSampler, DS_SLOT_HEIGHTMAP, BIND_DS);
	//terrain base texture
	pSampler = CreateSampler("Terrain", PS_TEXTURE_SAMPLER, BIND_PS);
	CreateTexture("TerrainBase", _T("../../Assets/Base_Texture.jpg"), pSampler, PS_SLOT_TERRAIN_BASE, BIND_PS);
	//terrain detail texture
	CreateTexture("TerrainDetail", _T("../../Assets/Detail_Texture_9.jpg"), pSampler, PS_SLOT_TERRAIN_DETAIL, BIND_PS);
	//terrain normal map
	//make sampler
	pSampler = CreateSampler("TerrainNormal", PS_NORMALMAP_SAMPLER, BIND_PS);
	//make sampler
	CreateTexture("TerrainNormalMap", _T("../../Assets/Base_Texture_Normal.jpg"), pSampler, PS_SLOT_NORMALMAP, BIND_PS);
	CreateTexture("FBX", _T("../../Assets/Model/Test/03_Monster/tex_Zombunny_diffuse.png"), m_mSampler["DEFAULT"], PS_TEXTURE, BIND_PS);

}

void CResourceManager::CreateRenderShaders() {
	//default
	CreateRenderShader("DEFAULT", TEXT("Default"),
		IE_POSITION | IE_NORMAL | IE_TEXCOORD | IE_INSWORLDMTX);
	//default

	//animation render shader
	CreateRenderShader("AnimationObject", TEXT("AnimationObject"),
		IE_POSITION | IE_NORMAL | IE_TEXCOORD | IE_BONEWEIGHT | IE_BONEINDEX);
	//animation render shader

	//post processing shader
	CreateRenderShader("PostProcessing", L"PostProcessing",
		IE_POSITION | IE_TEXCOORD);
	//post processing shader
	//terrain
	CreateRenderShader("Terrain", L"Terrain",
		IE_POSITION | IE_INSWORLDMTX,
		BIND_VS | BIND_HS | BIND_DS | BIND_PS);
	//terrain
	//BoundingBox
	CreateRenderShader("BoundingBox", L"BoundingBox",
		IE_INSPOS | IE_INSFLOAT_C_A | IE_INSQUATERNION,
		BIND_VS | BIND_GS | BIND_PS);
	//BoundingBox

	//debug textue
	CreateRenderShader("DebugTexture", TEXT("DebugTexture"),
		IE_INSFLOAT_B_A | IE_INSFLOAT_B_B,
		BIND_VS | BIND_GS | BIND_PS);

	//shader
	CreateRenderShader("SkyBox", TEXT("SkyBox"),
		IE_POSITION | IE_TEXCOORD | IE_INSWORLDMTX);


	//coordinatesys render shader
	CreateRenderShader("CoordinateSys", L"CoordinateSys",
		IE_POSITION | IE_INSWORLDMTX);
	//coordinatesys render shader

	//light shader
	shared_ptr<CRenderShader> pShader = CRenderShader::CreateRenderShader(L"DirectionalLight");
	m_mRenderShader.insert(pairShader("DirectionalLight", pShader));

	pShader = CRenderShader::CreateRenderShader(L"PointLight", 0,
		BIND_VS | BIND_HS | BIND_DS | BIND_PS);
	m_mRenderShader.insert(pairShader("PointLight", pShader));

	LPCTSTR pDebugPointLightShaderName[] = { TEXT("VSPointLight.cso") , TEXT("HSPointLight.cso"), TEXT("DSPointLight.cso"), TEXT("PSDebugLight.cso") };
	pShader = CRenderShader::CreateRenderShader(pDebugPointLightShaderName, 0,
		BIND_VS | BIND_HS | BIND_DS | BIND_PS);
	m_mRenderShader.insert(pairShader("DebugPointLight", pShader));

	pShader = CRenderShader::CreateRenderShader(L"CapsuleLight", 0,
		BIND_VS | BIND_HS | BIND_DS | BIND_PS);
	m_mRenderShader.insert(pairShader("CapsuleLight", pShader));

	LPCTSTR pDebugCapsuleLightShaderName[] = { TEXT("VSCapsuleLight.cso") , TEXT("HSCapsuleLight.cso"), TEXT("DSCapsuleLight.cso"), TEXT("PSDebugLight.cso") };
	pShader = CRenderShader::CreateRenderShader(pDebugCapsuleLightShaderName, 0,
		BIND_VS | BIND_HS | BIND_DS | BIND_PS);
	m_mRenderShader.insert(pairShader("DebugCapsuleLight", pShader));

	pShader = CRenderShader::CreateRenderShader(L"SpotLight", 0,
		BIND_VS | BIND_HS | BIND_DS | BIND_PS);
	m_mRenderShader.insert(pairShader("SpotLight", pShader));

	LPCTSTR pDebugSpotLightShaderName[] = { TEXT("VSSpotLight.cso") , TEXT("HSSpotLight.cso"), TEXT("DSSpotLight.cso"), TEXT("PSDebugLight.cso") };
	pShader = CRenderShader::CreateRenderShader(pDebugSpotLightShaderName, 0,
		BIND_VS | BIND_HS | BIND_DS | BIND_PS);
	m_mRenderShader.insert(pairShader("DebugSpotLight", pShader));
	//light shader

	//lay trace shader
	pShader = CRenderShader::CreateRenderShader(L"RayTrace");
	m_mRenderShader.insert(pairShader("RayTrace", pShader));
	//combine shader
	pShader = CRenderShader::CreateRenderShader(L"Combine");
	m_mRenderShader.insert(pairShader("Combine", pShader));
}

void CResourceManager::CreateMeshs() {
	//mesh
	shared_ptr<CMesh> pMesh;

	pMesh = make_shared<CTestMesh>();
	pMesh->Begin();
	m_mMesh.insert(pairMesh("Rect1", pMesh));

	pMesh = make_shared<CTestMesh>();
	pMesh->Begin();
	m_mMesh.insert(pairMesh("Rect2", pMesh));

	pMesh = make_shared<CTestMesh>();
	pMesh->Begin();
	m_mMesh.insert(pairMesh("Rect3", pMesh));

	pMesh = make_shared<CTestMesh>();
	pMesh->Begin();
	m_mMesh.insert(pairMesh("Rect4", pMesh));

	pMesh = make_shared<CTestMesh>();
	pMesh->Begin();
	m_mMesh.insert(pairMesh("Rect5", pMesh));

	pMesh = make_shared<CPlaneMesh>();
	pMesh->Begin();
	m_mMesh.insert(pairMesh("Plane", pMesh));

	pMesh = make_shared<CDirectionalLightMesh>();
	pMesh->Begin();
	m_mMesh.insert(pairMesh("DirectionalLight", pMesh));

	pMesh = make_shared<CPointLightMesh>();
	pMesh->Begin();
	m_mMesh.insert(pairMesh("PointLight", pMesh));

	pMesh = make_shared<CSpotLightMesh>();
	pMesh->Begin();
	m_mMesh.insert(pairMesh("SpotLight", pMesh));

	pMesh = make_shared<CCapsuleLightMesh>();
	pMesh->Begin();
	m_mMesh.insert(pairMesh("CapsuleLight", pMesh));

	pMesh = make_shared<CTestDeferredMesh>();
	pMesh->Begin();
	m_mMesh.insert(pairMesh("PostProcessing", pMesh));

	pMesh = make_shared<CSpaceMesh>();
	pMesh->Begin();
	m_mMesh.insert(pairMesh("Space", pMesh));

	pMesh = make_shared<CTerrainMesh>();
	pMesh->Begin();
	m_mMesh.insert(pairMesh("Terrain", pMesh));

	pMesh = make_shared<CBoundingBoxMesh>();
	pMesh->Begin();
	m_mMesh.insert(pairMesh("BoundingBox", pMesh));

	//debug
	pMesh = make_shared<CDebugTextureMesh>();
	pMesh->Begin();
	m_mMesh.insert(pairMesh("DebugTexture", pMesh));

	pMesh = make_shared<CCoordinateSysMesh>();
	pMesh->Begin();
	m_mMesh.insert(pairMesh("CoordinateSys", pMesh));
	//skybox
	pMesh = make_shared<CSkyBoxMesh>();
	pMesh->Begin();
	m_mMesh.insert(pairMesh("SkyBox", pMesh));

#ifdef USE_ANIM
	shared_ptr<CFileBasedMesh> pTestFBXMesh = make_shared<CFileBasedMesh>();
#else
	shared_ptr<CUseFBXMesh> pTestFBXMesh = make_shared<CUseFBXMesh>();
#endif
	//ddd
	//CreateMultiMesh("../../Assets/Model/fbx/1-2/Die_85.fbx", "Test");
	//CreateAnimater("../../Assets/Model/fbx/1-1/ATK1_45.fbx");

	//CreateMultiMesh("../../Assets/Model/fbx/Bless_Elf.fbx");
	//CreateMultiMesh("../../Assets/Model/fbx/2-1/ATK1_45.fbx");


	//pTestFBXMesh->Begin("../../Assets/Model/fbx/Bless_Elf.fbx");
	//pTestFBXMesh->Begin("../../Assets/Model/Test/02_Character_Juno/Juno_idle.fbx");
	//pTestFBXMesh->Begin("../../Assets/Model/Test/unit04_attack.fbx");
	//pTestFBXMesh->Begin("../../Assets/Model/Test/assback3_11_attack.fbx");
	//pTestFBXMesh->Begin("../../Assets/Model/Test/character1_move_r.fbx");
	//pTestFBXMesh->Begin("../../Assets/Model/Test/humanoid.fbx");
	//FBXIMPORTER->Begin("../../Assets/Model/Test/03_Monster/Zombunny_running.fbx");
	//pTestFBXMesh->Begin();
	//shared_ptr<CAnimater> pAnimater = make_shared<CAnimater>();
	//pAnimater->Begin();
	//CAnimationInfo* pAnimationInfo = new CAnimationInfo();
	//pAnimationInfo->Begin(pAnimater);
	//m_mAnimater.insert(pairAnimater("BUNNY", pAnimater));
	//FBXIMPORTER->End();
	//
	//m_mMesh.insert(pairMesh("BUNNY", pTestFBXMesh));


	//mesh
}
void CResourceManager::CreateBuffers() {
	//instance buffer
	shared_ptr<CBuffer> pConstantBuffer;

	CreateInstancingBuffer("DEFAULTIB", 1000, sizeof(VS_VB_INSTANCE));

	CreateInstancingBuffer("50000IB", 50000, sizeof(VS_VB_INSTANCE));

	CreateInstancingBuffer("ONEIB", 1, sizeof(VS_VB_INSTANCE));

	CreateInstancingBuffer("SpaceIB", (UINT)SPACE_NUM, sizeof(VS_VB_INSTANCE));
	CreateInstancingBuffer("TerrainIB", (UINT)SPACE_NUM, sizeof(VS_VB_INSTANCE));

	CreateInstancingBuffer("BoundingBoxIB", BOUNDINGBOX_NUM, sizeof(VS_VB_BOUNDINGBOX_INSTANCE));
	CreateInstancingBuffer("CoordinateSysIB", COORD_NUM, sizeof(VS_VB_INSTANCE));
	//debug texture
	CreateInstancingBuffer("DebugTextureIB", 1, sizeof(VS_VB_DEBUG_TEXTURE_INSTANCE));
	//instance buffer
	//test fbx
	CreateConstantBuffer("FBX", 1, sizeof(VS_VB_INSTANCE), VS_CB_MODEL, BIND_VS);
	//light buffer
	CreateConstantBuffer("DirectionalLightCB", 1, sizeof(DIRECTIONAL_AMBIENT_LIGHT), PS_OBJECT_BUFFER_SLOT, BIND_PS);
	CreateConstantBuffer("PointLightCB1", 1000, sizeof(POINT_LIGHT_DS_CB), DS_OBJECT_BUFFER_SLOT, BIND_DS);
	CreateConstantBuffer("PointLightCB2", 1000, sizeof(POINT_LIGHT_PS_CB), PS_OBJECT_BUFFER_SLOT, BIND_PS);
	CreateConstantBuffer("SpotLightCB1", 1000, sizeof(SPOT_LIGHT_DS_CB), DS_OBJECT_BUFFER_SLOT, BIND_DS);
	CreateConstantBuffer("SpotLightCB2", 1000, sizeof(SPOT_LIGHT_PS_CB), PS_OBJECT_BUFFER_SLOT, BIND_PS);
	CreateConstantBuffer("CapsuleLightCB1", 1000, sizeof(CAPSULE_LIGHT_DS_CB), DS_OBJECT_BUFFER_SLOT, BIND_DS);
	CreateConstantBuffer("CapsuleLightCB2", 1000, sizeof(CAPSULE_LIGHT_PS_CB), PS_OBJECT_BUFFER_SLOT, BIND_PS);
}

void CResourceManager::CreateGlobalBuffers() {
	CreateGlobalBuffer("TerrainGB", 1, sizeof(TERRAIN_GLOBAL_VALUE), VS_GLOBAL_BUFFER_SLOT, BIND_VS | BIND_DS);
}

void CResourceManager::CreateMaterials() {

	shared_ptr<CMaterial> pMaterial;
	//material
	CreateMaterial("DEFAULT", XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f), 1.0f, 1.0f);
	CreateMaterial("RED", XMFLOAT4(1.f, 0.f, 0.f, 1.0f), 1.0f, 1.0f);
	CreateMaterial("Core", XMFLOAT4(0.5f, 0.1f, 0.1f, 1.0f), 4.0f, 1.0f);
	CreateMaterial("Boost", XMFLOAT4(0.1f, 0.6f, 0.6f, 1.0f), 4.0f, 1.0f);
	CreateMaterial("Weapon", XMFLOAT4(0.1f, 0.7f, 0.0f, 1.0f), 4.0f, 1.0f);
	CreateMaterial("Adaptor", XMFLOAT4(0.6f, 0.5f, 0.1f, 1.0f), 4.0f, 1.0f);
	CreateMaterial("Plane", XMFLOAT4(0.5f, 0.5f, 0.0f, 1.0f), 8.0f, 2.0f);
	CreateMaterial("Terrain", XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 8.0f, 5.0f);
	CreateMaterial("BoundingBox", XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 8.0f, 2.0f);
	CreateMaterial("FBX", XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 1.f, 1.f);
	CreateMaterial("BUNNY", XMFLOAT4(1.0f, 1.f, 1.f, 1.f), 1.f, 1.f);
	CreateMaterial("SkyBox", XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 1.f, 1.f);
}
void CResourceManager::CreateAnimaters() {

}
void CResourceManager::CreateGJMResources() {
	//WCHAR*pwchar;
	//IMPORTER->ReadWCHAR(pwchar, 0);
}
void CResourceManager::CreateFBXResources() {

}

shared_ptr<CTexture> CResourceManager::CreateTexture(string name, UINT nTextures, _TCHAR(*ppstrFilePaths)[128], shared_ptr<CSampler> pSampler, UINT Slot, UINT BindFlag, shared_ptr<CBuffer> pConstantBuffer){
	shared_ptr<CTexture> pTexture;
	pTexture = CTexture::CreateTexture(nTextures, ppstrFilePaths, pSampler, Slot, BindFlag, pConstantBuffer);
	m_mTexture.insert(pairTexture(name, pTexture));
	return m_mTexture[name];
}

shared_ptr<CTexture> CResourceManager::CreateTexture(string name, UINT nTextures, ID3D11Texture2D ** ppd3dTextures, shared_ptr<CSampler> pSampler, UINT Slot, UINT BindFlag, shared_ptr<CBuffer> pConstantBuffer){
	shared_ptr<CTexture> pTexture;
	pTexture = CTexture::CreateTexture(nTextures, ppd3dTextures, pSampler, Slot, BindFlag, pConstantBuffer);
	m_mTexture.insert(pairTexture(name, pTexture));
	return m_mTexture[name];
}

shared_ptr<CTexture> CResourceManager::CreateTexture(string name, _TCHAR(pstrFilePath)[128], shared_ptr<CSampler> pSampler, UINT Slot, UINT BindFlag, shared_ptr<CBuffer> pConstantBuffer){
	shared_ptr<CTexture> pTexture;
	pTexture = CTexture::CreateTexture(pstrFilePath, pSampler, Slot, BindFlag, pConstantBuffer);
	m_mTexture.insert(pairTexture(name, pTexture));
	return m_mTexture[name];
}

shared_ptr<CTexture> CResourceManager::CreateTexture(string name, ID3D11ShaderResourceView * pShaderResourceView, shared_ptr<CSampler> pSampler, UINT Slot, UINT BindFlag, shared_ptr<CBuffer> pConstantBuffer){
	shared_ptr<CTexture> pTexture;
	pTexture = CTexture::CreateTexture(pShaderResourceView, pSampler, Slot, BindFlag, pConstantBuffer);
	m_mTexture.insert(pairTexture(name, pTexture));
	return m_mTexture[name];
}

shared_ptr<CSampler> CResourceManager::CreateSampler(string name, UINT Slot, UINT BindFlags, D3D11_TEXTURE_ADDRESS_MODE Mode, D3D11_FILTER Filter, D3D11_COMPARISON_FUNC ComparisionFunc, float MinLOD, float MaxLOD){
	shared_ptr<CSampler> pSampler;
	pSampler = CSampler::CreateSampler(Slot, BindFlags, Mode, Filter, ComparisionFunc, MinLOD, MaxLOD);
	m_mSampler.insert(pairSampler(name, pSampler));
	return m_mSampler[name];
}

shared_ptr<CRenderShader> CResourceManager::CreateRenderShader(string name, LPCTSTR ShaderName, UINT InputElementFlag, UINT BindFlag){
	shared_ptr<CRenderShader> pShader = CRenderShader::CreateRenderShader(ShaderName,InputElementFlag, BindFlag);
	m_mRenderShader.insert(pairShader(name, pShader));
	return m_mRenderShader[name];
}

shared_ptr<CBuffer> CResourceManager::CreateConstantBuffer(string name, UINT nObject, UINT BufferStride, UINT Slot, UINT BindFlag, UINT Offset){
	shared_ptr<CBuffer> pBuffer  = CBuffer::CreateConstantBuffer(nObject, BufferStride, Slot, BindFlag, Offset);
	m_mBuffer.insert(pairBuffer(name, pBuffer));
	return m_mBuffer[name];
}

shared_ptr<CBuffer> CResourceManager::CreateInstancingBuffer(string name, UINT nObject, UINT BufferStride, UINT Offset){
	shared_ptr<CBuffer> pBuffer = CBuffer::CreateInstancingBuffer(nObject, BufferStride, Offset);
	m_mBuffer.insert(pairBuffer(name, pBuffer));
	return m_mBuffer[name];
}

shared_ptr<CBuffer> CResourceManager::CreateGlobalBuffer(string name, UINT nObject, UINT BufferStride, UINT Slot, UINT BindFlag, UINT Offset) {
	shared_ptr<CBuffer> pBuffer = CBuffer::CreateConstantBuffer(nObject, BufferStride, Slot, BindFlag, Offset);
	m_mGlobalBuffer.insert(pairBuffer(name, pBuffer));
	return m_mGlobalBuffer[name];
}

shared_ptr<CMaterial> CResourceManager::CreateMaterial(string name, XMFLOAT4 color, float specExp, float specIntensity){
	shared_ptr<CMaterial> pMaterial;
	pMaterial = CMaterial::CreateMaterial(color, specExp, specIntensity);
	m_mMaterial.insert(pairMaterial(name, pMaterial));
	return m_mMaterial[name];
}

void CResourceManager::CreateAnimater(string path, string animaterName) {

}

UINT CResourceManager::CreateMultiMesh(string path, string name) {
	wstring ws{ L"" };
	ws.assign(path.cbegin(), path.cend());
	wstring extention{ PathFindExtension(ws.c_str()) };

	if (L".FBX" == extention || L".fbx" == extention) {
		return CreateFBXResource(path, name);
	}
	else if (L".GJM" == extention || L".gjm" == extention || L".txt" == extention) {
		return CreateGJMResource(path, name);
	}
}
UINT CResourceManager::CreateGJMResource(string path, string name){
	IMPORTER->Begin(path);
	char pName[20];

	bool bHasAnimation = IMPORTER->ReadBool();

	shared_ptr<CMesh> pMesh;
	UINT nMeshCnt = IMPORTER->ReadUINT();
	for (UINT i = 0; i < nMeshCnt; ++i) {
		sprintf(pName, "%s%d", name.c_str(), i);
		pMesh = CFileBasedMesh::CreateMeshFromGJMFile(i, bHasAnimation);
		m_mMesh.insert(pairMesh(pName, pMesh));
	}
	if (false == bHasAnimation) return nMeshCnt;

	//animater
	sprintf(pName, "%s", name.c_str());
	shared_ptr<CAnimater> pAnimater = CAnimater::CreateAnimaterFromGJMFile();
	m_mAnimater.insert(pairAnimater(pName, pAnimater));

	//animation info
	int animationCnt = IMPORTER->ReadInt();
	for (int i = 0; i < animationCnt; ++i) {
		CAnimationInfo* pAnimationInfo = CAnimationInfo::CreateAnimationInfoFromGJMFile(pAnimater);
	}//end animation info for

	IMPORTER->End();
	return nMeshCnt;
}
UINT CResourceManager::CreateFBXResource(string path, string name){
	string sPath{ "" };
	sPath.assign(path.cbegin(), path.cend());

	//multi mesh data load
	FBXIMPORTER->Begin(sPath);
	char pName[20];

	shared_ptr<CFileBasedMesh> pFBXMesh;
	//	int i = FBXIMPORTER->GetMeshCnt();
	if (FBXIMPORTER->GetHasAnimation()) {

		for (UINT i = 0; i < FBXIMPORTER->GetMeshCnt(); ++i) {
			sprintf(pName, "%s%d", name.c_str(), i);
			pFBXMesh = CFileBasedMesh::CreateMeshFromFBXFile(i);
			m_mMesh.insert(pairMesh(pName, pFBXMesh));
		}

		sprintf(pName, "%s", name.c_str());
		shared_ptr<CAnimater> pAnimater = CAnimater::CreateAnimaterFromFBXFile();
		CAnimationInfo* pAnimationInfo = CAnimationInfo::CreateAnimationInfoFromFBXFile(pAnimater);
		m_mAnimater.insert(pairAnimater(pName, pAnimater));
	}
	else {
		for (UINT j = 0; j < FBXIMPORTER->GetMeshCnt(); ++j) {
			sprintf(pName, "%s%d", name.c_str(), j);
			pFBXMesh = CFileBasedMesh::CreateMeshFromFBXFile(j);
			m_mMesh.insert(pairMesh(pName, pFBXMesh));
		}
	}

	int meshCnt = FBXIMPORTER->GetMeshCnt();
	FBXIMPORTER->End();
	return meshCnt;
}

//Release
//Release
void CResourceManager::ReleaseTextures(){
	for (auto data : m_mSampler) {
		if(data.second)data.second->End();
	}
	for (auto data : m_mTexture) {
		if (data.second)data.second->End();
	}
	m_mTexture.clear();
}

void CResourceManager::ReleaseRenderShaders(){
	for (auto data : m_mRenderShader) {
		if (data.second)data.second->End();
	}
	m_mRenderShader.clear();
}

void CResourceManager::ReleaseMeshs(){
	for (auto data : m_mMesh) {
		if (data.second)data.second->End();
	}
	m_mMesh.clear();
}

void CResourceManager::ReleaseMesh(string name){
	map<string, shared_ptr<CMesh>> ::iterator iter = m_mMesh.find(name);
	(iter->second)->End();
	//delete iter->second.get();
	m_mMesh.erase(iter);
}

void CResourceManager::ReleaseAnimater(string name){
	map<string, shared_ptr<CAnimater>> ::iterator iter = m_mAnimater.find(name);
	(iter->second)->End();
	m_mAnimater.erase(iter);
}

void CResourceManager::ReleaseBuffers(){
	for (auto data : m_mBuffer) {
		if (data.second)data.second->End();
	}
	m_mBuffer.clear();
}

void CResourceManager::ReleaseGlobalBuffers(){
	for (auto data : m_mGlobalBuffer) {
		if (data.second)data.second->End();
	}
	m_mGlobalBuffer.clear();
}

void CResourceManager::ReleaseMaterials(){
	for (auto data : m_mMaterial) {
		if (data.second)data.second->End();
	}
	m_mMaterial.clear();
}

void CResourceManager::ReleaseAnimaters(){
	for (auto data : m_mAnimater) {
		if (data.second)data.second->End();
	}
	m_mAnimater.clear();
}
//Release
//Release
CResourceManager::CResourceManager() :CSingleTonBase<CResourceManager>("resourcemanager") {

}

CResourceManager::~CResourceManager() {

}
