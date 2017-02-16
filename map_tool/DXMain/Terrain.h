#pragma once

#include "GameObject.h"

class CTerrainContainer;

class CTerrain : public CGameObject {

public:
	//----------------------------dxobject-----------------------------
	bool Begin();
	virtual bool End();
	//----------------------------dxobject-----------------------------
	void SetTerrainContainer(CTerrainContainer* pTerrainContainer);
	virtual void PickingProc() {};
	virtual bool CheckPickObject(XMVECTOR xmvWorldCameraStartPos, XMVECTOR xmvRayDir, float& distance);
private:
	CTerrainContainer* m_pTerrainContainer{ nullptr };

public:
	CTerrain();
	~CTerrain();
};

