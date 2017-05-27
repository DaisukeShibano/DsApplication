#include "DsAppPch.h"
#ifndef _DS_DECISION_TREE_OPERATOR_H_
#include "Evolve/Gene/DsDecisionTreeOperator.h"
#endif

#ifndef _DS_DECISION_TREE_NODE_H_
#include "Evolve/Gene/DsDecisionTreeNode.h"
#endif

using namespace DsApp;


DsDecisionTreeOperator::DsDecisionTreeOperator()
	: m_inputDatas()
	, m_totalTreeSize(0)
{
}

DsDecisionTreeOperator::~DsDecisionTreeOperator()
{
}

DsDecisionRootNode* DsDecisionTreeOperator::CreateTree()
{
	DsDecisionRootNode* ret = NULL;
	
	//入力データがなければ意味ないので作らない
	if (!m_inputDatas.empty())
	{
		DsDecisionTreeNode* arg = _CreateTree(DECISION_TREE_NODE_TYPE::NOTHING, 0, 0);
		std::vector<DsDecisionTreeNode*> args;
		args.push_back(arg);
		ret = new DsDecisionRootNode(args);
		ret->SetTotalSize(m_totalTreeSize);
		m_totalTreeSize = 0;
	}
	else{
		DS_LOG("入力データがなかったので決定木は作られませんでした");
	}
	return ret;
}

DsDecisionTreeNode* DsDecisionTreeOperator::_CreateTree(DECISION_TREE_NODE_TYPE parentType, int width, int depth)
{
	DsDecisionTreeNode* ret = NULL;
	if ((width == 0) && IsDecitionTreeNodeIF(parentType)){
		//親がIFで第一引数なので、bool系限定。
		std::vector<int> types;
		for (int i = static_cast<int>(DECISION_TREE_NODE_TYPE::BOOL_BEGIN); i < static_cast<int>(DECISION_TREE_NODE_TYPE::BOOL_END	); ++i){
			types.push_back(i);
		}
		const int noise = (width + depth) % types.size();
		const int createType = (rand() + noise) % types.size();
		ret = _CreateTreeNodeIns(static_cast<DECISION_TREE_NODE_TYPE>(createType), width, depth);
	}
	else{
		if ((m_totalTreeSize < TREE_NODE_MAX_NUM) && (depth < TREE_DEPTH_MAX_INIT))
		{
			//親がIF以外なので、制限無し
			std::vector<int> types;
			for (int i = static_cast<int>(DECISION_TREE_NODE_TYPE::THEORETICAL_BEGIN); i < static_cast<int>(DECISION_TREE_NODE_TYPE::THEORETICAL_END); ++i){
				types.push_back(i);
			}
			for (int i = static_cast<int>(DECISION_TREE_NODE_TYPE::BOOL_BEGIN); i < static_cast<int>(DECISION_TREE_NODE_TYPE::BOOL_END); ++i){
				types.push_back(i);
			}
			for (int i = static_cast<int>(DECISION_TREE_NODE_TYPE::IF_BEGIN); i < static_cast<int>(DECISION_TREE_NODE_TYPE::IF_END); ++i){
				types.push_back(i);
			}
			for (int i = static_cast<int>(DECISION_TREE_NODE_TYPE::TERMINAL_BEGIN); i < static_cast<int>(DECISION_TREE_NODE_TYPE::TERMINAL_END); ++i){
				types.push_back(i);
			}
			const int noise = (width + depth) % types.size();
			const int createTypeIdx = (rand() + noise) % types.size();
			ret = _CreateTreeNodeIns(static_cast<DECISION_TREE_NODE_TYPE>(types[createTypeIdx]), width, depth);
		}
		else{
			//サイズが最大まで来たのでターミナル
			ret = _CreateTreeNodeIns(DECISION_TREE_NODE_TYPE::INPUT, width, depth);
		}
	}
	return ret;
}

