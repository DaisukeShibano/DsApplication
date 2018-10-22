#ifndef _DS_CHR_STATE_
#define _DS_CHR_STATE_

namespace DsLib
{

}

namespace DsApp
{
	class DsActionRequest;
	class DsAnimEventFlags;
}

namespace DsApp
{
	enum class CHR_STATE : int
	{
		IDLE,
		RUN,
		ATTACK1,
		ATTACK2,
		ATTACK3,
		ATTACK4,
		DAMAGE_F,
		DAMAGE_B,
		DAMAGE_L,
		DAMAGE_R,
		NUM
	};

	class DsChrState;
	typedef std::map<CHR_STATE, DsChrState*> StateMap;

	/*--------------------------------------------------------------
	�X�e�[�g�^���
	---------------------------------------------------------------*/
	struct STATE_CLASS_TYPE
	{
		std::type_index type;
		CHR_STATE state;
		const char* pName;
	};

	/*--------------------------------------------------------------
	�X�e�[�g
	---------------------------------------------------------------*/
	class DsChrState : public DsLib::DsASNode
	{
	public:
		struct INIT_ARG
		{
			INIT_ARG(DsLib::DsAnimClip* arg1, DsActionRequest& arg2, StateMap& arg3, const DsAnimEventFlags& arg4, CHR_STATE arg5)
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
			CHR_STATE myState;
		};

	public:
		static size_t GetStateClassTypesNum();
		static STATE_CLASS_TYPE* GetStateClassTypes();
		static DsChrState* CreateIns(const INIT_ARG& arg);

	public:
		DsChrState(const INIT_ARG& arg)
			:m_pAnimClip(arg.pAnimClip), m_nextState(arg.myState), m_myState(arg.myState), m_actReq(arg.actReq), m_allState(arg.allState), m_animFlags(arg.animFlags), m_pNextStateNode(){}
		virtual ~DsChrState() {}

	public:
		DsLib::DsAnimClip* GetAnimClip()const { return m_pAnimClip; }

	protected:
		virtual void OnActive(double dt) override;
		virtual void Update(double dt) override;
		virtual void OnDeactive(double dt) override;
		virtual bool IsNext()const override { return m_nextState != m_myState; }//�f�t�H�����ŃX�e�[�g���ς������
		
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

	protected:
		DsLib::DsAnimClip* m_pAnimClip;
		CHR_STATE m_nextState;
		const CHR_STATE m_myState;
		DsActionRequest& m_actReq;
		const DsAnimEventFlags& m_animFlags;

	private:
		StateMap& m_allState;
		DsASNode* m_pNextStateNode[1];

	};
}

#endif // !_DS_CHR_STATE_
