#include "DsPhysicsPch.h"
#ifndef __DS_BALL_CONSTRAINT__
#include "Constraint/DsBallConstraint.h"
#endif
#include "Actor/DsActor.h"
#include "DsMass.h"
#include "DsPhysicsWorld.h"


using namespace DsPhysics;

DsBallConstraint::DsBallConstraint(const DsActorId masterId, const DsActorId subId, const DsPhysicsWorld& world, const DsVec3d& pos, const DsVec3d& ex, const DsVec3d& ey, const DsVec3d& ez, const double dt, const DsVec3d& errorP, const DsMat33d& errorR)
	: DsConstraint(masterId, subId, world, pos, ex, ey, ez, dt)
{
}

DsBallConstraint::DsBallConstraint(const DsPhysicsWorld& world)
	: DsConstraint(world)
{

}

void DsBallConstraint::RequestAttach(const DsActorId masterId, const DsActorId subId, const DsVec3d& pos, const DsVec3d& ex, const DsVec3d& ey, const DsVec3d& ez, const double dt, const DsVec3d& errorP)
{
	m_masterId = masterId;
	m_subId = subId;
	m_pos = pos;
	m_ex = ex;
	m_ey = ey;
	m_ez = ez;
	m_dt = dt;
	m_errorP = errorP;
	m_masterF = DsVec3d::Zero();
	m_masterT = DsVec3d::Zero();
	m_subF = DsVec3d::Zero();
	m_subT = DsVec3d::Zero();
}

//virtual
void DsBallConstraint::SetUp()
{
	DsConstraint::SetUp();
}

//virtual 
void DsBallConstraint::_SetEquation( const double w[6])
{
	//�͂��s��
	RefEq().isConstLamdba[0] = false;
	RefEq().isConstLamdba[1] = false;
	RefEq().isConstLamdba[2] = false;
	//�g���N�͌��܂��Ă���
	RefEq().isConstLamdba[3] = true;
	RefEq().isConstLamdba[4] = true;
	RefEq().isConstLamdba[5] = true;

	//w[t+1]�̕��s�ړ������͂O�����ǁA�덷���C����������ɑ��x�𔭐�������
	const double k = 0.1;//�덷�C���Q�C��
	const double e[3] = { DsVec3d::Dot(m_ex, m_errorP), DsVec3d::Dot(m_ey, m_errorP), DsVec3d::Dot(m_ez, m_errorP) };
	const double ke[3] = { -e[0] * k / m_dt, -e[1] * k / m_dt, -e[2] * k / m_dt }; //K=1�łP�t���Ō덷���C�����鑬�x�B

	//���s�ړ������̖ڕW���x�͂킩���Ă���̂ŁA���
	//3,4,5���s�肾���A�g���N�͂������܂��Ă���̂Ŏg���邱�Ƃ͂Ȃ�
	RefEq().min_wplus1[0] = ke[0];
	RefEq().min_wplus1[1] = ke[1];
	RefEq().min_wplus1[2] = ke[2];
	RefEq().max_wplus1[0] = ke[0];
	RefEq().max_wplus1[1] = ke[1];
	RefEq().max_wplus1[2] = ke[2];

	//�g���N�͂킩���Ă���̂ő��
	//1,2,3�͑O��̒l�������l�Ƃ��Ďg���̂ŃN���A���Ȃ�
	RefEq().lambda[3] = 0;
	RefEq().lambda[4] = 0;
	RefEq().lambda[5] = 0;

	const double maxLambda = FLT_MAX;
	if (1) {//�ꔭ�ŉ���
			//A^1�����߂�
		const DsMat33d invA33 = DsMat33d::Inverse(DsMat33d::ToMat33(RefEq().A));
		const DsVec3d wt1_b3 = {
			RefEq().max_wplus1[0] - RefEq().b[0],
			RefEq().max_wplus1[1] - RefEq().b[1],
			RefEq().max_wplus1[2] - RefEq().b[2],
		};

		const DsVec3d lambda3 = invA33*wt1_b3;
		RefEq().lambda[0] = Clamp(lambda3.v[0], -maxLambda, maxLambda);
		RefEq().lambda[1] = Clamp(lambda3.v[1], -maxLambda, maxLambda);
		RefEq().lambda[2] = Clamp(lambda3.v[2], -maxLambda, maxLambda);
	}
	else {//��񕪂̌J��Ԃ��B
		DsConstraintEquation& eq = RefEq();
		//�P�s������6*6����̂ł���������Ă����BA�̒��̍s���[�v
		for (int row = 0; row < 6; ++row) {
			if (!eq.isConstLamdba[row]) {//�ɂ��Œ�l�̍s�͌v�Z���Ȃ��Ă����ɋ��܂��Ă�̂ŏ��O

				if (0.0000001 < fabs(eq.A[row][row]))
				{
					double forward = 0;//�X�V�ς�K+1
					for (int col = 0; col < row; ++col) {//����A�����̗�̃��[�v�Bforward�́A���ɉ����Ă���row�����܂ŉ��Z
						forward += eq.A[row][col] * eq.lambda[col];
					}

					double back = 0;//���X�V��k
					for (int col = row + 1; col < 6; ++col) { //forward+�������X�L�b�v���Ă���ȍ~�̗�����Z
						back += eq.A[row][col] * eq.lambda[col];
					}

					//dst[col] = (srcVec[col] - forward - back) / srcMat[col][col];
					const double w_b = eq.max_wplus1[row] - eq.b[row];
					const double weight = 1.0;
					const double old_lamdba = eq.lambda[row];
					double new_lambda = (w_b - forward - back) / eq.A[row][row];
					eq.lambda[row] = old_lamdba + weight*(new_lambda - old_lamdba);//SOR�@�B�K�E�X�U�C�f���̉��̕ω��ʂ�weight�{����

					eq.lambda[row] = Clamp(eq.lambda[row], -maxLambda, maxLambda);
				}
			}
		}
	}
}





