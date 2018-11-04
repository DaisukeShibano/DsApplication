#pragma once

/*
���

�X�e�[�g�͂P��cpp�ɂP�L������z�肵���S�ĕ���������Ă���
�Ⴄ�p�^�[���̑J�ڂ�g�݂�����Ε�cpp��p�ӂ���
DsState.cpp �͋��ʏ���

�P��cpp��ID������U����
�L�������g���X�e�[�g�̓L�����p����ID�ƈ�v����cpp��ID���̗p�����
cpp�Ɋ���U����ID�Ƃ́ADsRegisterClass::GetId() �ŕԂ��l�̂���
DsRegisterClass�͊ecpp�Ōp�����ă��j�[�NID�����߂�
�ecpp�ł�static��DsRegisterClass�̔h���̃C���X�^���X�������
�R���X�g���N�^��AddRegisterClass�ɂĎ��g��o�^�BID���d�����Ă���΃G���[


�ecpp�ŃI�[�o�[���C�h�������X�e�[�g��DsState�Ɠ������O�ō��
��`�̎d����DsState�ƑS������
�X�e�[�g�ɑΉ������N���X��񂪏㏑�������悤�ɂȂ��Ă���

*/

namespace DsLib
{

}

namespace DsApp
{
	class DsActionRequest;
	class DsActionFlags;
	class DsStateFactory;
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
	�N���X�o�^
	---------------------------------------------------------------*/
	class DsRegisterClass
	{
	public:
		DsRegisterClass(std::vector<STATE_CLASS_TYPE>& classes, std::type_index* pTypes, int typeNum)
			: m_registerClasses(classes)
			, m_pOverwriteTypes(pTypes)
			, m_overrideTypesNum(typeNum)
		{}
		void Register();
		virtual int GetId() const = 0;

		const std::vector<STATE_CLASS_TYPE>& RefRegisterClasses() const { return m_registerClasses; }

	private:
		std::vector<STATE_CLASS_TYPE>& m_registerClasses;
		std::type_index* m_pOverwriteTypes;
		const int m_overrideTypesNum;
	};

	/*--------------------------------------------------------------
	�X�e�[�g
	---------------------------------------------------------------*/
	class DsChrState : public DsLib::DsASNode
	{
	public:
		struct INIT_ARG
		{
			INIT_ARG(DsLib::DsAnimClip* arg1, DsActionRequest& arg2, StateMap& arg3, DsActionFlags& arg4, CHR_STATE arg5)
				: pAnimClip(arg1)
				, actReq(arg2)
				, allState(arg3)
				, animFlags(arg4)
				, myState(arg5)

			{}
			DsLib::DsAnimClip* pAnimClip;
			DsActionRequest& actReq;
			StateMap& allState;
			DsActionFlags& animFlags;
			CHR_STATE myState;
		};

	public:
		static DsStateFactory* GetFactoryTop();
		static void SetFactoryTop(DsStateFactory*pFactory);
		static size_t GetStateClassTypesNum();
		static STATE_CLASS_TYPE* GetStateClassTypes();
		static DsChrState* CreateIns(INIT_ARG arg, const std::string& animName, const int id);
		static void AddRegisterClass(DsRegisterClass& registerClass);

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

	public:
		CHR_STATE GetMyState() const { return m_myState; }

	protected:
		DsLib::DsAnimClip* m_pAnimClip;
		CHR_STATE m_nextState;
		const CHR_STATE m_myState;
		DsActionRequest& m_actReq;
		DsActionFlags& m_animFlags;

	private:
		StateMap& m_allState;
		DsASNode* m_pNextStateNode[1];

	};

	/*--------------------------------------------------------------
	�X�e�[�g�t�@�N�g���[
	---------------------------------------------------------------*/
	class DsStateFactory
	{
	public:
		DsStateFactory() :m_pNext(NULL) {}

		virtual  DsChrState* Create(const DsChrState::INIT_ARG& arg) { return NULL; }
		virtual int GetId() const = 0;
		virtual std::type_index GetType() const = 0;

		void SetNext(DsStateFactory* pNext) { m_pNext = pNext; }
		DsStateFactory* GetNext() const { return m_pNext; }

	private:
		DsStateFactory * m_pNext;
	};

/*--------------------------------------------------------------
�X�e�[�g�t�@�N�g���[�R�[�h�����}�N��
DS_REGISTER_STATE(�N���X��)�Ƃ��邱�ƂŃO���[�o���̃t�@�N�g���[���X�g�ɒǉ������
DS_REGISTER_STATE_ID ���`���邱�ƁBDsChrStateDefine.h�ɒ�`����Ă���ID�ɑΉ�����
---------------------------------------------------------------*/
#define DS_REGISTER_STATE(ClassName)\
class ClassName##Factory : public DsStateFactory\
{\
public:\
	ClassName##Factory()\
		:DsStateFactory()\
	{\
		DsStateFactory* pFactoryTop = DsChrState::GetFactoryTop();\
		if(pFactoryTop){\
			DsStateFactory* pSet = pFactoryTop;\
			while(pSet){\
				DsStateFactory* pNext = pSet->GetNext();\
				if( pNext ){\
					pSet = pNext;\
				}else{\
					pSet->SetNext(this); \
					break; \
				}\
			}\
		}else{\
			DsChrState::SetFactoryTop(this);\
		}\
	}\
	virtual ClassName* Create(const DsChrState::INIT_ARG& arg) override { return new ClassName(arg); }\
	virtual int GetId() const override { return DS_REGISTER_STATE_GRAPH_ID; }\
	virtual std::type_index GetType() const override { return typeid(ClassName); }\
};\
static ClassName##Factory s_##ClassName##Factory;\

}