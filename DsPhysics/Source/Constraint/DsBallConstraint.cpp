#include "DsPhysicsPch.h"
#ifndef __DS_BALL_CONSTRAINT__
#include "Constraint/DsBallConstraint.h"
#endif
#include "Actor/DsActor.h"
#include "DsMass.h"
#include "DsPhysicsWorld.h"


using namespace DsPhysics;

#if 0

namespace
{
	template<int ROW, int COL>
	void __MultiMat( double dst[ROW][COL], const double src1[ROW][12], const double src2[12][COL] )
	{
		for(int i=0; ROW>i; ++i)
		{
			for(int j=0; COL>j; ++j)
			{
				dst[i][j] = 0.0;
				for(int k=0; ROW>k; ++k)
				{
					dst[i][j] += src1[i][k]*src2[k][j];
				}
			}
		}
	}


	template<int ROW, int COL>
	void __MultiMat( double dst[ROW][COL], const double src1[ROW][6], const double src2[6][COL] )
	{
		for(int i=0; ROW>i; ++i)
		{
			for(int j=0; COL>j; ++j)
			{
				dst[i][j] = 0.0;
				for(int k=0; ROW>k; ++k)
				{
					dst[i][j] += src1[i][k]*src2[k][j];
				}
			}
		}
	}

	template<int ROW, int COL>
	void __MultiVec( double dst[ROW], const double src1[ROW][COL], const double src2[COL] )
	{
		for(int i=0; ROW>i; ++i)
		{
			dst[i] = 0.0;
			for(int j=0; COL>j; ++j)
			{
				dst[i] += src1[i][j]*src2[j];
			}
		}
	}
};


DsBallConstraint::DsBallConstraint(const DsActor& master, const DsActor& sub, const DsVec3d& posE, const DsVec3d& ex, const DsVec3d& ey, const DsVec3d& ez, const double dt )
:m_dt(dt)
{
	const DsVec3d& posA = master.GetPosition();
	const DsVec3d& posB = sub.GetPosition();
	const DsVec3d ra = posE - posA;
	const DsVec3d rb = posE - posB;
	const DsVec3d ra_ex = DsVec3d::Cross(ra,ex);
	const DsVec3d ra_ey = DsVec3d::Cross(ra,ey);
	const DsVec3d ra_ez = DsVec3d::Cross(ra,ez);
	const DsVec3d rb_ex = DsVec3d::Cross(rb,ex);
	const DsVec3d rb_ey = DsVec3d::Cross(rb,ey);
	const DsVec3d rb_ez = DsVec3d::Cross(rb,ez);

	m_J[0][0]=-ex.x; m_J[0][1]=-ex.y; m_J[0][2]=-ex.z; m_J[0][3]=-ra_ex.x; m_J[0][4]=-ra_ex.y; m_J[0][5]=-ra_ex.z; m_J[0][6]=ex.x; m_J[0][7]=ex.y; m_J[0][8]=ex.z; m_J[0][9]=rb_ex.x; m_J[0][10]=rb_ex.y; m_J[0][11]=rb_ex.z;
	m_J[1][0]=-ey.x; m_J[1][1]=-ey.y; m_J[1][2]=-ey.z; m_J[1][3]=-ra_ey.x; m_J[1][4]=-ra_ey.y; m_J[1][5]=-ra_ey.z; m_J[1][6]=ey.x; m_J[1][7]=ey.y; m_J[1][8]=ey.z; m_J[1][9]=rb_ey.x; m_J[1][10]=rb_ey.y; m_J[1][11]=rb_ey.z;
	m_J[2][0]=-ez.x; m_J[2][1]=-ez.y; m_J[2][2]=-ez.z; m_J[2][3]=-ra_ez.x; m_J[2][4]=-ra_ez.y; m_J[2][5]=-ra_ez.z; m_J[2][6]=ez.x; m_J[2][7]=ez.y; m_J[2][8]=ez.z; m_J[2][9]=rb_ez.x; m_J[2][10]=rb_ez.y; m_J[2][11]=rb_ez.z;
	m_J[3][0]=0.0; m_J[3][1]=0.0; m_J[3][2]=0.0; m_J[3][3]=-ex.x; m_J[3][4]=-ex.y; m_J[3][5]=-ex.z; m_J[3][6]=0.0; m_J[3][7]=0.0; m_J[3][8]=0.0; m_J[3][9]=ex.x; m_J[3][10]=ex.y; m_J[3][11]=ex.z;
	m_J[4][0]=0.0; m_J[4][1]=0.0; m_J[4][2]=0.0; m_J[4][3]=-ey.x; m_J[4][4]=-ey.y; m_J[4][5]=-ey.z; m_J[4][6]=0.0; m_J[4][7]=0.0; m_J[4][8]=0.0; m_J[4][9]=ey.x; m_J[4][10]=ey.y; m_J[4][11]=ey.z;
	m_J[5][0]=0.0; m_J[5][1]=0.0; m_J[5][2]=0.0; m_J[5][3]=-ez.x; m_J[5][4]=-ez.y; m_J[5][5]=-ez.z; m_J[5][6]=0.0; m_J[5][7]=0.0; m_J[5][8]=0.0; m_J[5][9]=ez.x; m_J[5][10]=ez.y; m_J[5][11]=ez.z;

	for(int i=0; 6>i; ++i)
	{
		for(int j=0; 12>j; ++j)
		{
			m_Jt[j][i] = m_J[i][j];
		}
	}

	_SetVel(master, sub);
	_SetMass(master, sub);
	_SetExForce(master, sub);
}

