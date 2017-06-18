#include "DsPhysicsPch.h"
#ifndef __DS_HINGE2_CONSTRAINT__
#include "Constraint/DsHinge2Constraint.h"
#endif
#include "DsMass.h"
#include "DsPhysicsWorld.h"

using namespace DsLib;
using namespace DsPhysics;


DsHinge2Constraint::DsHinge2Constraint(const DsActorId masterId, const DsActorId subId, const DsPhysicsWorld& world, const DsVec3d& posE, const DsVec3d& ex, const DsVec3d& ey, const DsVec3d& ez, const double dt, const DsVec3d& errorP, const DsMat33d& errorR)
	:DsBallConstraint(masterId, subId, world, posE, ex, ey, ez, dt, errorP, errorR)
	, m_axisMas1(ez)
	, m_axisSub1(ez)
	, m_axisMas2(ey)
	, m_axisSub2(ey)
	, m_lambda()
{
	//z軸が回転軸

}
DsHinge2Constraint::DsHinge2Constraint(const DsPhysicsWorld& world)
	:DsBallConstraint(world)
	, m_axisMas1(DsVec3d::Zero())
	, m_axisSub1(DsVec3d::Zero())
	, m_axisMas2(DsVec3d::Zero())
	, m_axisSub2(DsVec3d::Zero())
	, m_lambda()
{
}

//virtual
void DsHinge2Constraint::SetUp()
{
	DsConstraint::SetUp();
	
	double eRX = 0;
	{
		//yz回転すればx回転成分も生まれてしまうので何を誤差とするかの判別がつかない
		const DsVec3d error = DsVec3d::Cross(m_axisMas1, m_axisSub1);
		eRX = DsVec3d::Dot(m_ex, error);
		
		const DsVec3d error2 = DsVec3d::Cross(m_axisMas2, m_axisSub2);
		double eRX2 = DsVec3d::Dot(m_ex, error2);
		eRX += eRX2;
		
		double check = fabs(fabs(eRX) - fabs(eRX2));
		if (0.00001 < check) {
			DS_ASSERT(true, "");
		}
	}
	
	const DsVec3d eP = { m_errorP.x, m_errorP.y, m_errorP.z };
	const double kp = 0.2;//誤差修正ゲイン
	const double kr = 0.02;//誤差修正ゲイン。回転//多分正しくないだろうけど、フリーになったときゆっくりと元に戻る程度
	const double e[4] = { DsVec3d::Dot(m_ex, eP), DsVec3d::Dot(m_ey, eP), DsVec3d::Dot(m_ez, eP), eRX};
	const double ke[4] = { -e[0] * kp / m_dt, -e[1] * kp / m_dt, -e[2] * kp / m_dt, -e[3] * kr / m_dt}; //K=1で１フレで誤差を修正する速度。

	//目標速度が分かっている成分を代入
	//[4][5]が不定だが、トルクは求まっているので使われることはない
	RefEq().min_wplus1[0] = ke[0];
	RefEq().min_wplus1[1] = ke[1];
	RefEq().min_wplus1[2] = ke[2];
	RefEq().min_wplus1[3] = ke[3];
	
	RefEq().max_wplus1[0] = ke[0];
	RefEq().max_wplus1[1] = ke[1];
	RefEq().max_wplus1[2] = ke[2];
	RefEq().max_wplus1[3] = ke[3];
}

//virtual 
void DsHinge2Constraint::_SetEquation(const double w[6])
{
	//ヒンジはz軸のトルクだけ既知。それ以外は未知
	RefEq().isConstLamdba[0] = false;
	RefEq().isConstLamdba[1] = false;
	RefEq().isConstLamdba[2] = false;
	RefEq().isConstLamdba[3] = false;
	RefEq().isConstLamdba[4] = true;
	RefEq().isConstLamdba[5] = true;

	//z軸のトルクはわかっているので代入
	//1,2,3,4は前回の値を初期値として使うのでクリアしない
	//RefEq().lambda[0] = 0;
	//RefEq().lambda[1] = 0;
	//RefEq().lambda[2] = 0;
	//RefEq().lambda[3] = 0;
	RefEq().lambda[4] = 0;
	RefEq().lambda[5] = 0;

	const double maxLambda = FLT_MAX;
	if (1){//一発で解く
		//A^1を求める
		double A44[4][4];
		for (int i = 0; i < 4; ++i){
			for (int j = 0; j < 4; ++j){
				A44[i][j] = RefEq().A[i][j];
			}
		}
		double invA44[4][4];
		const bool isInvOK = DsInverseMatrix<4, double>(A44, invA44);
		if (!isInvOK){
			return;//逆行列求められなかったので拘束力はかけずに終わる
		}

		const double wt1_b4[4] = {
			RefEq().max_wplus1[0] - RefEq().b[0],
			RefEq().max_wplus1[1] - RefEq().b[1],
			RefEq().max_wplus1[2] - RefEq().b[2],
			RefEq().max_wplus1[3] - RefEq().b[3],
		};

		double lambda4[4];
		DsMathUtil::MultiVec<4, 4>(lambda4, invA44, wt1_b4);
		RefEq().lambda[0] = Clamp(lambda4[0], -maxLambda, maxLambda);
		RefEq().lambda[1] = Clamp(lambda4[1], -maxLambda, maxLambda);
		RefEq().lambda[2] = Clamp(lambda4[2], -maxLambda, maxLambda);
		RefEq().lambda[3] = Clamp(lambda4[3], -maxLambda, maxLambda);
	}
	else{//一回分の繰り返し。
		DsConstraintEquation& eq = RefEq();
		//１行あたり6*6あるのでそれを解いていく。Aの中の行ループ
		for (int row = 0; row < 6; ++row){	
			if (!eq.isConstLamdba[row]){//λが固定値の行は計算しなくても既に求まってるので除外

				if (0.0000001 < fabs(eq.A[row][row]))
				{
					double forward = 0;//更新済みK+1
					for (int col = 0; col < row; ++col){//一つ一つのA成分の列のループ。forwardは、既に解いてあるrow成分まで加算
						forward += eq.A[row][col] * eq.lambda[col];
					}

					double back = 0;//未更新分k
					for (int col = row + 1; col < 6; ++col){ //forward+自分をスキップしてそれ以降の列を加算
						back += eq.A[row][col] * eq.lambda[col];
					}

					//dst[col] = (srcVec[col] - forward - back) / srcMat[col][col];
					const double w_b = eq.max_wplus1[row] - eq.b[row];
					const double weight = 1.0;
					const double old_lamdba = eq.lambda[row];
					double new_lambda = (w_b - forward - back) / eq.A[row][row];
					eq.lambda[row] = old_lamdba + weight*( new_lambda - old_lamdba );//SOR法。ガウスザイデルの解の変化量をweight倍加速
					
					eq.lambda[row] = Clamp(eq.lambda[row], -maxLambda, maxLambda);
				}
			}
		}
	}
}
