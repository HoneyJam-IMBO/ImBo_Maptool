#pragma once
#include "DXObject.h"
#include "Stemp.h"

struct TERRAIN_PICPOS_RENDER_INFO {
	XMFLOAT2 PickPos{ 0.f, 0.f };//pick pos/ space_size;
	float Extent{ 50.f / SPACE_SIZE };//¿øÀÇ r/space_size;
	UINT ToolMode;
};

class CStempManager : public DXObject {
public:
	void Begin();
	virtual bool End();

	virtual void SetShaderState();
	virtual void CleanShaderState();
	virtual void UpdateShaderState();

	void IncreaseTerrain(Pixel24* pHeightMapData);
	void DecreaseTerrain(Pixel24* pHeightMapData);
	void SetTerrain(Pixel24* pHeightMapData);

	void SetPickPos(XMFLOAT2 xmf2Pos) { m_pPicposRenderInfo->PickPos = xmf2Pos; }
	XMFLOAT2 GetPickPos() { return m_pPicposRenderInfo->PickPos; }
	void SetExtent(float extent) { m_pPicposRenderInfo->Extent = (extent / SPACE_SIZE); }
	float GetExtent() { return m_pPicposRenderInfo->Extent; }
	TERRAIN_PICPOS_RENDER_INFO* GetTerrainPickPosRenderInfo() { return m_pPicposRenderInfo; }
	vector<CStemp*>& GetStemps() { return m_vStemp; }

	void SetCurStempIndex(int index);
	void CreateStemp(wstring name);
private:
	float m_fExtent{ 50.f };
	TERRAIN_PICPOS_RENDER_INFO* m_pPicposRenderInfo{ nullptr };
	shared_ptr<CBuffer> m_pPicposRenderInfoBuffer{ nullptr };

	vector<CStemp*> m_vStemp;
	float m_nCurStemp{ 0.f };
public:
	CStempManager();
	~CStempManager();
};