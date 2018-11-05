#ifndef __DS_APP_COLLISION_FILTER__
#define __DS_APP_COLLISION_FILTER__

/*
#include "World/Physics/DsAppCollisionFilter.h"
*/

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
		HIT,//地形
		DAMAGE,//ダメージ
		//MAX = 31,
	};

	//同一オーナー
	enum class INSIDE_COLLISION_GROUP : unsigned int
	{
		SKIN,
		CLOTHES,
		//MAX=31,
	};

	class DsAppCollisionFilter
	{
	public:
		static DsPhysics::DsCollisionFilter CalcFilter(COLLISION_GROUP group, INSIDE_COLLISION_GROUP inside)
		{
			//32bit内部グループ | 32bitグループ
			return ((1ULL << 32ULL ) << static_cast<unsigned int>(inside)) | (1ULL <<static_cast<unsigned int>(group));
		}

		//内部グループは全てに当たる設定。グループのみ設定
		static DsPhysics::DsCollisionFilter CalcFilterGroupInsideAllHit(COLLISION_GROUP group)
		{
			//32bit内部グループ | 32bitグループ
			return (1ULL) << static_cast<unsigned int>(group);
		}

		//内部グループは全てに当たらない設定。グループのみ設定
		static DsPhysics::DsCollisionFilter CalcFilterGroupInsideNoHit(COLLISION_GROUP group)
		{
			//32bit内部グループ | 32bitグループ
			return (0xFFFFFFFFULL << 32ULL) | (1ULL << static_cast<unsigned int>(group));
		}

		//内部グループのみ設定。グループは全てに当たる設定
		static DsPhysics::DsCollisionFilter CalcFilterInsideGroupAllHit(INSIDE_COLLISION_GROUP inside)
		{
			//32bit内部グループ | 32bitグループ
			return (1ULL << 32ULL ) << static_cast<unsigned int>(inside);
		}

		//内部グループのみ設定。グループは全てに当たらない設定
		static DsPhysics::DsCollisionFilter CalcFilterInsideGroupNoHit(INSIDE_COLLISION_GROUP inside)
		{
			//32bit内部グループ | 32bitグループ
			return ((1ULL << 32ULL) << static_cast<unsigned int>(inside)) | (0xFFFFFFFFULL);
		}

		//全ての内部グループに所属する設定。グループは全てに当たる設定
		static DsPhysics::DsCollisionFilter CalcFilterInsideAllGroup()
		{
			//32bit内部グループ | 32bitグループ
			return (0xFFFFFFFFULL << 32ULL);
		}

		//何も当たらない。内部グループ全てにする設定。グループ全てにする設定
		static DsPhysics::DsCollisionFilter CalcFilterAllOne()
		{
			//32bit内部グループ | 32bitグループ
			return (0xFFFFFFFFULL << 32ULL) | (0xFFFFFFFFULL);
		}


		//グループが当たる関係か
		static bool IsGroupCollision(DsPhysics::DsCollisionFilter filter1, DsPhysics::DsCollisionFilter filter2)
		{
			return static_cast<bool>( ! ((filter1 & 0xFFFFFFFFULL) & (filter2 & 0xFFFFFFFFULL)) );
		}

		//内部グループが当たる関係か
		static bool IsInsideGroupCollision(DsPhysics::DsCollisionFilter filter1, DsPhysics::DsCollisionFilter filter2)
		{
			return static_cast<bool>( ! (((filter1 >> 32ULL ) & 0xFFFFFFFFULL) & ((filter2 >> 32ULL ) & 0xFFFFFFFFULL)) );
		}

	};
}

#endif