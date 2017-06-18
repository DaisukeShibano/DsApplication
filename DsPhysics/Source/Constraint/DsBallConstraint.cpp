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
	//力が不明
	RefEq().isConstLamdba[0] = false;
	RefEq().isConstLamdba[1] = false;
	RefEq().isConstLamdba[2] = false;
	//トルクは決まっている
	RefEq().isConstLamdba[3] = true;
	RefEq().isConstLamdba[4] = true;
	RefEq().isConstLamdba[5] = true;

	//w[t+1]の平行移動成分は０だけど、誤差を修正する方向に速度を発生させる
	const double k = 0.1;//誤差修正ゲイン
	const double e[3] = { DsVec3d::Dot(m_ex, m_errorP), DsVec3d::Dot(m_ey, m_errorP), DsVec3d::Dot(m_ez, m_errorP) };
	const double ke[3] = { -e[0] * k / m_dt, -e[1] * k / m_dt, -e[2] * k / m_dt }; //K=1で１フレで誤差を修正する速度。

	//平行移動成分の目標速度はわかっているので、代入
	//3,4,5が不定だが、トルクはもう求まっているので使われることはない
	RefEq().min_wplus1[0] = ke[0];
	RefEq().min_wplus1[1] = ke[1];
	RefEq().min_wplus1[2] = ke[2];
	RefEq().max_wplus1[0] = ke[0];
	RefEq().max_wplus1[1] = ke[1];
	RefEq().max_wplus1[2] = ke[2];

	//トルクはわかっているので代入
	//1,2,3は前回の値を初期値として使うのでクリアしない
	RefEq().lambda[3] = 0;
	RefEq().lambda[4] = 0;
	RefEq().lambda[5] = 0;

	const double maxLambda = FLT_MAX;
	if (1) {//一発で解く
			//A^1を求める
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
	else {//一回分の繰り返し。
		DsConstraintEquation& eq = RefEq();
		//１行あたり6*6あるのでそれを解いていく。Aの中の行ループ
		for (int row = 0; row < 6; ++row) {
			if (!eq.isConstLamdba[row]) {//λが固定値の行は計算しなくても既に求まってるので除外

				if (0.0000001 < fabs(eq.A[row][row]))
				{
					double forward = 0;//更新済みK+1
					for (int col = 0; col < row; ++col) {//一つ一つのA成分の列のループ。forwardは、既に解いてあるrow成分まで加算
						forward += eq.A[row][col] * eq.lambda[col];
					}

					double back = 0;//未更新分k
					for (int col = row + 1; col < 6; ++col) { //forward+自分をスキップしてそれ以降の列を加算
						back += eq.A[row][col] * eq.lambda[col];
					}

					//dst[col] = (srcVec[col] - forward - back) / srcMat[col][col];
					const double w_b = eq.max_wplus1[row] - eq.b[row];
					const double weight = 1.0;
					const double old_lamdba = eq.lambda[row];
					double new_lambda = (w_b - forward - back) / eq.A[row][row];
					eq.lambda[row] = old_lamdba + weight*(new_lambda - old_lamdba);//SOR法。ガウスザイデルの解の変化量をweight倍加速

					eq.lambda[row] = Clamp(eq.lambda[row], -maxLambda, maxLambda);
				}
			}
		}
	}
}





