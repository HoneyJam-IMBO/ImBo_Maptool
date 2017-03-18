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

	virtual void RegistToContainer();
	void RegistToContainer2();
	virtual void PickingProc() {};
	virtual bool CheckPickObject(XMVECTOR xmvWorldCameraStartPos, XMVECTOR xmvRayDir, float& distance);
	static CTerrain* CreateTerrain(CTerrainContainer* pTerrainContainer, int x, int y);
private:
	CTerrainContainer* m_pTerrainContainer{ nullptr };

	bool m_bRender{ false };
public:
	CTerrain();
	~CTerrain();
};

