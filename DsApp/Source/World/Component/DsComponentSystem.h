#ifndef _DS_COMPONENT_SYSTEM_
#define _DS_COMPONENT_SYSTEM_

#ifndef _DS_COMPONENT_SYSTEM_DEFINE_
#include "World\/Component/DsComponentSystemDefine.h"
#endif

namespace  DsLib
{
	class DsSys;
}

namespace DsApp
{
	class DsGameSys;
	class DsFieldObj;
	class DsComponent;
	class DsItemBoxComponent;
	class DsAttachEntity;
	class DsEquipComponent;
	class DsLockOnComponent;
}

namespace DsApp
{
	class DsComponentSystem
	{
	public:
		//キーは、１キャラに１個ならオーナーのアドレス、アニメイベントならバーのアドレス、など
		struct KEY
		{
			explicit KEY(ds_uint64 _key)
				:key(_key)
			{}

			bool operator == (const KEY& r) const{
				return (key == r.key);
			}

			const ds_uint64 key;
		};

		class KeyHash
		{
		public:
			size_t operator()(const KEY& r) const {
				return r.key;
			}
		};

		class TypeHash
		{
		public:
			size_t operator()(const std::type_index& r) const {
				return r.hash_code();
			}
		};

	public:
		DsComponentSystem(DsFieldObj& owner, DsLib::DsSys& sys, DsPhysics::DsPhysicsWorld& physWorld, DsGameSys* pGameSys);
		virtual ~DsComponentSystem();

	public:
		void Update(double dt);

		const COMPONENT_UPDATE_RESULT& RefResult() const { return m_componentResult; }

	private:
		template<class CLASS>
		CLASS* _CreateGetComponent(ds_int64 key);

		template<class CLASS>
		CLASS* _CreateOneShotComponent();

		template<class CLASS>
		CLASS* _GetComponent(ds_int64 key)const;

	public:
		void RequestTraceEffect(ds_int64 key, int effectId, int dmypolyId0, int dmypolyId1);
		void RequestOneShotHitEffect(int effectId, const DsVec3d& hitPos, DsVec3d hitDir);
		void RequestSoundEffect(ds_int64 key, int soundId, int dmypolyId);
		void RequestEquip();
		void RequestItemBox();
		void RequestLockOnPoint();
		void RequestDamage(ds_int64 key, int damageId, int dmypolyId0, int dmypolyId1);
		void RequestKnockBack(const DsVec3d& move, double time);

	public:
		DsItemBoxComponent * GetItemBox()const;
		DsEquipComponent* GetEquip() const;

	private:
		typedef std::unordered_map<KEY, DsComponent*, KeyHash> KeyMap;

		std::unordered_map<std::type_index, KeyMap, TypeHash> m_components;
		std::list<DsComponent*> m_oneShotComponents;
		COMPONENT_UPDATE_RESULT m_componentResult;
		DsFieldObj& m_owner;
		DsLib::DsSys& m_sys;
		DsPhysics::DsPhysicsWorld& m_physWorld;
		DsGameSys* m_pGameSys;

	};




}


#endif