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
private:
	//rendercontainer map!
	mapRenderContainer m_mRenderContainer;
	
	ID3D11DepthStencilState* m_pd3dDepthStencilState{ nullptr };
	ID3D11DepthStencilState* m_pd3dTempDepthStencilState{ nullptr };
	UINT m_TempStencil{ 0 };

	ID3D11RasterizerState* m_pd3dSpaceRSState{ nullptr };
	ID3D11RasterizerState* m_pd3dTempRSState{ nullptr };



public:
	CObjectRenderer();
	virtual ~CObjectRenderer();
};