#ifndef _DS_CHR_STATE_
#define _DS_CHR_STATE_

namespace DsLib
{

}

namespace DsApp
{
	class DsActionRequest;
}

namespace DsApp
{
	enum class CHR_STATE : int
	{
		IDLE,
		RUN,
		NUM
	};

	class DsChrState;
	typedef std::map<CHR_STATE, DsChrState*> StateMap;

	class DsChrState : public DsLib::DsASNode
	{
	public:
		struct INIT_ARG
		{
			INIT_ARG(DsLib::DsAnimClip* arg1, DsActionRequest& arg2, StateMap& arg3, CHR_STATE arg4)
				: pAnimClip(arg1)
				, actReq(arg2)
				, allState(arg3)
				, myState(arg4)

			{}
			DsLib::DsAnimClip* pAnimClip;
			DsActionRequest& actReq;
			StateMap& allState;
			CHR_STATE myState;
		};

	public:
		static DsChrState* CreateIns(const INIT_ARG& arg);

	public:
		DsChrState(const INIT_ARG& arg)
			:m_pAnimClip(arg.pAnimClip), m_nextState(arg.myState), m_actReq(arg.actReq), m_allState(arg.allState), m_pNextStateNode(){}
		virtual ~DsChrState() {}

	public:
		DsLib::DsAnimClip* GetAnimClip()const { return m_pAnimClip; }

	protected:
		virtual void OnActive(double dt) override { m_nextState = GetMyState(); }//デフォ実装では次のステートに行かないように設定
		virtual void Update(double dt) override {};
		virtual void OnDeactive(double dt) override {};
		virtual bool IsNext()const override { return m_nextState != GetMyState(); }//デフォ実装でステートが変わったら
		
	private:
		virtual int GetNextNum() const override final{ return 1; }
		virtual DsASNode** GetNext() override final
		{
			auto it = m_allState.find(m_nextState);
			if (it != m_allState.end()) {
				m_pNextStateNode[0] = it->second;
				return m_pNextStateNode;
			}
			else {
				return NULL;
			}
		}

	public:
		virtual CHR_STATE GetMyState() const = 0;

	protected:
		DsLib::DsAnimClip* m_pAnimClip;
		CHR_STATE m_nextState;
		DsActionRequest& m_actReq;

	private:
		StateMap& m_allState;
		DsASNode* m_pNextStateNode[1];

	};
}

#endif // !_DS_CHR_STATE_
