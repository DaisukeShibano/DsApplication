#ifndef _DS_DECISION_TREE_NODE_H_
#define _DS_DECISION_TREE_NODE_H_
#endif

namespace DsApp
{
	enum class DECISION_TREE_NODE_TYPE : int
	{
		ROOT_BEGIN,
		ROOT,
		ROOT_END,

		THEORETICAL_BEGIN,
		AND,
		OR,
		NOT,
		THEORETICAL_END,

		BOOL_BEGIN,
		OVER,
		UNDER,
		EQUAL,
		BOOL_END,

		IF_BEGIN,
		IF,
		IF_END,

		TERMINAL_BEGIN,
		INPUT,
		TERMINAL_END,

		NOTHING,
	};

	struct DsDecisionTreeData
	{
		DsDecisionTreeData()
			:flag(0)
		{}
		unsigned int flag;
	};

	inline bool IsDecitionTreeNodeBool(DECISION_TREE_NODE_TYPE type)
	{
		return ((static_cast<int>(DECISION_TREE_NODE_TYPE::BOOL_BEGIN) < static_cast<int>(type)) && (static_cast<int>(type) < static_cast<int>(DECISION_TREE_NODE_TYPE::BOOL_END)));
	}

	inline bool IsDecitionTreeNodeIF(DECISION_TREE_NODE_TYPE type)
	{
		return ((static_cast<int>(DECISION_TREE_NODE_TYPE::IF_BEGIN) < static_cast<int>(type)) && (static_cast<int>(type) < static_cast<int>(DECISION_TREE_NODE_TYPE::IF_END)));
	}

	class DsDecisionTreeNode
	{
	public:
		DsDecisionTreeNode(DECISION_TREE_NODE_TYPE type);
		~DsDecisionTreeNode();
		
	public:
		virtual DsDecisionTreeData Eval() = 0;
		virtual int GetArgNum() const = 0;
		virtual void SetArg(int argIdx, DsDecisionTreeNode* pArg) = 0;
		virtual DsDecisionTreeNode* GetArg(int argIdx) = 0;

	public:
		DECISION_TREE_NODE_TYPE GetType() const { return m_type; }

	private:
		DECISION_TREE_NODE_TYPE m_type;
		
	};

	//à¯êîÇPå¬ÉmÅ[Éh
	class DsDecisionTreeNodeArg1 : public DsDecisionTreeNode
	{
	public:
		DsDecisionTreeNodeArg1(DECISION_TREE_NODE_TYPE type, DsDecisionTreeNode* pArg1)
			:DsDecisionTreeNode(type), m_pArg1(pArg1){}
	public:
		virtual int GetArgNum() const override{ return 1; }
		virtual void SetArg(int argIdx, DsDecisionTreeNode* pArg) override { m_pArg1 = pArg; }
		virtual DsDecisionTreeNode* GetArg(int argIdx) override { return m_pArg1; }
	protected:
		DsDecisionTreeNode* m_pArg1;
	};
	
	//à¯êîÇQå¬ÉmÅ[Éh
	class DsDecisionTreeNodeArg2 : public DsDecisionTreeNode
	{
	public:
		DsDecisionTreeNodeArg2(DECISION_TREE_NODE_TYPE type, DsDecisionTreeNode* pArg1, DsDecisionTreeNode* pArg2)
			:DsDecisionTreeNode(type), m_pArg1(pArg1), m_pArg2(pArg2){}
	public:
		virtual int GetArgNum() const override{ return 2; }
		virtual void SetArg(int argIdx, DsDecisionTreeNode* pArg) override { if (argIdx < GetArgNum()){ m_pArgs[argIdx] = pArg; } }
		virtual DsDecisionTreeNode* GetArg(int argIdx) override { return ( (argIdx < GetArgNum()) ? (m_pArgs[argIdx]) : (NULL) ); }

	protected:
		union{
			DsDecisionTreeNode* m_pArgs[2];
			struct{
				DsDecisionTreeNode* m_pArg1;
				DsDecisionTreeNode* m_pArg2;
			};
		};
	};

