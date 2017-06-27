#ifndef __DS_APP_COLLISION_FILTER__
#define __DS_APP_COLLISION_FILTER__


namespace DsPhysics
{
	typedef unsigned long long DsCollisionFilter;
}

namespace DsApp
{
	//同一グループでは当たらない

	//別オーナー
	enum class COLLISION_GROUP : unsigned int
	{
		NUM = 31,
	};

	//同一オーナー
	enum class INSIDE_COLLISION_GROUP : unsigned int
	{
		SKIN,
		CLOTHES,
		NUM=31,
	};

	class DsAppCollisionFilter
	{
	public:
		static DsPhysics::DsCollisionFilter CalcFilter(COLLISION_GROUP group, INSIDE_COLLISION_GROUP inside)
		{
			//32bit内部グループ | 32bitグループ
			return ((1 << 0xFFFFFFFF) << static_cast<unsigned int>(inside)) | (1<<static_cast<unsigned int>(group));
		}

		//内部グループのみ設定。グループは全てに当たる設定
		static DsPhysics::DsCollisionFilter CalcFilterInside(INSIDE_COLLISION_GROUP inside)
		{
			//32bit内部グループ | 32bitグループ
			return (1 << 0xFFFFFFFF) >> static_cast<unsigned int>(inside);
		}

		//グループが当たる関係か
		static bool IsGroupCollision(DsPhysics::DsCollisionFilter filter1, DsPhysics::DsCollisionFilter filter2)
		{
			return static_cast<bool>( ! ((filter1 & 0xFFFFFFFF) & (filter2 & 0xFFFFFFFF)) );
		}

		//内部グループが当たる関係か
		static bool IsInsideGroupCollision(DsPhysics::DsCollisionFilter filter1, DsPhysics::DsCollisionFilter filter2)
		{
			return static_cast<bool>( ! (((filter1 >> 32) & 0xFFFFFFFF) & ((filter2 >> 32) & 0xFFFFFFFF)) );
		}

	};
}

#endif