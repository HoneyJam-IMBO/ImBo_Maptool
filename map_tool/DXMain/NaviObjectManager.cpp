#include "stdafx.h"
#include "NaviObjectManager.h"

vector<CNaviObject*> CNaviObjectManager::m_vNaviObject;

vector<CNaviVertex*> CNaviObjectManager::m_vpNaviVertex;
CBuffer* CNaviObjectManager::m_pGSNaviMeshVertexBuffer;
CBuffer* CNaviObjectManager::m_pNaviMeshInstancingBuffer;

CMesh* CNaviObjectManager::m_pNaviMesh;
CRenderShader* CNaviObjectManager::m_pNaviObjectShader;

//int CNaviObjectManager::m_ControlVertexs[3];//index ��
CNaviVertex* CNaviObjectManager::m_ControlVertexs[3];//index ��
int CNaviObjectManager::m_ControlVertexCount = 0;

bool CNaviObjectManager::m_bAutoCreateNaviObject = true;

bool CNaviObjectManager::Begin(){
	m_vNaviObject.clear();
	m_vpNaviVertex.clear();

	m_pGSNaviMeshVertexBuffer = RESOURCEMGR->CreateConstantBuffer("GSNavyMeshVertexBuffer", 1000, sizeof(XMFLOAT4), 1, BIND_GS);
	m_pNaviMeshInstancingBuffer = RESOURCEMGR->CreateInstancingBuffer("NavyMeshInstancingBuffer", 1000, sizeof(NaviMeshIndexData));

	//����ƺ� mesh = boundingbox meshh
	m_pNaviMesh = RESOURCEMGR->GetMesh("NaviMesh", 0);
	m_pNaviMesh->AddInstancingBuffer(m_pNaviMeshInstancingBuffer);

	m_pNaviObjectShader = RESOURCEMGR->CreateRenderShader("NaviObject", L"NaviObject",
		IE_INSUINT_C_A,
		BIND_VS | BIND_GS | BIND_PS);
	
//	m_vpNaviVertex.push_back(CNaviVertex::CreateNaviVertex(XMFLOAT3(0, 20, 0)));
//	m_vpNaviVertex.push_back(CNaviVertex::CreateNaviVertex(XMFLOAT3(0, 20, 100)));
//	m_vpNaviVertex.push_back(CNaviVertex::CreateNaviVertex(XMFLOAT3(100, 20, 0)));
//	m_vpNaviVertex.push_back(CNaviVertex::CreateNaviVertex(XMFLOAT3(100, 20, 100)));
	
	return false;
}

bool CNaviObjectManager::End(){
	//navi vertex�� navi object�� ���� ���̰� �Ǿ� ���ΰ� ���θ� delete�ϸ� �и� ���δ�. manager�� å������ delete�ϵ��� ����
	for (auto pObject : m_vNaviObject) {
		pObject->End();
		delete pObject;
	}
	m_vNaviObject.clear();

	for (auto pVertex : m_vpNaviVertex) {
		pVertex->End();//�ƹ��ŵ� ���ϴ� ������ �׳�
		delete pVertex;
	}
	m_vpNaviVertex.clear();
	//navi vertex�� navi object�� ���� ���̰� �Ǿ� ���ΰ� ���θ� delete�ϸ� �и� ���δ�. manager�� å������ delete�ϵ��� ����

	m_pGSNaviMeshVertexBuffer = nullptr;
	m_pNaviMeshInstancingBuffer = nullptr;

	m_pNaviMesh = nullptr;
	m_pNaviObjectShader = nullptr;

	return true;
}

