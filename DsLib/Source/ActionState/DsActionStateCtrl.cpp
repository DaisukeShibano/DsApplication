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

void DsActionStateCtrl::RegisterActiveNode(DsASNode* pNode)
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
				DsASNode** pAddNode = pNode->GetNext();
				if (pAddNode) {
					add.push_back(pAddNode[idx]);
				}
			}
		}
		else {
			pNode->Update(dt);
		}
	}

	for (DsASNode* pNode : del) {
		auto it = std::find_if(m_execNodes.begin(), m_execNodes.end(), [pNode, dt](DsASNode* pExecNode) {
			return pNode == pExecNode;
		});

		if (m_execNodes.end() != it) {
			//他のノードがUpdateでフラグ立てるかもしれないので、Deactiveは全てのUpdateが終わった後
			(*it)->OnDeactive(dt);
			m_execNodes.erase(it);
		}
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


std::vector<DsASNode*> DsActionStateCtrl::GetActiveNode() const
{
	return m_execNodes;
}