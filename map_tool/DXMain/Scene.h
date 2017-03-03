#pragma once

#include "DXObject.h"
#include "GameObject.h"

class CScene{

public:
	virtual bool Begin() { return true; };
	virtual bool End() { return true; };

	virtual void Animate(float fTimeElapsed) = 0;

	virtual void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) = 0;
	virtual void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) = 0;
	virtual void ProcessInput(float fTimeElapsed) = 0;

	virtual CGameObject* PickObjectPointedByCursor(int xClient, int yClient) = 0;

	//ssao
	void SetSSAORarius(float fSSAORadius) { m_fSSAORadius = fSSAORadius; };
	void SetSSAOOffsetRadius(float SSAOOffsetRadius) { m_fSSAOOffsetRadius = SSAOOffsetRadius; }
	//bloom
	void SetBLOOMThreshold(float BLOOMThreshold) { m_fBLOOMThreshold = BLOOMThreshold; }
	void SetBLOOMMiddleGrey(float BLOOMMiddleGrey) { m_fBLOOMMiddleGrey = BLOOMMiddleGrey; }
	void SetBLOOMWhith(float BLOOMWhith) { m_fBLOOMWhith = BLOOMWhith; }
	void SetBLOOMScale(float BLOOMScale) { m_fBLOOMScale = BLOOMScale; }
	//sslr
	void SetSSLROnOff(bool SSLROnOff) { m_bSSLROnOff = SSLROnOff; }
	void SetSSLROffsetSunPos(float SSLROffsetSunPos) { m_fSSLROffsetSunPos = SSLROffsetSunPos; }
	void SetSSLRMaxSunDist(float SSLRMaxSunDist) { m_fSSLRMaxSunDist = SSLRMaxSunDist; }
	void SetSSLRInitDecay(float SSLRInitDecay) { m_fSSLRInitDecay = SSLRInitDecay; }
	void SetSSLRDistDecay(float SSLRDistDecay) { m_fSSLRDistDecay = SSLRDistDecay; }
	void SetSSLRMaxDeltaLen(float SSLRMaxDeltaLen) { m_fSSLRMaxDeltaLen = SSLRMaxDeltaLen; }

	static CScene* CreateScene(wstring path);
protected:
	string m_sName;

	//ssao
	float m_fSSAORadius{ 0.f };
	float m_fSSAOOffsetRadius{ 0.f };
	//bloom
	float m_fBLOOMThreshold{ 2.f };
	float m_fBLOOMMiddleGrey{ 0.01f };
	float m_fBLOOMWhith{ 2.0f };
	float m_fBLOOMScale{ 2.0f };
	//sslr
	bool m_bSSLROnOff;
	float m_fSSLROffsetSunPos{ -200.f };
	float m_fSSLRMaxSunDist{ 1000.f };
	float m_fSSLRInitDecay{ 0.05f };
	float m_fSSLRDistDecay{ 0.05f };
	float m_fSSLRMaxDeltaLen{ 0.05f };

private:

public:
	CScene::CScene(string name) { m_sName = name; }
	~CScene() {}
};