DsDecisionTreeNode* DsDecisionTreeOperator::_CreateTreeNodeIns(DECISION_TREE_NODE_TYPE createType, int width, int depth)
{
	m_totalTreeSize = m_totalTreeSize + 1;
	DsDecisionTreeNode* ret = NULL;
	switch (createType)
	{
	case DECISION_TREE_NODE_TYPE::AND:
		ret = new DsDecisionAndNode(_CreateTree(createType, 0, depth+1), _CreateTree(createType, 1, depth+1));
		break;
	case DECISION_TREE_NODE_TYPE::OR:
		ret = new DsDecisionOrNode(_CreateTree(createType, 0, depth + 1), _CreateTree(createType, 1, depth + 1));
		break;
	case DECISION_TREE_NODE_TYPE::NOT:
		ret = new DsDecisionNotNode(_CreateTree(createType, 0, depth + 1));
		break;
	case DECISION_TREE_NODE_TYPE::OVER:
		ret = new DsDecisionOverNode(_CreateTree(createType, 0, depth + 1), _CreateTree(createType, 1, depth + 1));
		break;
	case DECISION_TREE_NODE_TYPE::UNDER:
		ret = new DsDecisionUnderNode(_CreateTree(createType, 0, depth + 1), _CreateTree(createType, 1, depth + 1));
		break;
	case DECISION_TREE_NODE_TYPE::EQUAL:
		ret = new DsDecisionEqualNode(_CreateTree(createType, 0, depth + 1), _CreateTree(createType, 1, depth + 1));
		break;
	case DECISION_TREE_NODE_TYPE::IF:
		ret = new DsDecisionIfNode(_CreateTree(createType, 0, depth + 1), _CreateTree(createType, 1, depth + 1), _CreateTree(createType, 2, depth + 1));
		break;
	case DECISION_TREE_NODE_TYPE::INPUT:
		{
			if (!m_inputDatas.empty()){
				const int noise = (width + depth) % m_inputDatas.size();
				const int idx = (rand() + noise) % m_inputDatas.size();
				ret = new DsDecisionInputNode(*m_inputDatas[idx]);
			}
		}		
		break;

	default:
		DS_ASSERT(false, "不正な決定木ノード");
		break;
	}
	return ret;
}

/*
	交差
*/
void DsDecisionTreeOperator::Crossover(DsDecisionRootNode* pRoot1, DsDecisionRootNode* pRoot2)
{
	int arg1 = 0;
	DsDecisionTreeNode* pParent1 = NULL;
	DsDecisionTreeNode* pChange1 = _GetRandomNode(pRoot1, arg1, pParent1);
	int arg2 = 0;
	DsDecisionTreeNode* pParent2 = NULL;
	DsDecisionTreeNode* pChange2 = _GetRandomNode(pRoot2, arg2, pParent2);

	if (pParent1 && pChange1 && pParent2 && pChange2){
		
		//１の交差点が、IFの第一引数でないかチェック
		if (IsDecitionTreeNodeIF(pParent1->GetType())){
			if (arg1 == 0){
				//IFの第一なら、相手はbool系でなければ交差できない
				if (!IsDecitionTreeNodeBool(pChange2->GetType())){
					//bool系でなかったので、IFの第一引数以外にする。
					arg1 = (rand() % (pChange1->GetArgNum() - 1) + 1);
					pChange1 = pParent1->GetArg(arg1);
				}
			}
		}

		//２の交差点が、IFの第一引数でないかチェック
		if (IsDecitionTreeNodeIF(pParent2->GetType())){
			if (arg2 == 0){
				//IFの第一なら、相手はbool系でなければ交差できない
				if (!IsDecitionTreeNodeBool(pChange1->GetType())){
					//bool系でなかったので、IFの第一引数以外にする。
					arg2 = (rand() % (pChange2->GetArgNum() - 1) + 1);
					pChange2 = pParent2->GetArg(arg2);
				}
			}
		}

		//交差する
		pParent1->SetArg(arg1, pChange2);
		pParent2->SetArg(arg2, pChange1);

	}
}

DsDecisionTreeNode* DsDecisionTreeOperator::_GetRandomNode(DsDecisionRootNode* pRoot, int& argIdx, DsDecisionTreeNode*& pParent) const
{
	DsDecisionTreeNode* ret = NULL;
	
	const int crossPoint = rand() % pRoot->GetTotalSize();
	DsDecisionTreeNode* crossNode = _GetNode(pRoot, crossPoint);
	if (crossNode && (0 < crossNode->GetArgNum())){
		const int idx = rand() % crossNode->GetArgNum();
		ret = crossNode->GetArg(idx);
		pParent = crossNode;
		argIdx = idx;
	}

	return ret;
}

/*
	crossPosまで検索して返す
*/
DsDecisionTreeNode* DsDecisionTreeOperator::_GetNode(DsDecisionTreeNode* pNode, int targetPos, int& currentPos) const
{
	DsDecisionTreeNode* ret = NULL;
	if (targetPos <= currentPos){
		//指定ノード到達
		return pNode;
	}
	else{
		//指定ノードではないので検索
		for (int i = 0; i < pNode->GetArgNum(); ++i){
			currentPos = currentPos + 1;
			ret = _GetNode(pNode->GetArg(i), targetPos, currentPos);
			//見つかったので抜ける。
			if (ret){
				break;
			}
		}
	}
	return ret;
}
DsDecisionTreeNode* DsDecisionTreeOperator::_GetNode(DsDecisionTreeNode* pNode, int targetPos) const
{
	int tmpSize = 0;
	return _GetNode(pNode, targetPos, tmpSize);
}

