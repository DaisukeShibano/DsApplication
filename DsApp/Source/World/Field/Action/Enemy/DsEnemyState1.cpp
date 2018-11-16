#include "DsAppPch.h"
#include "World/Field/Action/Enemy/DsEnemyState1.h"
//他のヘッダ
#include "World/Field/Action/DsChrStateDefine.h"
#include "World/Field/Action/DsActionFlags.h"

using namespace DsApp;

namespace
{
	//ステートグラフID登録
	static const int DS_REGISTER_STATE_GRAPH_ID = DsChrStateDefine::ENEMY1_STATE_GRAPH_ID;


	/*********************************************************
	@brief 待機
	**********************************************************/
	class DsChrStateIdle : public DsEnemyState1
	{
	public:
		DsChrStateIdle(const INIT_ARG& arg) :DsEnemyState1(arg)
		{
			if (m_pAnimClip) {
				m_pAnimClip->SetLoop(true);
			}
		}

	private:
		virtual void Update(double dt) override
		{
			DsEnemyState1::Update(dt);

			switch (m_animFlags.GetDmgDir())
			{
			case DsDmgDir::F:
				m_nextState = CHR_STATE::DAMAGE_F;
				break;
			case DsDmgDir::B:
				m_nextState = CHR_STATE::DAMAGE_B;
				break;
			case DsDmgDir::L:
				m_nextState = CHR_STATE::DAMAGE_L;
				break;
			case DsDmgDir::R:
				m_nextState = CHR_STATE::DAMAGE_R;
				break;
			}

		};
	};
	DS_REGISTER_STATE(DsChrStateIdle)


	/*---------------------------------------------------------
	DsRegisterMyClass
	---------------------------------------------------------*/
	class DsRegisterMyClass : public DsRegisterClass
	{
	public:
		DsRegisterMyClass(std::vector<STATE_CLASS_TYPE>& classes, std::type_index* pTypes, int typeNum)
			:DsRegisterClass(classes, pTypes, typeNum)
		{}
		virtual int GetId() const override
		{
			return DsChrStateDefine::ENEMY1_STATE_GRAPH_ID;
		}
	};

}

/*---------------------------------------------------------
DsPlayerState
---------------------------------------------------------*/
//static
void DsEnemyState1::Initialize()
{
	//上書きするステートリストを登録します
	static std::vector<STATE_CLASS_TYPE> s_myClasses;
	static std::type_index s_myClassTypes[] =
	{
		typeid(DsChrStateIdle),
	};
	static DsRegisterMyClass s_registerMyClass(s_myClasses, s_myClassTypes, static_cast<int>(std::size(s_myClassTypes)));
	DsChrState::AddRegisterClass(s_registerMyClass);
}