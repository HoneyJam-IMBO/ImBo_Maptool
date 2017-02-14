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

public:
	CObjectRenderer();
	virtual ~CObjectRenderer();
};