#include "DsAppPch.h"
#ifndef _DS_ATTACH_COMPONENT_
#include "World/Component/Attach/DsAttachComponent.h"
#endif
//‘¼‚Ìƒwƒbƒ_
#ifndef _DS_FIELD_OBJ_H_
#include "World/Field/DsFieldObj.h"
#endif

using namespace DsApp;


DsAttachComponent::DsAttachComponent()
	: m_target(DsMat44d::Identity())
	, m_pMove(NULL)
	, m_reqAttach(false)
{
}

DsAttachComponent::~DsAttachComponent()
{
}

//virtual
bool DsAttachComponent::Update(const COMPONENT_UPDATE_ARG& arg)
{
	const bool ret = m_reqAttach;

	if (m_pMove) {
		m_pMove->SetPosition(m_target.GetPos());
		m_pMove->SetRotation(m_target.ToMat33());
	}

	m_reqAttach = false;
	return ret;
}


void DsAttachComponent::Request(const DsMat44d target, DsFieldObj* pMove)
{
	m_target = target;
	m_pMove = pMove;
	m_reqAttach = true;
}