void DsBallConstraint::_SetVel(const DsActor& master, const DsActor& sub )
{
	m_u[0] = master.GetVelocity().x;
	m_u[1] = master.GetVelocity().y;
	m_u[2] = master.GetVelocity().z;
	m_u[3] = master.GetAngularVel().x;
	m_u[4] = master.GetAngularVel().y;
	m_u[5] = master.GetAngularVel().z;
	m_u[6] = sub.GetVelocity().x;
	m_u[7] = sub.GetVelocity().y;
	m_u[8] = sub.GetVelocity().z;
	m_u[9] = sub.GetAngularVel().x;
	m_u[10] = sub.GetAngularVel().y;
	m_u[11] = sub.GetAngularVel().z;
}

void DsBallConstraint::_SetMass( const DsActor& master, const DsActor& sub )
{
	const DsMass& m1 = master.GetMass();
	const DsMass& m2 = sub.GetMass();

	for(int i=0; 12>i; ++i)
	{
		for(int j=0; 12>j; ++j)
		{
			m_M[i][j]=0.0;
		}
	}

	m_M[0][0] = m1.mass; m_M[1][1] = m1.mass; m_M[2][2] = m1.mass;
	m_M[3][3] = m1.inertia[0]; m_M[3][4] = m1.inertia[1]; m_M[3][5] = m1.inertia[2];
	m_M[4][3] = m1.inertia[3]; m_M[4][4] = m1.inertia[4]; m_M[4][5] = m1.inertia[5];
	m_M[5][3] = m1.inertia[6]; m_M[5][4] = m1.inertia[7]; m_M[5][5] = m1.inertia[8];
	m_M[6][6] = m2.mass; m_M[7][7] = m2.mass; m_M[8][8] = m2.mass;
	m_M[9][9] = m2.inertia[0]; m_M[9][10] = m2.inertia[1]; m_M[9][11] = m2.inertia[2];
	m_M[10][9] = m2.inertia[3]; m_M[10][10] = m2.inertia[4]; m_M[10][11] = m2.inertia[5];
	m_M[11][9] = m2.inertia[6]; m_M[11][10] = m2.inertia[7]; m_M[11][11] = m2.inertia[8];

	DsInverseMatrix<12>(m_M, m_invM);
}

void DsBallConstraint::_SetExForce( const DsActor& master, const DsActor& sub )
{
	const DsVec3d& master_exF = master.GetForce();
	const DsVec3d& master_exT = master.GetTorque();
	const DsVec3d& sub_exF = sub.GetForce();
	const DsVec3d& sub_exT = sub.GetTorque();

	m_Fe[0]=master_exF.x; m_Fe[1]=master_exF.y; m_Fe[2]=master_exF.z; m_Fe[3]=master_exT.x; m_Fe[4]=master_exT.y; m_Fe[5]=master_exT.z,
	m_Fe[6]=sub_exF.x;    m_Fe[7]=sub_exF.y;    m_Fe[8]=sub_exF.z;    m_Fe[9]=sub_exT.x;    m_Fe[10]=sub_exT.y;   m_Fe[11]=sub_exT.z;


}


