#pragma once

/*
解説

ステートは１つのcppに１キャラを想定した全て分が書かれている
違うパターンの遷移を組みたければ別cppを用意する
DsState.cpp は共通処理

１つのcppにIDが割り振られる
キャラが使うステートはキャラパラのIDと一致するcppのIDが採用される
cppに割り振られるIDとは、DsRegisterClass::GetId() で返す値のこと
DsRegisterClassは各cppで継承してユニークIDを決める
各cppではstaticでDsRegisterClassの派生のインスタンスが作られる
コンストラクタでAddRegisterClassにて自身を登録。IDが重複していればエラー


各cppでオーバーライドしたいステートをDsStateと同じ名前で作る
定義の仕方はDsStateと全く同じ
ステートに対応したクラス情報が上書きされるようになっている

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
	ステート型情報
	---------------------------------------------------------------*/
	struct STATE_CLASS_TYPE
	{
		std::type_index type;
		CHR_STATE state;
		const char* pName;
	};

	/*--------------------------------------------------------------
	クラス登録
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
	ステート
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
		virtual bool IsNext()const override { return m_nextState != m_myState; }//デフォ実装でステートが変わったら
		
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
	ステートファクトリー
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
ステートファクトリーコード生成マクロ
DS_REGISTER_STATE(クラス名)とすることでグローバルのファクトリーリストに追加される
DS_REGISTER_STATE_ID を定義すること。DsChrStateDefine.hに定義されているIDに対応する
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