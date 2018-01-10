#ifndef _DS_ACTION_STATE_CTRL_
#define _DS_ACTION_STATE_CTRL_
namespace DsLib
{
	class DsASNode;
}

namespace DsLib
{
	class DsActionStateCtrl
	{
	public:
		DsActionStateCtrl();
		virtual ~DsActionStateCtrl();

	public:
		void RegisterActiveNode(DsASNode* pNode);
		void Update(double dt);
		std::vector<DsASNode*> GetActiveNode() const;

	private:
		std::vector<DsASNode*> m_execNodes;
		std::vector<DsASNode*> m_registerNodes;
	};


};


#endif