void CNaviObjectManager::Render(){
	{
		//vertex buffer �� ������ bounding box�� render
		for (auto pVertex : m_vpNaviVertex) {
			BoundingOrientedBox obb;
			obb.Center = pVertex->GetPosition();
			obb.Extents = XMFLOAT3(5.f, 5.f, 5.f);
			DEBUGER->RegistOBB(obb);
		}

	}

	{
		//update �߿� ui������ update����
		//���� control���� navivertex�� ��ġ ����, ���� ���
		//bool������ mode, show��� ó�� ���߿�
		if (GLOBALVALUEMGR->GetToolMode() != TOOL_MODE_NAVIMESH) {
			ClearUI();
			return;
		}
	}
	
	{
		XMFLOAT4* pData = (XMFLOAT4*)m_pGSNaviMeshVertexBuffer->Map();
		int i = 0;
		for (auto pVertex : m_vpNaviVertex) {
			XMFLOAT3 vertex = pVertex->GetPosition();
			pData[i++] = XMFLOAT4(vertex.x, vertex.y, vertex.z, 1);
		}
		
		m_pGSNaviMeshVertexBuffer->Unmap();
		m_pGSNaviMeshVertexBuffer->SetShaderState();
	}
	{
		NaviMeshIndexData* pData = (NaviMeshIndexData*)m_pNaviMeshInstancingBuffer->Map();
		int index = 0;
		for (auto pNaviObject : m_vNaviObject) {
			
			pData->m_pIndices[index++] = pNaviObject->GetNaviVertices()[0]->GetIndex();
			pData->m_pIndices[index++] = pNaviObject->GetNaviVertices()[1]->GetIndex();
			pData->m_pIndices[index++] = pNaviObject->GetNaviVertices()[2]->GetIndex();
		}
		m_pNaviMeshInstancingBuffer->Unmap();
		//m_pNaviMeshInstancingBuffer->SetShaderState();
	}
	m_pNaviObjectShader->SetShaderState();

	//mesh
	m_pNaviMesh->SetShaderState();
	m_pNaviMesh->Render(m_vNaviObject.size());

	m_pNaviObjectShader->CleanShaderState();
}

void CNaviObjectManager::PickingProc(XMFLOAT3 xmf3PickingPos){
	/*
	0610 ���Ȧ �ٳ�ͼ� �� �� ! 
	- ��ŷ ������ ������ ���� ��ó�̸� ���� �׳༮���� ��ü
	- UIó��! ���� UI�� ��Ʈ�ϴ��� �˰���
	*/
	
	if (m_ControlVertexCount > 2) {//���� control point�� 3���̻��ε� 0 1 2 // 3!
		
		//auto pVertex = m_vpNaviVertex[m_ControlVertexs[2]];//������ control point�� ������ control point��
		auto pVertex = m_ControlVertexs[2];//������ control point�� ������ control point��
		//auto Iter = m_vpNaviVertex.end();
		//Iter--;
		if (pVertex->GetNaviObjectCount() == 0) {//���� ��������� �̰ɷ� �� �� �ִ�. 
			//���� naviObject�� 0���� ũ�� ���� ���� ���̴�. 
			//���λ������� ������ �ȴ�!
			DeleteVertex(pVertex->GetIndex());
		}
		m_ControlVertexCount = 2;
	}

	int index = 0;
	for (auto pVertex : m_vpNaviVertex) {
		XMVECTOR xmvVertexPos;
		XMVECTOR xmvPickingPos;
		xmvVertexPos = XMLoadFloat3(&pVertex->GetPosition());
		xmvPickingPos = XMLoadFloat3(&xmf3PickingPos);
		XMFLOAT4 xmf4Result;
		XMStoreFloat4(&xmf4Result, XMVector3Length(xmvPickingPos - xmvVertexPos));
		if (5.f > xmf4Result.x) {
			break;
		}

		index++;
	}
	
	if (index == m_vpNaviVertex.size()) {
		m_vpNaviVertex.push_back(CNaviVertex::CreateNaviVertex(xmf3PickingPos));//�ϳ� �߰��ϰ� 
		m_vpNaviVertex[index]->SetIndex(index);
		//ResetVertexIndex();//index ������
	}

	//int ControlVertexIndex = MAX(2, m_ControlVertexCount++);
	m_ControlVertexs[m_ControlVertexCount++] = m_vpNaviVertex[index];


	if (m_ControlVertexCount > 2) {
		if (m_bAutoCreateNaviObject) {//���� ���� ��尡 autoCreateNaviobject���
			//NaviObject Create
			CreateNaviObject();
		}
	}
	ClearUI();
	RenderUI();

}

void CNaviObjectManager::ResetVertexIndex(){
	int index = 0;
	for (auto pVertex : m_vpNaviVertex) {
		pVertex->SetIndex(index++);
	}
}

