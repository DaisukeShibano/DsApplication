#pragma once

namespace DsApp
{
	class DsActionRequest;
	class DsAnimEventFlags;
}

namespace DsApp
{
	enum class ENEMY_STATE : int
	{
		IDLE,
		RUN,
		ATTACK1,
		ATTACK2,
		NUM
	};

	class DsEnemyState;
	typedef std::map<ENEMY_STATE, DsEnemyState*> StateMap;

	class DsEnemyState : public DsLib::DsASNode
	{
	public:
		struct INIT_ARG
		{
			INIT_ARG(DsLib::DsAnimClip* arg1, DsActionRequest& arg2, StateMap& arg3, const DsAnimEventFlags& arg4, ENEMY_STATE arg5)
				: pAnimClip(arg1)
				, actReq(arg2)
				, allState(arg3)
				, animFlags(arg4)
				, myState(arg5)

			{}
			DsLib::DsAnimClip* pAnimClip;
			DsActionRequest& actReq;
			StateMap& allState;
			const DsAnimEventFlags& animFlags;
			ENEMY_STATE myState;
		};

	public:
		static DsEnemyState* CreateIns(const INIT_ARG& arg);

	public:
		DsEnemyState(const INIT_ARG& arg)
			:m_pAnimClip(arg.pAnimClip), m_nextState(arg.myState), m_actReq(arg.actReq), m_allState(arg.allState), m_animFlags(arg.animFlags), m_pNextStateNode() {}
		virtual ~DsEnemyState() {}

	public:
		DsLib::DsAnimClip* GetAnimClip()const { return m_pAnimClip; }

	protected:
		virtual void OnActive(double dt) override;
		virtual void Update(double dt) override;
		virtual void OnDeactive(double dt) override;
		virtual bool IsNext()const override { return m_nextState != GetMyState(); }//デフォ実装でステートが変わったら

	private:
		virtual int GetNextNum() const override final { return 1; }
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
		virtual ENEMY_STATE GetMyState() const = 0;

	protected:
		DsLib::DsAnimClip* m_pAnimClip;
		ENEMY_STATE m_nextState;
		DsActionRequest& m_actReq;
		const DsAnimEventFlags& m_animFlags;

	private:
		StateMap & m_allState;
		DsASNode* m_pNextStateNode[1];

	};
}