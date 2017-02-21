#pragma once
#include "DXObject.h"
#include "Texture.h"

#define BLEND_DATA_NUM 256*256
#define BLEND_WIDTH 256
#define BLEND_HEIGHT 256


class CSplattingInfo : public DXObject{
public:
	void Begin();
	virtual bool End();
	virtual void SetShaderState();
	virtual void CleanShaderState();
	virtual void UpdateShaderState();//blend info ÆíÁý

	void SetBlendInfoTexture(shared_ptr<CTexture> pTexture) { m_pBlendInfoTexture = pTexture; }
	void SetBaseTexture(shared_ptr<CTexture> pTexture) { m_pBaseTexture = pTexture; }
	void SetDetailTexture(shared_ptr<CTexture> pTexture) { m_pDetailTexture = pTexture; }
	Pixel24* GetBlendInfo() { return m_pBlendInfo; }
	static CSplattingInfo* CreateSplattingInfo(WCHAR* pBaseTextureName, WCHAR* pDetailTextureName);

private:
	shared_ptr<CTexture> m_pBlendInfoTexture{ nullptr };
	shared_ptr<CTexture> m_pBaseTexture{ nullptr };
	shared_ptr<CTexture> m_pDetailTexture{ nullptr };

	Pixel24* m_pBlendInfo{ nullptr };
public:
	CSplattingInfo();
	~CSplattingInfo();
};