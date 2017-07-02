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
		HIT,
		//MAX = 31,
	};

	//����I�[�i�[
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
			//32bit�����O���[�v | 32bit�O���[�v
			return ((1ULL << 32ULL ) << static_cast<unsigned int>(inside)) | (1ULL <<static_cast<unsigned int>(group));
		}

		//�����O���[�v�͑S�Ăɓ�����ݒ�B�O���[�v�̂ݐݒ�
		static DsPhysics::DsCollisionFilter CalcFilterGroup(COLLISION_GROUP group)
		{
			//32bit�����O���[�v | 32bit�O���[�v
			return (1ULL) << static_cast<unsigned int>(group);
		}

		//�����O���[�v�̂ݐݒ�B�O���[�v�͑S�Ăɓ�����ݒ�
		static DsPhysics::DsCollisionFilter CalcFilterInside(INSIDE_COLLISION_GROUP inside)
		{
			//32bit�����O���[�v | 32bit�O���[�v
			return (1ULL << 32ULL ) << static_cast<unsigned int>(inside);
		}

		//�S�Ă̓����O���[�v�ɏ�������ݒ�B�O���[�v�͑S�Ăɓ�����ݒ�
		static DsPhysics::DsCollisionFilter CalcFilterInsideAllGroup()
		{
			//32bit�����O���[�v | 32bit�O���[�v
			return (0xFFFFFFFFULL << 32ULL);
		}

		//�����O���[�v�S�Ăɂ���ݒ�B�O���[�v�S�Ăɂ���ݒ�
		static DsPhysics::DsCollisionFilter CalcFilterAllOne()
		{
			//32bit�����O���[�v | 32bit�O���[�v
			return (0xFFFFFFFFULL << 32ULL) | (0xFFFFFFFFULL);
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