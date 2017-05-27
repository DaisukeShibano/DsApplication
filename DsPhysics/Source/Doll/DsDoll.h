#ifndef _DS_DOLL_
#define _DS_DOLL_

namespace DsPhysics
{
	/*
		�p�[�c

		center ������K�܂ł̃��C��
		cenetr head ��
		cenetr arm ���@�r�����̈ʒu�ɒǏ]����
		center leg ���@�r�����̈ʒu�ɒǏ]����

		arm shoulder ��
		arm elbow �I
		arm hand ��

		leg hip �Ҋ֐�
		leg knee �G
		leg foot ��

	*/

	class DsDoll
	{
	public:
		
		enum PARTS_TYPE
		{
			CENTER_HEAD = 0,
			CENTER_ARM,
			CENTER_LEG,
			CENTER_END,

			ARM_SHOULDER,
			ARM_ELBOW,
			ARM_HAND,
			ARM_END,

			LEG_HIP,
			LEG_KNEE,
			LEG_FOOT,
			LEG_END,

			PARTS_NUM,
		};

	public:
		struct PartsInfo
		{
			DsVec3d pos[ PARTS_NUM ];
		};


	public:
		DsDoll( const PartsInfo& info );
		virtual ~DsDoll(){}

	public:
		void Update();

	private:
		void _UpdatePos();

	private:
		DsVec3d m_centerPos[CENTER_END-CENTER_HEAD];
		DsVec3d m_rightArmPos[ARM_END-ARM_SHOULDER];
		DsVec3d m_leftArmPos[ARM_END-ARM_SHOULDER];
		DsVec3d m_rightLegPos[LEG_END-LEG_HIP];
		DsVec3d m_leftLegPos[LEG_END-LEG_HIP];

		double m_centerAng[CENTER_END-CENTER_HEAD];
		double m_rightArmAng[ARM_END-ARM_SHOULDER];
		double m_leftArmAng[ARM_END-ARM_SHOULDER];
		double m_rightLegAng[LEG_END-LEG_HIP];
		double m_leftLegAng[LEG_END-LEG_HIP];

		double m_centerLen[CENTER_END-CENTER_HEAD];
		double m_rightArmLen[ARM_END-ARM_SHOULDER];
		double m_leftArmLen[ARM_END-ARM_SHOULDER];
		double m_rightLegLen[LEG_END-LEG_HIP];
		double m_leftLegLen[LEG_END-LEG_HIP];


	};
}

#endif