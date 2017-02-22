#pragma once
#include "DXObject.h"
#include "SplattingInfo.h"
#include "Texture.h"
#include "StempManager.h"

#define MAX_SPLATTINGINFO_NUM 10

class CSplattingInfoManager {
public:
	void Begin();
	virtual bool End();

	virtual void SetShaderState();
	virtual void CleanShaderState();
	virtual void UpdateShaderState();

	UINT GetCurIndex() { return m_nCurIndex; }
	void SetCurIndex(UINT index) { m_nCurIndex = index; }
	vector<CSplattingInfo*>& GetSplattingInfos() { return m_vSplattinfInfo; }

	void IncreaseBlendinginfo(CStempManager* pStempManager);
	void DecreaseBlendinginfo(CStempManager* pStempManager);
	void SetBlendinginfo(CStempManager* pStempManager);

	void RemoveSplattingInfoByIndex(UINT index);
	void CreateSplattingInfo(WCHAR* pBlendInfoTextureName, WCHAR* pDetailTextureName);

private:
	UINT m_nCurIndex{ 0 };
	vector<CSplattingInfo*> m_vSplattinfInfo;

	shared_ptr<CTexture> m_pDetailTextures{ nullptr };
	shared_ptr<CTexture> m_pBlendInfoTextures{ nullptr };

public:
	CSplattingInfoManager();
	~CSplattingInfoManager();
};