#include "stdafx.h"
#include "FileBasedMesh.h"

static bool gVerbose = true;

bool CFileBasedMesh::End() {
	if(m_pNormals) delete[] m_pNormals;
	if(m_pUVs) delete[] m_pUVs;
	//animation
	if(m_pxmf4BoneIndex) delete[] m_pxmf4BoneIndex;
	if(m_pxmf3Weight) delete[] m_pxmf3Weight;

	return CMesh::End();
}

shared_ptr<CFileBasedMesh> CFileBasedMesh::CreateMesh(wstring path, UINT index, bool bHasAnimation){
	wstring extention{ PathFindExtension(path.c_str()) };
	if (L".FBX" == extention || L".fbx" == extention) {
		return CFileBasedMesh::CreateMeshFromFBXFile(index, bHasAnimation);
	}
	else if (L".GJM" == extention || L".gjm" == extention) {
		return CFileBasedMesh::CreateMeshFromGJMFile(index, bHasAnimation);
	}
}

shared_ptr<CFileBasedMesh> CFileBasedMesh::CreateMeshFromFBXFile(UINT index, bool bHasAnimation){
	shared_ptr<CFileBasedMesh> pFileBasedMesh;
	pFileBasedMesh = make_shared<CFileBasedMesh>();
	//set mesh name, index
	pFileBasedMesh->SetMeshIndex(index);
	
	//set vertex buffer info
	if (FBXIMPORTER->GetHasAnimation()) {
		//1. ��ü ������ ���� ���Ѵ�.
		UINT nVertices = FBXIMPORTER->GetMeshDatas()[index].GetVertexCnt();
		pFileBasedMesh->SetnVertices(nVertices);

		//2. �Ҵ�
		XMFLOAT3* pVertices = new XMFLOAT3[nVertices];
		XMFLOAT3* pNormals = new XMFLOAT3[nVertices];
		XMFLOAT2* pUVs = new XMFLOAT2[nVertices];
		XMFLOAT3* pxmf3Weight = new XMFLOAT3[nVertices];
		XMFLOAT4* pxmf4BoneIndex = new XMFLOAT4[nVertices];

		//3. ����
		int nVertex{ 0 };
		//mesh�� ��� ������ ���ؼ�
		for (UINT j = 0; j < FBXIMPORTER->GetMeshDatas()[index].GetVertexCnt(); ++j) {
			//mesh info
			pVertices[nVertex] = FBXIMPORTER->GetMeshDatas()[index].GetVertexDatas()[j].GetPosition();
			pNormals[nVertex] = FBXIMPORTER->GetMeshDatas()[index].GetVertexDatas()[j].GetNormal();
			pUVs[nVertex] = FBXIMPORTER->GetMeshDatas()[index].GetVertexDatas()[j].GetUV();
			//animation info
			pxmf4BoneIndex[nVertex].x = static_cast<float>(FBXIMPORTER->GetMeshDatas()[index].GetVertexDatas()[j].GetBlendWeightPairs()[0].GetIndex());
			pxmf4BoneIndex[nVertex].y = static_cast<float>(FBXIMPORTER->GetMeshDatas()[index].GetVertexDatas()[j].GetBlendWeightPairs()[1].GetIndex());
			pxmf4BoneIndex[nVertex].z = static_cast<float>(FBXIMPORTER->GetMeshDatas()[index].GetVertexDatas()[j].GetBlendWeightPairs()[2].GetIndex());
			pxmf4BoneIndex[nVertex].w = static_cast<float>(FBXIMPORTER->GetMeshDatas()[index].GetVertexDatas()[j].GetBlendWeightPairs()[3].GetIndex());

			pxmf3Weight[nVertex].x = static_cast<float>(FBXIMPORTER->GetMeshDatas()[index].GetVertexDatas()[j].GetBlendWeightPairs()[0].GetWeight());
			pxmf3Weight[nVertex].y = static_cast<float>(FBXIMPORTER->GetMeshDatas()[index].GetVertexDatas()[j].GetBlendWeightPairs()[1].GetWeight());
			pxmf3Weight[nVertex++].z = static_cast<float>(FBXIMPORTER->GetMeshDatas()[index].GetVertexDatas()[j].GetBlendWeightPairs()[2].GetWeight());
		}
		//4. set
		pFileBasedMesh->SetpVertices(pVertices);
		pFileBasedMesh->SetpNormals(pNormals);
		pFileBasedMesh->SetpUVs(pUVs);
		pFileBasedMesh->SetpWeights(pxmf3Weight);
		pFileBasedMesh->SetpBoneIndex(pxmf4BoneIndex);
	}
	else {
		//1. ��ü ������ ���Ѵ�.
		UINT nVertices = FBXIMPORTER->GetMeshDatas()[index].GetVertexCnt();
		pFileBasedMesh->SetnVertices(nVertices);

		//2. �Ҵ�
		XMFLOAT3* pVertices = new XMFLOAT3[nVertices];
		XMFLOAT3* pNormals = new XMFLOAT3[nVertices];
		XMFLOAT2* pUVs = new XMFLOAT2[nVertices];

		//3. ����
		int nVertex{ 0 };
		//mesh�� ��� ������ ���ؼ�
		for (UINT j = 0; j < FBXIMPORTER->GetMeshDatas()[index].GetVertexCnt(); ++j) {
			//���� ���� ���´�.
			pVertices[nVertex] = FBXIMPORTER->GetMeshDatas()[index].GetVertexDatas()[j].GetPosition();
			pNormals[nVertex] = FBXIMPORTER->GetMeshDatas()[index].GetVertexDatas()[j].GetNormal();
			pUVs[nVertex++] = FBXIMPORTER->GetMeshDatas()[index].GetVertexDatas()[j].GetUV();
		}

		//4. set
		pFileBasedMesh->SetpVertices(pVertices);
		pFileBasedMesh->SetpNormals(pNormals);
		pFileBasedMesh->SetpUVs(pUVs);
	}


	//set index info
	//index
	//�߿��߿� offset�� �� ���� �ؾ��Ѵ�. 
	//ó�� mesh�� �ε����� �Ǿ��ִٸ� ��� mesh�� index�� �Ǿ��ִٰ� �����Ѵ�
	if (!FBXIMPORTER->GetMeshDatas()[0].GetByControlPoint()) {
		//1. ��� �ε��� ���� ����
		UINT nIndices = FBXIMPORTER->GetMeshDatas()[index].GetIndexCnt();
		pFileBasedMesh->SetnIndices(nIndices);

		//2. �Ҵ�
		UINT* pnIndices = new UINT[nIndices];

		//3. offset�� ���ذ��� index������ �ϼ�
		//������ �ε��� 
		//		int nIndex{ 0 };
		int offset{ 0 };
		for (UINT j = 0; j < nIndices; ++j) {
			//mesh�� index�� 0�� �ƴ� ��� offset�� ���ذ���.
			//offset�� ���� index�� �����Ѵ�. offest�� 0��mesh�� 0/ 1�� mesh�� 0�� mesh�� ���� ����ŭ offset�� ������.
			//������ ������ �ʴ´�.
			pnIndices[j] = FBXIMPORTER->GetMeshDatas()[index].GetIndexs()[j] + offset;
		}

		pFileBasedMesh->SetpIndices(pnIndices);

	}
	//index

	//set mesh info
	pFileBasedMesh->Begin();
	pFileBasedMesh->AddMeshTexture(RESOURCEMGR->GetTexture("DEFAULT"));
	return pFileBasedMesh;

	return nullptr;
}