void CNaviObjectManager::DeleteVertex(int index) {
	if (m_ControlVertexCount > 0) {
		for (int i = 0; i < m_ControlVertexCount; ++i) {
			if (m_ControlVertexs[i]->GetIndex() == index) {
				m_ControlVertexCount--;
				break;
			}
		}
	}

	auto Iter = m_vpNaviVertex.begin();
	for (auto pVertex : m_vpNaviVertex) {
		if (pVertex->GetIndex() == index) {
			delete pVertex;
			break;
		}
		Iter++;
	}
	if (Iter == m_vpNaviVertex.end()) return;

	m_vpNaviVertex.erase(Iter);
	
	ResetVertexIndex();//index�� ���⼭�� �����ϴ� ������ vertex�� ���� ������ �����̱� �����̴�.
	ClearUI();
	RenderUI();
}

void CNaviObjectManager::DeleteNaviObject(int index){
	auto Iter = m_vNaviObject.begin();
	for (auto pNaviObejct : m_vNaviObject) {
		if (pNaviObejct->GetNaviObjectID() == index) {
			delete pNaviObejct;
			break;
		}
		Iter++;
	}
	if (Iter == m_vNaviObject.end()) return;
	m_vNaviObject.erase(Iter);

	//ResetVertexIndex();
}

void TW_CALL CreateNaviObjectButtonCallback(void * clientData) {
//	CNaviVertex* pVertex = reinterpret_cast<CNaviVertex*>(clientData);

	CNaviObjectManager::CreateNaviObject();
}
bool CNaviObjectManager::IsValiableIndex(int index){
	if (index < 0 || index > m_vNaviObject.size()) return false;

	return true;
}
XMVECTOR CNaviObjectManager::GetNaviMeshPosition(int index){
	if(false == IsValiableIndex(index)) return XMVECTOR();
	XMVECTOR xmvPos = XMLoadFloat3(&m_vNaviObject[index]->GetNaviVertices()[0]->GetPosition());
	return xmvPos;
}
int CNaviObjectManager::GetValiableIndex(XMVECTOR pos){

	return -1;
}
bool CNaviObjectManager::IsIntersection(float x, float z, int index){
	if (false == IsValiableIndex(index)) return false;

	return m_vNaviObject[index]->IsIntersection(x, z);
}
int CNaviObjectManager::GetIndex(float x, float z, int index){
	if (false == IsValiableIndex(index)) return -1;

	for (auto pNaviObject : m_vNaviObject[index]->GetBorderNaviObjects()) {
		if (pNaviObject->IsIntersection(x, z)) return pNaviObject->GetNaviObjectID();
	}
	return -1;
}
void CNaviObjectManager::LoadData(){

}
void CNaviObjectManager::SaveData(){
	//navi vertex
	int nVertices = m_vpNaviVertex.size();
	EXPORTER->WriteInt(nVertices); EXPORTER->WriteSpace();
	for (auto pVertex : m_vpNaviVertex) {
		XMFLOAT3 xmf3Pos = pVertex->GetPosition();
		EXPORTER->WriteFloat(xmf3Pos.x); EXPORTER->WriteSpace();
		EXPORTER->WriteFloat(xmf3Pos.y); EXPORTER->WriteSpace();
		EXPORTER->WriteFloat(xmf3Pos.z); EXPORTER->WriteSpace();
	}
	EXPORTER->WriteEnter();
	//navi vertex

	//navi object
	int nObject = m_vNaviObject.size();
	EXPORTER->WriteInt(nObject); EXPORTER->WriteSpace();
	for (auto pNaviObject : m_vNaviObject) {
		for (auto pVertex : pNaviObject->GetNaviVertices()) {
			int index = 0;
			index = pVertex->GetIndex();
			EXPORTER->WriteInt(index); EXPORTER->WriteSpace();
		}
	}
	EXPORTER->WriteEnter();
	//navi object
}
void CNaviObjectManager::RenderUI(){
	//control point�� ��ġ ���� 	
	//
	const char* pName = "NaviMeshControlBar";
	TWBARMGR->AddBar(pName);

	TWBARMGR->AddBoolBar(pName, "Button", "AutoMake", &m_bAutoCreateNaviObject);
	TWBARMGR->AddButtonCB(pName, "Button", "CreateNaviObject", CreateNaviObjectButtonCallback, nullptr);
	for (int i = 0; i < m_ControlVertexCount; ++i) {
		//m_vpNaviVertex[m_ControlVertexs[i]]->CreateControlUI();
		m_ControlVertexs[i]->CreateControlUI();
	}

}

