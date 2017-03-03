#pragma once
#include "Object.h"

class CSkyBoxContainer : public CObject {
public:
	void Begin();
	bool End();

	void Render(shared_ptr<CCamera> pCamera);
	void Update(shared_ptr<CCamera> pCamera, float fTimeElapsed);

	void SetSkyBox(CSkyBox* pSkyBox) { m_pSkyBox = pSkyBox; }
	CSkyBox* GetSkyBox() { return m_pSkyBox; }
	void SetDirectionalLight(CDirectionalLight* pDirectionalLight) { m_pDirectionalLight = pDirectionalLight; }
	CDirectionalLight* GetDirectionalLight() { return m_pDirectionalLight; }

	static CSkyBoxContainer* CreateSkyBoxContainer();
private:
	ID3D11DepthStencilState* m_pd3dDepthStencilState{ nullptr };
	ID3D11DepthStencilState* m_pd3dTempDepthStencilState{ nullptr };
	UINT m_TempStencil{ 0 };

	CRenderContainer* m_pSkyboxContainer{ nullptr };
	
	//global object
	CSkyBox* m_pSkyBox{ nullptr };
	CDirectionalLight* m_pDirectionalLight{ nullptr };
	//global object

public:
	CSkyBoxContainer();
	~CSkyBoxContainer();
};