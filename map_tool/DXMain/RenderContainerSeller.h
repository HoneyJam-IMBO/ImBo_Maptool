#pragma once
//#include "Camera.h"
#include "SingleTon.h"

#include "RenderContainer.h"
#include "DebugRenderContainer.h"

class CRenderContainerSeller : public CSingleTonBase<CRenderContainerSeller>{
public:
	bool Begin();
	bool End();

	CRenderContainer* GetRenderContainer(object_id objectid);

	//void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
private:
	//이게 진짜 rendercontainer
	mapRenderContainer m_mRenderContainer;

public:
	CRenderContainerSeller() : CSingleTonBase<CRenderContainerSeller>("inputmanagersingleton") {}

};
