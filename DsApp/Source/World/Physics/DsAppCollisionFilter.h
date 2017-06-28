#ifndef __DS_APP_COLLISION_FILTER__
#define __DS_APP_COLLISION_FILTER__


namespace DsPhysics
{
	typedef unsigned long long DsCollisionFilter;
}

namespace DsApp
{
	//����O���[�v�ł͓�����Ȃ�

	//�ʃI�[�i�[
	enum class COLLISION_GROUP : unsigned int
	{
		NUM = 31,
	};

	//����I�[�i�[
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
			//32bit�����O���[�v | 32bit�O���[�v
			return ((1ULL << 32ULL ) << static_cast<unsigned int>(inside)) | (1ULL <<static_cast<unsigned int>(group));
		}

		//�����O���[�v�̂ݐݒ�B�O���[�v�͑S�Ăɓ�����ݒ�
		static DsPhysics::DsCollisionFilter CalcFilterInside(INSIDE_COLLISION_GROUP inside)
		{
			//32bit�����O���[�v | 32bit�O���[�v
			return (1ULL << 32ULL ) << static_cast<unsigned int>(inside);
		}

		//�O���[�v��������֌W��
		static bool IsGroupCollision(DsPhysics::DsCollisionFilter filter1, DsPhysics::DsCollisionFilter filter2)
		{
			return static_cast<bool>( ! ((filter1 & 0xFFFFFFFFULL) & (filter2 & 0xFFFFFFFFULL)) );
		}

		//�����O���[�v��������֌W��
		static bool IsInsideGroupCollision(DsPhysics::DsCollisionFilter filter1, DsPhysics::DsCollisionFilter filter2)
		{
			return static_cast<bool>( ! (((filter1 >> 32ULL ) & 0xFFFFFFFFULL) & ((filter2 >> 32ULL ) & 0xFFFFFFFFULL)) );
		}

	};
}

#endif