#include "DsPch.h"
#ifndef _DS_ACTION_STATE_CTRL_
#include"ActionState/DsActionStateCtrl.h"
#endif

#ifndef _DS_AS_NODE_
#include "ActionState/DsASNode.h"
#endif
using namespace DsLib;


DsActionStateCtrl::DsActionStateCtrl()
	: m_execNodes()
	, m_registerNodes()
{
}

DsActionStateCtrl::~DsActionStateCtrl()
{
}

void DsActionStateCtrl::RegisterNode(DsASNode* pNode)
{
	if (pNode) {
		m_registerNodes.push_back(pNode);
	}
}

void DsActionStateCtrl::Update(double dt)
{
	std::vector<DsASNode*> add;
	std::vector<DsASNode*> del;

	for (DsASNode* pNode : m_execNodes) {
		if (pNode->IsNext()) {
			del.push_back(pNode);

			const int num = pNode->GetNextNum();
			for (int idx = 0; idx < num; ++idx) {
				DsASNode* pAddNode = pNode->GetNext()[idx];
				add.push_back(pAddNode);
			}
		}
		else {
			pNode->Update(dt);
		}
	}

	for (DsASNode* pNode : del) {
		std::remove_if(m_execNodes.begin(), m_execNodes.end(), [pNode, dt](DsASNode* pExecNode) {
			const bool ret = pNode == pExecNode;
			//他のノードがUpdateでフラグ立てるかもしれないので、Deactiveは全てのUpdateが終わった後
			pNode->OnDeactive(dt);
			return ret;
		});
	}

	for (DsASNode* pNode : m_registerNodes) {
		pNode->OnActive(dt);
		m_execNodes.push_back(pNode);
	}
	m_registerNodes.clear();

	for (DsASNode* pNode : add) {
		//Deactiveでフラグが降りてしまうかもしれないので、その後
		pNode->OnActive(dt);
		m_execNodes.push_back(pNode);
	}

}