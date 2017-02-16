#include "stdafx.h"
#include "Terrain.h"
#include "TerrainContainer.h"

bool CTerrain::Begin() {
	//object_id set
	m_objectID = object_id::OBJECT_TERRAIN;
	CGameObject::Begin();
	return true;
}
bool CTerrain::End() {

	return CGameObject::End();
}

void CTerrain::SetTerrainContainer(CTerrainContainer * pTerrainContainer){
	m_pTerrainContainer = pTerrainContainer;
}

bool CTerrain::CheckPickObject(XMVECTOR xmvWorldCameraStartPos, XMVECTOR xmvRayDir, float & distance){
	BoundingBox BoundingBox;
	GetMainBoundingBox(BoundingBox);
	bool b = BoundingBox.Intersects(xmvWorldCameraStartPos, xmvRayDir, distance);
	XMFLOAT4 xmf3TerrainPickPos;
	if (b) {
		XMStoreFloat4(&xmf3TerrainPickPos, xmvWorldCameraStartPos + xmvRayDir * distance);
		m_pTerrainContainer->SetPicpos(xmf3TerrainPickPos.x, xmf3TerrainPickPos.z);
	}
	else {

	}
	return b;
}

CTerrain::CTerrain() : CGameObject("terrain", tag::TAG_TERRAIN) {

}
CTerrain::~CTerrain() {

}