void DsDecisionTreeOperator::Mutation(DsDecisionRootNode* pRoot)
{
	const int mutationPoint = (rand() % max(0, pRoot->GetTotalSize()-1)+1);
	DsDecisionTreeNode* pMutationNode = _GetNode(pRoot, mutationPoint);
	if (pMutationNode){
		//同じ枝の数のやつに突然変異
		if (pMutationNode->GetArgNum() == 2){
			//引数退避
			DsDecisionTreeNode* arg1 = pMutationNode->GetArg(0);
			DsDecisionTreeNode* arg2 = pMutationNode->GetArg(1);
			 
			//作り直し
			std::vector<DECISION_TREE_NODE_TYPE> types;
			types.push_back(DECISION_TREE_NODE_TYPE::AND);
			types.push_back(DECISION_TREE_NODE_TYPE::OR);
			types.push_back(DECISION_TREE_NODE_TYPE::OVER);
			types.push_back(DECISION_TREE_NODE_TYPE::UNDER);
			types.push_back(DECISION_TREE_NODE_TYPE::EQUAL);
			
			DsDecisionTreeNode* newNode = NULL;
			const int createTypeIdx = rand() % types.size();
			switch (types[createTypeIdx]){
			case DECISION_TREE_NODE_TYPE::AND:
				newNode = new DsDecisionAndNode(arg1, arg2);
				break;
			case DECISION_TREE_NODE_TYPE::OR:
				newNode = new DsDecisionAndNode(arg1, arg2);
				break;
			case DECISION_TREE_NODE_TYPE::OVER:
				newNode = new DsDecisionAndNode(arg1, arg2);
				break;
			case DECISION_TREE_NODE_TYPE::UNDER:
				newNode = new DsDecisionAndNode(arg1, arg2);
				break;
			case DECISION_TREE_NODE_TYPE::EQUAL:
				newNode = new DsDecisionAndNode(arg1, arg2);
				break;
			default:
				break;
			}
			if (newNode){
				delete pMutationNode;
				pMutationNode = newNode;
			}
		}
		else{
			//今のところ、引数が２個のやつしか突然変異できるやつがない
		}
	}
}

void DsDecisionTreeOperator::Delete(DsDecisionTreeNode* pNode)
{
	const int argNum = pNode->GetArgNum();
	for (int i = 0; i < argNum; ++i){
		Delete(pNode->GetArg(i));
		pNode->SetArg(i, NULL);
	}
	delete pNode;
}

DsDecisionTreeNode* DsDecisionTreeOperator::Clone(DsDecisionTreeNode* pNode)
{
	DsDecisionTreeNode* ret = NULL;
	switch (pNode->GetType())
	{
	case DECISION_TREE_NODE_TYPE::ROOT:
		{
			std::vector<DsDecisionTreeNode*> args;
			for (int i = 0; i < pNode->GetArgNum(); ++i){
				args.push_back( Clone(pNode->GetArg(1)) );
			}
			ret = new DsDecisionRootNode(args);
		}
		break;
	case DECISION_TREE_NODE_TYPE::AND:
		ret = new DsDecisionAndNode( Clone(pNode->GetArg(0)), Clone(pNode->GetArg(1)) );
		break;
	case DECISION_TREE_NODE_TYPE::OR:
		ret = new DsDecisionOrNode(Clone(pNode->GetArg(0)), Clone(pNode->GetArg(1)));
		break;
	case DECISION_TREE_NODE_TYPE::NOT:
		ret = new DsDecisionNotNode(Clone(pNode->GetArg(0)));
		break;
	case DECISION_TREE_NODE_TYPE::OVER:
		ret = new DsDecisionOverNode(Clone(pNode->GetArg(0)), Clone(pNode->GetArg(1)));
		break;
	case DECISION_TREE_NODE_TYPE::UNDER:
		ret = new DsDecisionUnderNode(Clone(pNode->GetArg(0)), Clone(pNode->GetArg(1)));
		break;
	case DECISION_TREE_NODE_TYPE::EQUAL:
		ret = new DsDecisionEqualNode(Clone(pNode->GetArg(0)), Clone(pNode->GetArg(1)));
		break;
	case DECISION_TREE_NODE_TYPE::IF:
		ret = new DsDecisionIfNode(Clone(pNode->GetArg(0)), Clone(pNode->GetArg(1)), Clone(pNode->GetArg(2)) );
		break;
	case DECISION_TREE_NODE_TYPE::INPUT:
		ret = new DsDecisionInputNode(static_cast<DsDecisionInputNode*>(pNode)->RefData());
		break;
	default:
		DS_ASSERT(false, "不正な決定木ノード");
		break;
	}
	return ret;
}