#pragma once

#include "RenderContainer.h"

class CDebugRenderContainer : public CRenderContainer{
public:

	//---------------------------dxobject---------------------------------
	virtual void UpdateShaderState(shared_ptr<CCamera> pCamera);
	//---------------------------dxobject---------------------------------

	//--------------------------container---------------------------------
	virtual void UpdateGlobalBuffer();


public:
	CDebugRenderContainer();
	virtual ~CDebugRenderContainer();

};

typedef map<object_id, CRenderContainer*> mapRenderContainer;
typedef pair<object_id, CRenderContainer*> pairRenderContainer;