shared_ptr<CFileBasedMesh> CFileBasedMesh::CreateMeshFromGJMFile(UINT index, bool bHasAnimation){
	/*
	test
	*/
	//WCHAR* p = IMPORTER->ReadWCHAR(0);

	shared_ptr<CFileBasedMesh> pFileBasedMesh;
	pFileBasedMesh = make_shared<CFileBasedMesh>();
	//set mesh name, index
	pFileBasedMesh->SetMeshIndex(index);
	
	//set vertex buffer info
	if (bHasAnimation) {
		//mesh texture
		int MeshTextureCnt = IMPORTER->ReadInt();
		if (MeshTextureCnt <= 0) {
			pFileBasedMesh->AddMeshTexture(RESOURCEMGR->GetTexture("DEFAULT"));
		}
		for (int i = 0; i < MeshTextureCnt; ++i) {
			//char name[64];
			//sprintf(name, "Test%d", index);
			string path;
			path = IMPORTER->Readstring();
			wstring wPath{ L"" };
			wPath.assign(path.cbegin(), path.cend());
			pFileBasedMesh->AddMeshTexture(RESOURCEMGR->CreateTexture(path.c_str(), (WCHAR*)wPath.c_str(), RESOURCEMGR->GetSampler("DEFAULT")));
			pFileBasedMesh->SetMeshMaterial(RESOURCEMGR->GetMaterial("DEFAULT"));
		}
		//mesh texture

		//1. ��ü ������ ���� ���Ѵ�.
		UINT nVertices = IMPORTER->ReadUINT();
		pFileBasedMesh->SetnVertices(nVertices);

		//2. �Ҵ�
		XMFLOAT3* pVertices = new XMFLOAT3[nVertices];
		XMFLOAT3* pNormals = new XMFLOAT3[nVertices];
		XMFLOAT2* pUVs = new XMFLOAT2[nVertices];
		XMFLOAT3* pxmf3Weight = new XMFLOAT3[nVertices];
		XMFLOAT4* pxmf4BoneIndex = new XMFLOAT4[nVertices];

		//3. ����
		int nVertex{ 0 };
		//mesh�� ��� ������ ���ؼ�
		for (UINT j = 0; j < nVertices; ++j) {
			pVertices[nVertex].x = IMPORTER->ReadFloat();
			pVertices[nVertex].y = IMPORTER->ReadFloat();
			pVertices[nVertex++].z = IMPORTER->ReadFloat();
		}
		nVertex = 0;
		for (UINT j = 0; j < nVertices; ++j) {
			pNormals[nVertex].x = IMPORTER->ReadFloat();
			pNormals[nVertex].y = IMPORTER->ReadFloat();
			pNormals[nVertex++].z = IMPORTER->ReadFloat();
		}
		nVertex = 0;
		for (UINT j = 0; j < nVertices; ++j) {
			pUVs[nVertex].x = IMPORTER->ReadFloat();
			pUVs[nVertex++].y = IMPORTER->ReadFloat();
		}
		nVertex = 0;
		for (UINT j = 0; j < nVertices; ++j) {
			pxmf3Weight[nVertex].x = IMPORTER->ReadFloat();
			pxmf3Weight[nVertex].y = IMPORTER->ReadFloat();
			pxmf3Weight[nVertex++].z = IMPORTER->ReadFloat();
		}
		nVertex = 0;
		for (UINT j = 0; j < nVertices; ++j) {
			pxmf4BoneIndex[nVertex].x = IMPORTER->ReadFloat();
			pxmf4BoneIndex[nVertex].y = IMPORTER->ReadFloat();
			pxmf4BoneIndex[nVertex].z = IMPORTER->ReadFloat();
			pxmf4BoneIndex[nVertex++].w = IMPORTER->ReadFloat();
		}
		//4. set
		pFileBasedMesh->SetpVertices(pVertices);
		pFileBasedMesh->SetpNormals(pNormals);
		pFileBasedMesh->SetpUVs(pUVs);
		pFileBasedMesh->SetpWeights(pxmf3Weight);
		pFileBasedMesh->SetpBoneIndex(pxmf4BoneIndex);


	}
	else {
		//1. ��ü ������ ���Ѵ�.
		UINT nVertices = IMPORTER->ReadUINT();
		pFileBasedMesh->SetnVertices(nVertices);

		//2. �Ҵ�
		XMFLOAT3* pVertices = new XMFLOAT3[nVertices];
		XMFLOAT3* pNormals = new XMFLOAT3[nVertices];
		XMFLOAT2* pUVs = new XMFLOAT2[nVertices];

		//3. ����
		int nVertex{ 0 };
		//mesh�� ��� ������ ���ؼ�
		for (UINT j = 0; j < nVertices; ++j) {
			//���� ���� ���´�.
			pVertices[nVertex].x = IMPORTER->ReadFloat();
			pVertices[nVertex].y = IMPORTER->ReadFloat();
			pVertices[nVertex++].z = IMPORTER->ReadFloat();
		}
		nVertex = 0;
		for (UINT j = 0; j < nVertices; ++j) {
			pNormals[nVertex].x = IMPORTER->ReadFloat();
			pNormals[nVertex].y = IMPORTER->ReadFloat();
			pNormals[nVertex++].z = IMPORTER->ReadFloat();
		}
		nVertex = 0;
		for (UINT j = 0; j < nVertices; ++j) {
			pUVs[nVertex].x = IMPORTER->ReadFloat();
			pUVs[nVertex].y = IMPORTER->ReadFloat();
		}
		//4. set
		pFileBasedMesh->SetpVertices(pVertices);
		pFileBasedMesh->SetpNormals(pNormals);
		pFileBasedMesh->SetpUVs(pUVs);
	}


	//set index info
	
	//1. ��� �ε��� ���� ����
	UINT nIndices = IMPORTER->ReadUINT();
	if (nIndices > 0) {
		pFileBasedMesh->SetnIndices(nIndices);

		//2. �Ҵ�
		UINT* pnIndices = new UINT[nIndices];

		//3. offset�� ���ذ��� index������ �ϼ�
		//������ �ε��� 
		//		int nIndex{ 0 };
		int offset{ 0 };
		for (UINT j = 0; j < nIndices; ++j) {
			//mesh�� index�� 0�� �ƴ� ��� offset�� ���ذ���.
			//offset�� ���� index�� �����Ѵ�. offest�� 0��mesh�� 0/ 1�� mesh�� 0�� mesh�� ���� ����ŭ offset�� ������.
			//������ ������ �ʴ´�.
			pnIndices[j] = IMPORTER->ReadUINT();
		}

		pFileBasedMesh->SetpIndices(pnIndices);
		//index
	}
	pFileBasedMesh->Begin();
	return pFileBasedMesh;

	return nullptr;
}