	//à¯êîÇRå¬ÉmÅ[Éh
	class DsDecisionTreeNodeArg3 : public DsDecisionTreeNode
	{
	public:
		DsDecisionTreeNodeArg3(DECISION_TREE_NODE_TYPE type, DsDecisionTreeNode* pArg1, DsDecisionTreeNode* pArg2, DsDecisionTreeNode* pArg3)
			:DsDecisionTreeNode(type), m_pArg1(pArg1), m_pArg2(pArg2), m_pArg3(pArg3){}
	public:
		virtual int GetArgNum() const override{ return 3; }
		virtual void SetArg(int argIdx, DsDecisionTreeNode* pArg) override { if (argIdx < GetArgNum()){ m_pArgs[argIdx] = pArg; } }
		virtual DsDecisionTreeNode* GetArg(int argIdx) override { return ((argIdx < GetArgNum()) ? (m_pArgs[argIdx]) : (NULL)); }

	protected:
		union{
			DsDecisionTreeNode* m_pArgs[3];
			struct{
				DsDecisionTreeNode* m_pArg1;
				DsDecisionTreeNode* m_pArg2;
				DsDecisionTreeNode* m_pArg3;
			};
		};
	};

	/*
		Root
		@par argsÇ…ÇÕó~ÇµÇ¢åãâ ÇÃêîÇæÇØópà”Ç∑ÇÈ
	*/
	class DsDecisionRootNode : public DsDecisionTreeNode
	{
	public:
		DsDecisionRootNode( const std::vector<DsDecisionTreeNode*>& args )
			:DsDecisionTreeNode(DECISION_TREE_NODE_TYPE::ROOT)
			,m_args()
		{
			for(DsDecisionTreeNode* pNode : args){
				m_args.push_back(pNode);
			}
		}
	public:
		virtual DsDecisionTreeData Eval() override
		{
			for(DsDecisionTreeNode* pNode : m_args){
				pNode->Eval();
			}
			return DsDecisionTreeData();
		}
		virtual int GetArgNum() const override { return static_cast<int>(m_args.size()); }
		virtual void SetArg(int argIdx, DsDecisionTreeNode* pArg) override {}
		virtual DsDecisionTreeNode* GetArg(int argIdx) override { return ((argIdx < GetArgNum()) ? (m_args[argIdx]) : (NULL)); }

	public:
		void SetTotalSize(int size){ m_totalSize = size; }
		int GetTotalSize() const { return m_totalSize; }

	private:
		std::vector<DsDecisionTreeNode*> m_args;
		int m_totalSize;
	};

	/*
		ââéZéqån
	*/
	class DsDecisionAndNode : public DsDecisionTreeNodeArg2
	{
	public:
		DsDecisionAndNode(DsDecisionTreeNode* pArg1, DsDecisionTreeNode* pArg2)
			:DsDecisionTreeNodeArg2(DECISION_TREE_NODE_TYPE::AND, pArg1, pArg2){}
		virtual DsDecisionTreeData Eval() override
		{
			DsDecisionTreeData ret;
			ret.flag = m_pArg1->Eval().flag & m_pArg2->Eval().flag;
			return ret;
		}
	};

	class DsDecisionOrNode : public DsDecisionTreeNodeArg2
	{
	public:
		DsDecisionOrNode(DsDecisionTreeNode* pArg1, DsDecisionTreeNode* pArg2)
			:DsDecisionTreeNodeArg2(DECISION_TREE_NODE_TYPE::OR, pArg1, pArg2){}
		virtual DsDecisionTreeData Eval() override
		{
			DsDecisionTreeData ret;
			ret.flag = m_pArg1->Eval().flag | m_pArg2->Eval().flag;
			return ret;
		}
	};

