#include "stdafx.h"
#include "FbxAnimStackData.h"

bool CFbxAnimStackData::Begin(){
	
	return true;
}

bool CFbxAnimStackData::End(){
	m_pAnimStack->Destroy();
	m_pAnimStack = nullptr;

	m_tStart = 0;
	m_tEnd = 0;

	return true;
}