void CNaviObjectManager::ClearUI(){
	const char* pName = "NaviMeshControlBar";
	TWBARMGR->DeleteBar(pName);
}

float CNaviObjectManager::GetHeight(XMFLOAT2 pos, int index){
	if (false == IsValiableIndex(index)) return 0.f;

	return m_vNaviObject[index]->GetHeight(pos.x, pos.y);
}

void CNaviObjectManager::CreateNaviObject(){
	if (m_ControlVertexCount < 3) return;
	XMVECTOR p0, p1, p2;
	//int idx0 = m_ControlVertexs[0], idx1 = m_ControlVertexs[1], idx2 = m_ControlVertexs[2];
	
	CNaviVertex* idx0 = m_ControlVertexs[0];
	CNaviVertex* idx1 = m_ControlVertexs[1];
	CNaviVertex* idx2 = m_ControlVertexs[2];
	p0 = XMLoadFloat3(&idx0->GetPosition());
	p1 = XMLoadFloat3(&idx1->GetPosition());
	p2 = XMLoadFloat3(&idx2->GetPosition());


	//012 �� ���̴� index���!
	//
	XMVECTOR v = XMVector3Normalize(XMVector3Cross(p1-p0, p2-p0));
	XMVECTOR look = XMVector3Normalize(UPDATER->GetCamera()->GetLook());
	XMFLOAT4 xmf4DotResult;
	XMStoreFloat4(&xmf4DotResult, XMVector3Dot(v, look));
	if (xmf4DotResult.x >= 0) {
		m_ControlVertexs[0] = idx2;
		m_ControlVertexs[1] = idx1;
		m_ControlVertexs[2] = idx0;
	}

	//���� ���� �ϼ��Ǹ� �׳� �ڵ����� �����. 
	CNaviObject* pNaviObject = CNaviObject::CreateNaviObject(m_ControlVertexs[0], m_ControlVertexs[1], m_ControlVertexs[2]);
	m_vNaviObject.push_back(pNaviObject);

	//���� ������ ��� vertex���� ��ȸ�ϸ鼭 ���� ���۵� naviObject�� ���� naviObejct�� �����Ѵ�.
	vector<CNaviObject*> tmpNaviObject;
	for (int i = 0; i < 3; ++i) {
		for (auto pNaviObject : m_ControlVertexs[i]->GetNaviObjects()) {//vertex�� naviobject�� id��
			//tmpNaviObject vector�� �ֳ� �˻�
			int count = 0;
			for (auto pTmpNaviObject : tmpNaviObject) {
				if (pTmpNaviObject->GetNaviObjectID() == pNaviObject->GetNaviObjectID())
					break;
				count++;
			}
			if(count == tmpNaviObject.size()) tmpNaviObject.push_back(pNaviObject);//���� ���ٸ� ���� naviObject�� �����Ѵ�.
		}
	}
	//���� naviObejct���� ���θ� ��Ͻ�Ų��.
	for (auto pBorderNaviObject : tmpNaviObject) {
		pBorderNaviObject->AddBorderNaviObject(pNaviObject);
		pNaviObject->AddBorderNaviObject(pBorderNaviObject);
	}
	//control ui ����
	const char* pName = "NaviMeshControlBar";
	char* pGroupName = "NaviVertexControl";
	char pMenuName[64];
	
	

	//���������� �� vertex�鿡�� ������� object�� ����Ѵ�.
	for (int i = 0; i < 3; ++i) {
		m_ControlVertexs[i]->AddNaviObject(pNaviObject);

		int index = m_ControlVertexs[i]->GetIndex();
		sprintf(pMenuName, "Vertex%d", index);
		TWBARMGR->DeleteVar(pName, pMenuName);
	}
	m_ControlVertexCount = 0;
	
}

CNaviObjectManager::CNaviObjectManager()
{
}

CNaviObjectManager::~CNaviObjectManager()
{
}
