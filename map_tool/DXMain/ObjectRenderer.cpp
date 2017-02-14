#include "stdafx.h"
#include "ObjectRenderer.h"

bool CObjectRenderer::Begin(){
	for (int i = 0; i < object_id::OBJECT_END; ++i) {
		object_id id = (object_id)i;
		m_mRenderContainer.insert(pairRenderContainer(id, RCSELLER->GetRenderContainer(id)));
	}

	return true;
}

bool CObjectRenderer::End(){
	m_mRenderContainer.clear();

	return true;
}

void CObjectRenderer::SetShaderState(){
	
}

void CObjectRenderer::CleanShaderState(){
	object_id id = object_id::OBJECT_END;
	//scene의 모든 Part의 rendercontainer안에 part list Clear!
	for (int i = 0; i < object_id::OBJECT_END; ++i) {
		id = (object_id)i;
		m_mRenderContainer[id]->ClearObjectList();
	}
}

void CObjectRenderer::UpdateShaderState(){

}

void CObjectRenderer::Excute(shared_ptr<CCamera> pCamera){
	object_id id = object_id::OBJECT_END;
	//scene의 모든 Part의 rendercontainer안에 part list Render!
	for (int i = 3; i < object_id::OBJECT_END; ++i) {
		id = (object_id)i;
		m_mRenderContainer[id]->Render(pCamera);
	}

	CleanShaderState();
}

CObjectRenderer::CObjectRenderer() : DXObject("objectrenderer"){

}

CObjectRenderer::~CObjectRenderer(){

}
