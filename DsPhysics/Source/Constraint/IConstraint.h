#ifndef __ICONSTRAINT__
#define __ICONSTRAINT__

#pragma warning( disable : 4351 )

namespace DsPhysics
{
	class DsActorId;
	class DsPhysicsWorld;
	class IConstraint;
}

namespace DsPhysics
{
	//�ՓˑS�̂̍S������x�ɉ������߂̏��
	//w[t+1] = A�� + b 
	struct DsConstraintEquation
	{
		explicit DsConstraintEquation(IConstraint* pOwner)
			: wplus1()
			, A()
			, lambda()
			, b()
			, min_wplus1()
			, max_wplus1()
			, isConstLamdba()
			, m_pOwner(pOwner)
		{}

		double wplus1[6];
		double A[6][6];
		double lambda[6];
		double b[6];

		double min_wplus1[6];
		double max_wplus1[6];

		bool isConstLamdba[6];//�ɑ����Œ肷�邩�H���̃t���O�������Ă���ɐ����͊��m

		IConstraint* m_pOwner;
	};

	class IConstraint
	{
		public:
			virtual void CalclateConstraintForce() = 0;
			virtual void ApplyConstraintForce() = 0;
			virtual void SetUp() =0;
			virtual void UpdateExForce() = 0;
			virtual DsActorId GetMasterActorId() const = 0;
			virtual DsActorId GetSubActorId() const = 0;
			virtual DsConstraintEquation& RefEq() = 0;
	};
}

#endif