bool CFileBasedMesh::CreateVertexBuffer() {
	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	UINT nVertexBuffer{ 0 };
	ID3D11Buffer *pd3dBuffers[1];
	UINT pnBufferStrides[1];
	UINT pnBufferOffsets[1];	

	//position/ aabb
	if (m_pVertices) {
		m_pd3dPositionBuffer = CreateBuffer(sizeof(XMFLOAT3), m_nVertices, m_pVertices, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DEFAULT, 0);
		//create space mesh aabb
		BoundingBox boundingBox;
		BoundingBox::CreateFromPoints(boundingBox, (size_t)m_nVertices, m_pVertices, (size_t)sizeof(XMFLOAT3));
		m_AABB.SetBoundingBoxInfo(boundingBox);

		pd3dBuffers[0] = m_pd3dPositionBuffer;
		pnBufferStrides[0] = sizeof(XMFLOAT3);
		pnBufferOffsets[0] = 0;
		AssembleToVertexBuffer(1, pd3dBuffers, pnBufferStrides, pnBufferOffsets);
	}
	//normal
	if (m_pNormals) {
		m_pd3dNormalBuffer = CreateBuffer(sizeof(XMFLOAT3), m_nVertices, m_pNormals, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DEFAULT, 0);
		
		pd3dBuffers[0] = m_pd3dNormalBuffer;
		pnBufferStrides[0] = sizeof(XMFLOAT3);
		pnBufferOffsets[0] = 0;
		AssembleToVertexBuffer(1, pd3dBuffers, pnBufferStrides, pnBufferOffsets);
	}
	//uv
	if (m_pUVs) {
		m_pd3dUVBuffer = CreateBuffer(sizeof(XMFLOAT2), m_nVertices, m_pUVs, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DEFAULT, 0);
		
		pd3dBuffers[0] = m_pd3dUVBuffer;
		pnBufferStrides[0] = sizeof(XMFLOAT2);
		pnBufferOffsets[0] = 0;
		AssembleToVertexBuffer(1, pd3dBuffers, pnBufferStrides, pnBufferOffsets);
	}
	//weight
	if (m_pxmf3Weight) {
		m_pd3dWeightBuffer = CreateBuffer(sizeof(XMFLOAT3), m_nVertices, m_pxmf3Weight, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DEFAULT, 0);

		pd3dBuffers[0] = m_pd3dWeightBuffer;
		pnBufferStrides[0] = sizeof(XMFLOAT3);
		pnBufferOffsets[0] = 0;
		AssembleToVertexBuffer(1, pd3dBuffers, pnBufferStrides, pnBufferOffsets);
	}
	//bone index
	if (m_pxmf4BoneIndex) {
		m_pd3dBoneIndexBuffer = CreateBuffer(sizeof(XMFLOAT4), m_nVertices, m_pxmf4BoneIndex, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DEFAULT, 0);

		pd3dBuffers[0] = m_pd3dBoneIndexBuffer;
		pnBufferStrides[0] = sizeof(XMFLOAT4);
		pnBufferOffsets[0] = 0;
		AssembleToVertexBuffer(1, pd3dBuffers, pnBufferStrides, pnBufferOffsets);
	}

	if (m_ppd3dVertexBuffers)	return true;

	return false;
}
bool CFileBasedMesh::CreateIndexBuffer() {
	if (m_pnIndices) {
		m_pd3dIndexBuffer = CreateBuffer(sizeof(UINT), m_nIndices, m_pnIndices, D3D11_BIND_INDEX_BUFFER, D3D11_USAGE_DEFAULT, 0);
	}

	return true;
}

CFileBasedMesh::CFileBasedMesh() : CMesh() {

}
CFileBasedMesh::~CFileBasedMesh(){

}