	class DsDecisionNotNode : public DsDecisionTreeNodeArg1
	{
	public:
		DsDecisionNotNode(DsDecisionTreeNode* pArg1)
			:DsDecisionTreeNodeArg1(DECISION_TREE_NODE_TYPE::NOT, pArg1){}
		virtual DsDecisionTreeData Eval() override
		{
			DsDecisionTreeData ret;
			ret.flag = ~m_pArg1->Eval().flag;
			return ret;
		}
	};

	/*
		bool
	*/
	class DsDecisionOverNode : public DsDecisionTreeNodeArg2
	{
	public:
		DsDecisionOverNode(DsDecisionTreeNode* pArg1, DsDecisionTreeNode* pArg2)
			:DsDecisionTreeNodeArg2(DECISION_TREE_NODE_TYPE::OVER, pArg1, pArg2){}
		virtual DsDecisionTreeData Eval() override
		{
			DsDecisionTreeData ret;
			ret.flag = m_pArg1->Eval().flag < m_pArg2->Eval().flag;
			return ret;
		}
	};

	class DsDecisionUnderNode : public DsDecisionTreeNodeArg2
	{
	public:
		DsDecisionUnderNode(DsDecisionTreeNode* pArg1, DsDecisionTreeNode* pArg2)
			:DsDecisionTreeNodeArg2(DECISION_TREE_NODE_TYPE::UNDER, pArg1, pArg2){}
	public:
		virtual DsDecisionTreeData Eval() override
		{
			DsDecisionTreeData ret;
			ret.flag = m_pArg1->Eval().flag > m_pArg2->Eval().flag;
			return ret;
		}
	};

	class DsDecisionEqualNode : public DsDecisionTreeNodeArg2
	{
	public:
		DsDecisionEqualNode(DsDecisionTreeNode* pArg1, DsDecisionTreeNode* pArg2)
			:DsDecisionTreeNodeArg2(DECISION_TREE_NODE_TYPE::EQUAL, pArg1, pArg2){}
	public:
		virtual DsDecisionTreeData Eval() override
		{
			DsDecisionTreeData ret;
			ret.flag = m_pArg1->Eval().flag == m_pArg2->Eval().flag;
			return ret;
		}
	};

	/*
		ï™äÚ
		ëÊàÍà¯êîÇ…ÇÕboolånÇì¸ÇÍÇÈÇ±Ç∆
	*/
	class DsDecisionIfNode : public DsDecisionTreeNodeArg3
	{
	public:
		DsDecisionIfNode(DsDecisionTreeNode* pArg1, DsDecisionTreeNode* pArg2, DsDecisionTreeNode* pArg3)
			:DsDecisionTreeNodeArg3(DECISION_TREE_NODE_TYPE::IF, pArg1, pArg2, pArg3)
		{
			DS_ASSERT(IsDecitionTreeNodeBool(pArg1->GetType()), "IFÉmÅ[ÉhÇÃëÊàÍà¯êîÇ™boolÇ≈ÇÕÇ»Ç¢ÅBëÊàÍà¯êî");
		}
	public:
		virtual DsDecisionTreeData Eval() override
		{
			DsDecisionTreeData ret;
			if ( (m_pArg1->Eval().flag == 0)){
				ret = m_pArg2->Eval();
			}else{
				ret = m_pArg3->Eval();
			}
			return ret;
		}
	};

	/*
		èIí[
	*/
	class DsDecisionInputNode : public DsDecisionTreeNode
	{
	public:
		DsDecisionInputNode(DsDecisionTreeData& input)
			:DsDecisionTreeNode(DECISION_TREE_NODE_TYPE::INPUT)
			,m_inputData(input){}
	public:
		virtual DsDecisionTreeData Eval() override
		{
			return m_inputData;
		}
		virtual int GetArgNum() const override { return 0; }
		virtual void SetArg(int argIdx, DsDecisionTreeNode* pArg) override {}
		virtual DsDecisionTreeNode* GetArg(int argIdx) override { return NULL; }
	public:
		DsDecisionTreeData& RefData() { return m_inputData; }

	private:
		DsDecisionTreeData& m_inputData;
	};


}