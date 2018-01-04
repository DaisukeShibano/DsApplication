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
	};

	class DsChrState;
	typedef std::map<CHR_STATE, DsChrState*> StateMap;

	class DsChrState : public DsLib::DsASNode
	{
	public:
		static DsChrState* CreateIns(CHR_STATE state, StateMap& allState, DsLib::DsAnimClip* pAnimClip, const DsActionRequest& actReq);

	public:
		DsChrState(StateMap& allState, DsLib::DsAnimClip* pAnimClip, const DsActionRequest& actReq)
			:m_pAnimClip(pAnimClip), m_nextState(CHR_STATE::IDLE), m_actReq(actReq), m_allState(allState), m_pNextStateNode(){}
		virtual ~DsChrState() {}

	public:
		DsLib::DsAnimClip* GetAnimClip()const { return m_pAnimClip; }

	protected:
		virtual void OnActive(double dt) override {};
		virtual void Update(double dt) override {};
		virtual void OnDeactive(double dt) override {};
		virtual bool IsNext()const override { return m_nextState != GetMyState(); }//デフォ実装でステートが変わったら
		
	private:
		virtual int GetNextNum() const override final{ return 1; }
		virtual DsASNode** GetNext() override final
		{
			auto it = m_allState.find(GetMyState());
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
		const DsActionRequest& m_actReq;

	private:
		StateMap& m_allState;
		DsASNode* m_pNextStateNode[1];

	};
}

#endif // !_DS_CHR_STATE_
