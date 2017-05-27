#ifndef _DS_DECISION_TREE_OPERATOR_H_
#define _DS_DECISION_TREE_OPERATOR_H_

namespace DsApp
{
	class DsDecisionTreeNode;
	class DsDecisionRootNode;
	struct DsDecisionTreeData;
	enum class DECISION_TREE_NODE_TYPE : int;
}

namespace DsApp
{
	class DsDecisionTreeOperator
	{
	private:
		enum
		{
			TREE_NODE_MAX_NUM = 10000,//ノードの整合性を保つためこのサイズを超えることもある。
			TREE_DEPTH_MAX_INIT = 100,
		};

	public:
		DsDecisionTreeOperator();
		~DsDecisionTreeOperator();

	public:
		std::vector<DsDecisionTreeData*> RefData(){ return m_inputDatas; }

	public:
		DsDecisionRootNode* CreateTree();
	private:
		DsDecisionTreeNode* _CreateTreeNodeIns(DECISION_TREE_NODE_TYPE createType, int width, int depth);
		DsDecisionTreeNode* _CreateTree(DECISION_TREE_NODE_TYPE parentType, int width, int depth);

	public:
		void Crossover(DsDecisionRootNode* pRoot1, DsDecisionRootNode* pRoot2);
	private:
		DsDecisionTreeNode* _GetRandomNode(DsDecisionRootNode* pRoot, int& argIdx, DsDecisionTreeNode*& pParent) const;
		DsDecisionTreeNode* _GetNode(DsDecisionTreeNode* pNode, int targetPos, int& currentPos) const;
		DsDecisionTreeNode* _GetNode(DsDecisionTreeNode* pNode, int targetPos) const;

	public:
		void Mutation(DsDecisionRootNode* pRoot);

	public:
		void Delete(DsDecisionTreeNode* pNode);
		DsDecisionTreeNode* Clone(DsDecisionTreeNode* pNode);

	private:
		std::vector<DsDecisionTreeData*> m_inputDatas;
		int m_totalTreeSize;
	};
}

#endif