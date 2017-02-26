#include "stdafx.h"
#include "SplattingInfo.h"

void TW_CALL SplattingDeleteButtonCallback(void* clientData) {
	CSplattingInfo* pData = (CSplattingInfo*)clientData;
	pData->GetSplattingManager()->RemoveSplattingInfoByIndex(pData->GetIndex());
}
void TW_CALL SplattingDetailTextureSelectCallback(void* clientData) {
	LoadFileStructSP* pData = (LoadFileStructSP*)clientData;
	pData->m_pSplatting->SetDetailTextuePath(pData->path.c_str());
}
void CSplattingInfo::Begin(){
	SetIndex(m_pSplattingInfoManager->GetSplattingInfos().size());

	m_pBlendInfo = new Pixel24[BLEND_DATA_NUM];
	ZeroMemory(m_pBlendInfo, sizeof(Pixel24) * BLEND_DATA_NUM);
	//blend data init
	
	EXPORTER->MakeBitmap24(m_pBlendInfoTexturePath.c_str(), m_pBlendInfo, BLEND_WIDTH, BLEND_HEIGHT);
	m_pBlendInfo = IMPORTER->ReadBitmap24(m_pBlendInfoTexturePath.c_str());
	//m_pBlendInfoTexture = CTexture::CreateTexture(L"../slide.bmp", RESOURCEMGR->GetSampler("DEFAULT"), 2, BIND_PS);
}

bool CSplattingInfo::End(){
	m_vLoadFileStruct.clear();
	if (m_pBlendInfo) delete[] m_pBlendInfo; 
	m_pBlendInfo = nullptr;
	return true;
}

void CSplattingInfo::SetShaderState(){
	
}

void CSplattingInfo::CleanShaderState(){
	
}

void CSplattingInfo::UpdateShaderState(){
	//height map data init
	EXPORTER->MakeBitmap24(m_pBlendInfoTexturePath.c_str() , m_pBlendInfo, BLEND_WIDTH, BLEND_HEIGHT);
}

CSplattingInfo * CSplattingInfo::CreateSplattingInfo(CSplattingInfoManager* pSplattingManager, const WCHAR * pDetailTextureName){
	CSplattingInfo* pSplattingInfo = new CSplattingInfo;

	pSplattingInfo->SetSplattingManager(pSplattingManager);
	pSplattingInfo->SetDetailTextuePath(pDetailTextureName);
	
	pSplattingInfo->Begin();
	return pSplattingInfo;
}

void CSplattingInfo::SetIndex(UINT index){
	m_nIndex = index; 
	WCHAR path[256];
	wsprintf(path, L"../outputdata/SplattingBlendInfo/BlendInfo%d.bmp", m_nIndex);
	SetBlendInfoTextuePath(path);
}

void CSplattingInfo::CreateControllUI(){
	const char* barName = "SplattingControll";
	TWBARMGR->DeleteBar(barName);
	
	TWBARMGR->AddBar(barName);
	//set param
	TWBARMGR->SetBarSize(barName, 250, 250);
	TWBARMGR->SetBarPosition(barName, 750, 300);
	TWBARMGR->SetBarColor(barName, 255, 0, 255);
	TWBARMGR->SetBarContained(barName, true);
	TWBARMGR->SetBarMovable(barName, false);
	TWBARMGR->SetBarResizable(barName, false);
	//set param

	TWBARMGR->AddButtonCB(barName, "SplattingControll", "DeleteSplatting", SplattingDeleteButtonCallback, this);
	vector<wstring> vFile;
	DIRECTORYFINDER->GetFiles(vFile, L"../../Assets/Detail_Texture", true, true, L".jpg");
	DIRECTORYFINDER->GetFiles(vFile, L"../../Assets/Detail_Texture", true, true, L".JPG");
	DIRECTORYFINDER->GetFiles(vFile, L"../../Assets/Detail_Texture", true, true, L".png");
	DIRECTORYFINDER->GetFiles(vFile, L"../../Assets/Detail_Texture", true, true, L".PNG");

	//const char* groupName = "File";
	char menuName[64];
	int cnt{ 0 };
	m_vLoadFileStruct.resize(vFile.size());
	for (auto data : vFile) {
		//file directory store;
		data = DIRECTORYFINDER->ReplaceString(data, L"\\", L"/");
		m_vLoadFileStruct[cnt] = LoadFileStructSP{ this, data };

		//menu name = file name
		string menuNameString{ "" };
		menuNameString.assign(data.cbegin(), data.cend());
		sprintf(menuName, "%s", menuNameString.c_str());

		//group name = directory name
		data = DIRECTORYFINDER->ReplaceString(data, L"/", L"\\");
		LPWSTR str = (LPWSTR)data.c_str();
		PathRemoveFileSpec(str);

		wstring wGroupName{ str };
		string groupName;
		groupName.assign(wGroupName.cbegin(), wGroupName.cend());
		TWBARMGR->AddButtonCB(barName, groupName.c_str(), menuName, SplattingDetailTextureSelectCallback, &m_vLoadFileStruct[cnt]);
		cnt++;
	}
}

CSplattingInfo::CSplattingInfo() : DXObject("splattinginfo")
{
}

CSplattingInfo::~CSplattingInfo()
{
}