void DsBallConstraint::CalcConatraintForce( DsVec3d& masterF, DsVec3d& masterT, DsVec3d& subF, DsVec3d& subT, const ConstraintType& constraint  )
{
	double invMJt[12][6];
	__MultiMat<12,6>(invMJt, m_invM, m_Jt);

	double JinvMJt[6][6];
	__MultiMat<6,6>(JinvMJt, m_J, invMJt);

	
	
	double invMFe[12];
	__MultiVec<12,12>(invMFe, m_invM, m_Fe);

	double JinvMFe[6];
	__MultiVec<6,12>(JinvMFe, m_J, invMFe);



	double w[6];
	__MultiVec<6,12>(w, m_J, m_u);

	//w[t+1] = Ax+b
	//A = JinvMJt * dt
	//b = w[t] + JinvMFe * dt

	double A[6][6];
	for(int i=0; 6>i; ++i)
	{
		for(int j=0; 6>j; ++j)
		{
			A[i][j] = JinvMJt[i][j] * m_dt;
		}
	}
	double b[6] = { w[0]+JinvMFe[0]*m_dt, w[1]+JinvMFe[1]*m_dt, w[2]+JinvMFe[2]*m_dt, 
					w[3]+JinvMFe[3]*m_dt, w[4]+JinvMFe[4]*m_dt, w[5]+JinvMFe[5]*m_dt };

	double lambda[6];
	if( constraint.Solve( lambda, w, A, b ) )
	{
		double Fc[12];
		__MultiVec<12,6>(Fc, m_Jt,lambda);

		masterF.x = Fc[0];
		masterF.y = Fc[1];
		masterF.z = Fc[2];
		masterT.x = Fc[3];
		masterT.y = Fc[4];
		masterT.z = Fc[5];
		subF.x = Fc[6];
		subF.y = Fc[7];
		subF.z = Fc[8];
		subT.x = Fc[9];
		subT.y = Fc[10];
		subT.z = Fc[11];
	}
	else
	{
		masterF.x = 0.0;
		masterF.y = 0.0;
		masterF.z = 0.0;
		masterT.x = 0.0;
		masterT.y = 0.0;
		masterT.z = 0.0;
		subF.x = 0.0;
		subF.y = 0.0;
		subF.z = 0.0;
		subT.x = 0.0;
		subT.y = 0.0;
		subT.z = 0.0;
	}
}



bool DsBallConstraint::Collision::Solve( double dst[6], const double w[6], const double A[6][6], const double b[6] ) const
{
	const double wb[6] = { -b[0], -b[1], -b[2], -b[3], -b[4], -b[5] }; //相対速度0
	double invA[6][6];
	if( !DsInverseMatrix<6>(A, invA) )
	{
		return false;
	}
	__MultiVec<6,6>(dst, invA, wb);


	return true;
}

#else
namespace
{
	template<int ROW, int COL>
	void __MultiMat(double dst[ROW][COL], const double src1[ROW][12], const double src2[12][COL])
	{
		for (int i = 0; ROW>i; ++i)
		{
			for (int j = 0; COL>j; ++j)
			{
				dst[i][j] = 0.0;
				for (int k = 0; ROW>k; ++k)
				{
					dst[i][j] += src1[i][k] * src2[k][j];
				}
			}
		}
	}


	template<int ROW, int COL>
	void __MultiMat(double dst[ROW][COL], const double src1[ROW][6], const double src2[6][COL])
	{
		for (int i = 0; ROW>i; ++i)
		{
			for (int j = 0; COL>j; ++j)
			{
				dst[i][j] = 0.0;
				for (int k = 0; ROW>k; ++k)
				{
					dst[i][j] += src1[i][k] * src2[k][j];
				}
			}
		}
	}

	template<int ROW, int COL>
	void __MultiVec(double dst[ROW], const double src1[ROW][COL], const double src2[COL])
	{
		for (int i = 0; ROW>i; ++i)
		{
			dst[i] = 0.0;
			for (int j = 0; COL>j; ++j)
			{
				dst[i] += src1[i][j] * src2[j];
			}
		}
	}
};



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

	

