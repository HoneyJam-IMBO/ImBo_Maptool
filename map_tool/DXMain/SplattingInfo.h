#pragma once
#include "DXObject.h"
#include "Texture.h"

#define BLEND_DATA_NUM 256*256
#define BLEND_WIDTH 256
#define BLEND_HEIGHT 256

class CSplattingInfoManager;

class CSplattingInfo : public DXObject{
public:
	void Begin();
	virtual bool End();
	virtual void SetShaderState();
	virtual void CleanShaderState();
	virtual void UpdateShaderState();//blend info ÆíÁý

	//void SetBlendInfoTexture(shared_ptr<CTexture> pTexture) { m_pBlendInfoTexture = pTexture; }
	//ID3D11ShaderResourceView* GetBlendInfoSRV() { return m_pBlendInfoTexture->GetShaderResourceView(); }
	//void SetDetailTexture(shared_ptr<CTexture> pTexture) { m_pDetailTexture = pTexture; }
	//ID3D11ShaderResourceView* GetDetailSRV() { return m_pDetailTexture->GetShaderResourceView(); }
	void SetBlendInfoTextuePath(const WCHAR* path) { m_pBlendInfoTexturePath = path; }
	const WCHAR* GetBlendInfoTexturePath() { return m_pBlendInfoTexturePath; }
	void SetDetailTextuePath(const WCHAR* path) { m_pDetailTexturePath = path; }
	const WCHAR* GetDetailTexturePath() { return m_pDetailTexturePath; }
	Pixel24* GetBlendInfo() { return m_pBlendInfo; }
	static CSplattingInfo* CreateSplattingInfo(CSplattingInfoManager* pSplattingManager, const WCHAR* pBlendInfoTextureName, const WCHAR* pDetailTextureName);

	void SetSplattingManager(CSplattingInfoManager* pSplattingManager) { m_pSplattingInfoManager = pSplattingManager; }
	CSplattingInfoManager* GetSplattingManager() { m_pSplattingInfoManager; }
	void SetIndex(UINT index) { m_nIndex = index; }
	UINT GetIndex() { return m_nIndex; }

	void IncreaseBlendinginfo(CStempManager* pStempManager);
	void DecreaseBlendinginfo(CStempManager* pStempManager);
	void SetBlendinginfo(CStempManager* pStempManager);
private:
	//shared_ptr<CTexture> m_pBlendInfoTexture{ nullptr };
	//shared_ptr<CTexture> m_pDetailTexture{ nullptr };
	const WCHAR* m_pBlendInfoTexturePath{ nullptr };
	const WCHAR* m_pDetailTexturePath{ nullptr };

	CSplattingInfoManager* m_pSplattingInfoManager{ nullptr };

	Pixel24* m_pBlendInfo{ nullptr };
	UINT m_nIndex{ 0 };
public:
	CSplattingInfo();
	~CSplattingInfo();
};