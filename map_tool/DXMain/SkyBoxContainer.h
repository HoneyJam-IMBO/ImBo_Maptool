#pragma once
#include "Object.h"
#include "SpaceContainer.h"
#include "SkyBox.h"

class CSkyBoxContainer : public CObject {
public:
	void Begin();
	bool End();

	void Render(shared_ptr<CCamera> pCamera);
	void Update(shared_ptr<CCamera> pCamera, float fTimeElapsed);

	void SetSkyBox(CSkyBox* pSkyBox) { m_pSkyBox = pSkyBox; }
	CSkyBox* GetSkyBox() { return m_pSkyBox; }
	//void SetDirectionalLight(CDirectionalLight* pDirectionalLight) { m_pDirectionalLight = pDirectionalLight; }
	//CDirectionalLight* GetDirectionalLight() { return m_pDirectionalLight; }

	static CSkyBoxContainer* CreateSkyBoxContainer(LPCTSTR pSkyBoxName, UINT textureIndex, CSpaceContainer* pSpaceContainer);
	void SetSkyBoxName(wstring wsName) { m_wsSkyBoxName = wsName; }
	void CreateSkyBoxTexture(UINT index);
	void SetSpaceContainer(CSpaceContainer* pSpaceContainer) { m_pSpaceContainer = pSpaceContainer; }

private:
	bool m_bActive{ true };
	wstring m_wsSkyBoxName;
	ID3D11DepthStencilState* m_pd3dDepthStencilState{ nullptr };
	ID3D11DepthStencilState* m_pd3dTempDepthStencilState{ nullptr };
	UINT m_TempStencil{ 0 };

	CRenderContainer* m_pSkyboxContainer{ nullptr };
	CSpaceContainer* m_pSpaceContainer{ nullptr };
	//global object
	CSkyBox* m_pSkyBox{ nullptr };
	//CDirectionalLight* m_pDirectionalLight{ nullptr };
	//global object

	//skybox texture
	shared_ptr<CTexture> m_ptxtSkyBox{ nullptr };
public:
	CSkyBoxContainer();
	~CSkyBoxContainer();
};