#if 0//ここでは解かない
	//既に相対速度０なら解散
	if ((fabs(b[0]) < 0.0001) && (fabs(b[1]) < 0.0001) && (fabs(b[2]) < 0.0001) && (m_errorP.Length() < 0.0001))
	{
		dstLambda[0] = 0.0;
		dstLambda[1] = 0.0;
		dstLambda[2] = 0.0;
		dstLambda[3] = 0.0;
		dstLambda[4] = 0.0;
		dstLambda[5] = 0.0;
		return true;
	}

	double wtplus1[6];
	//ボールジョイントは、速度が０で角速度が未知。拘束力が未知で、拘束トルクが０
	//w[t+1] の上３つが０で、下３つが未知。lambda の上３つが未知で、下３つが未知、ということ


	if (iteCounter == 0)
	{
		//w[t+1]の平行移動成分は０だけど、誤差を修正する方向に速度を発生させる
		const double k = 0.1;//誤差修正ゲイン
		const double e[3] = { DsVec3d::Dot(m_ex, m_errorP), DsVec3d::Dot(m_ey, m_errorP), DsVec3d::Dot(m_ez, m_errorP) };
		const double ke[3] = { -e[0] * k / m_dt, -e[1] * k / m_dt, -e[2] * k / m_dt }; //K=1で１フレで誤差を修正する速度。

		//確定している答えを入れとく
		wtplus1[0] = ke[0];   wtplus1[1] = ke[1];   wtplus1[2] = ke[2];
		
	}
	else
	{
		//確定している答えを入れとく
		wtplus1[0] = 0;   wtplus1[1] = 0;   wtplus1[2] = 0;
	}
	dstLambda[3] = 0.0; dstLambda[4] = 0.0; dstLambda[5] = 0.0;
	
	//ボールジョイントは速度成分はすべて０。上33行列で、未知であるlambda1～3まで求められる
	double lambda3[3];
	{
		//上3x3行列に対し、
		//w[t+1] = Aλ + b から
		//w[t+1]-b = Aλ から
		//A^1{w[t+1]-b} = λ 
		//をこれから計算する。

		//w[t+1]とあるが、上記にあるように上3つ成分は０。
		//なので実質、A^1{-b} = λ 

		//3x3へ
		double invA33[3][3];
		DsMat33d A33;
		for (int row = 0; row < 3; ++row)
		{
			for (int col = 0; col < 3; ++col)
			{
				A33.mat[row * 3 + col] = A[row][col];
			}
		}

		//A^1を求める
		A33 = DsMat33d::Inverse(A33);
		for (int row = 0; row < 3; ++row)
		{
			for (int col = 0; col < 3; ++col)
			{
				invA33[row][col] = A33.mat[row * 3 + col];
			}
		}

		//{-b}を求める
		const double _b3[3] = { wtplus1[0] - b[0], wtplus1[1] - b[1], wtplus1[2] - b[2] };

		//λ = A^1{-b} を求める
		__MultiVec<3, 3>(lambda3, invA33, _b3);
	}

	//lambda1～3が求まったので、Lambdaに関しては解けた
	dstLambda[0] = lambda3[0]; dstLambda[1] = lambda3[1]; dstLambda[2] = lambda3[2];


	//lambdaが求まったので、w[t+1]の下３つである角速度成分を求めることができる
	//w[t+1] = Aλ + b なので、普通に求められる
#if 0	//でも別に使わないはずなので、解かなくていい。
	double wtplus13[3];
	{
		double A33[3][6];//下行のみでいい
		for (int row = 0; row < 3; ++row)
		{
			for (int col = 0; col < 6; ++col)
			{
				A33[row][col] = A[row + 3][col];
			}
		}
		__MultiVec<3, 6>(wtplus13, A, dstLambda);
	}

	//下３つの角速度も求まった
	wtplus1[3] = wtplus13[0]; wtplus1[4] = wtplus13[0]; wtplus1[5] = wtplus13[0];
#endif

	//速度を０にしようと力をかける→０にしようと逆方向に力かける
	//デバッグで値を見ると上下に発散している

	//即座に解決しようとするのではなく、徐々に。
	//const double gain = 0.4;
	//dstLambda[0] *= gain;
	//dstLambda[1] *= gain;
	//dstLambda[2] *= gain;

	//それでも発散するのでクランプ
	dstLambda[0] = Clamp(dstLambda[0], -10.0, 10.0);
	dstLambda[1] = Clamp(dstLambda[1], -10.0, 10.0);
	dstLambda[2] = Clamp(dstLambda[2], -10.0, 10.0);

	return true;
#endif

}
#endif





