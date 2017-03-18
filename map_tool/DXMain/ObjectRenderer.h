#pragma once
#include "DXObject.h"
#include "Camera.h"
#include "RenderContainer.h"

class CObjectRenderer : public DXObject{
public:
	bool Begin();
	virtual bool End();

	virtual void SetShaderState();
	virtual void CleanShaderState();

	virtual void UpdateShaderState();

	void Excute(shared_ptr<CCamera> pCamera);

	CRenderContainer* GetSkyBoxRenderContainer() { return m_mRenderContainer[object_id::OBJECT_SKYBOX]; }
	CRenderContainer* GetTerrainRenderContainer() { return m_mRenderContainer[object_id::OBJECT_TERRAIN]; }
private:
	ID3D11DepthStencilState* m_pd3dDepthStencilState{ nullptr };
	ID3D11DepthStencilState* m_pd3dTempDepthStencilState{ nullptr };
	UINT m_TempStencil{ 0 };

	//rendercontainer map!
	mapRenderContainer m_mRenderContainer;
	
public:
	CObjectRenderer();
	virtual ~CObjectRenderer();
};