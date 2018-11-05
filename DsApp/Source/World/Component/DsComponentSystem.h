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
		//別のコンポーネントとキーが被らないよう、型+任意の数値でキーを表現する
		struct KEY
		{
			KEY(const std::type_info& _type, ds_uint64 _key)
				:type(_type)
				,key(_key)
			{}

			bool operator == (const KEY& r) const{
				return (type == r.type) && (key == r.key);
			}

			std::type_index type;
			ds_uint64 key;
		};

		class KeyHash
		{
		public:
			std::size_t operator()(const KEY& key) const {
				return key.type.hash_code() + key.key;
			}
		};

	public:
		DsComponentSystem(DsFieldObj& owner, DsLib::DsSys& sys, DsPhysics::DsPhysicsWorld& physWorld, DsGameSys* pGameSys);
		virtual ~DsComponentSystem();

	public:
		void Update(double dt);

	private:
		template<class CLASS>
		CLASS* _CreateGetComponent(ds_int64 key);

		template<class CLASS>
		CLASS* _GetComponent(ds_int64 key)const;

	public:
		void RequestTraceEffect(ds_int64 key, int effectId, int dmypolyId0, int dmypolyId1);
		void RequestSoundEffect(ds_int64 key, int soundId, int dmypolyId);
		void RequestEquip();
		void RequestItemBox();
		void RequestAttachWithUpdate(const DsMat44d& target, DsAttachEntity* pMove, double dt);
		void RequestLockOnPoint();
		void RequestDamage(ds_int64 key, int damageId, int dmypolyId0, int dmypolyId1);

	public:
		DsItemBoxComponent * GetItemBox()const;
		DsEquipComponent* GetEquip() const;

	private:
		std::unordered_map<KEY, DsComponent*, KeyHash> m_components;
		DsFieldObj& m_owner;
		DsLib::DsSys& m_sys;
		DsPhysics::DsPhysicsWorld& m_physWorld;
		DsGameSys* m_pGameSys;

	};




}


#endif