#include "DsPhysicsPch.h"
#ifndef __DS_COLLISION_CONSTRAINT__
#include "Constraint/DsCollisionConstraint.h"
#endif
#include "DsMass.h"
#include "DsPhysicsWorld.h"

using namespace DsPhysics;


//衝突方向のみの計算


void DsCollisionConstraint::SetUp()
{
	DsActor* pMas = m_world->GetActor(m_masterId);
	DsActor* pSub = m_world->GetActor(m_subId);

	if (pMas && pSub){
		if (!m_isSetUp){
			const DsVec3d posA = pMas->GetPositionForConstraint();
			const DsVec3d posB = pSub->GetPositionForConstraint();
			const DsVec3d ra = m_colPos - posA;
			const DsVec3d rb = m_colPos - posB;
			const DsVec3d n = m_colNormal;
			const DsVec3d ra_n = DsVec3d::Cross(ra, n);
			const DsVec3d rb_n = DsVec3d::Cross(rb, n);

			m_bound = (pMas->RefMaterial().m_bundCoef + pSub->RefMaterial().m_bundCoef)*0.5;
			if (((0.0000001 > pMas->RefMaterial().m_bundCoef) && (pMas->RefMaterial().m_bundCoef < -0.0000001)) ||
				((0.0000001 > pSub->RefMaterial().m_bundCoef) && (pSub->RefMaterial().m_bundCoef < -0.0000001))){
				m_bound = 0;//どっちかが０なら跳ね返らない
			}

			m_sFric = (pMas->RefMaterial().m_staticFrictionCoef + pSub->RefMaterial().m_staticFrictionCoef)*0.5;
			if (((0.0000001 > pMas->RefMaterial().m_staticFrictionCoef) && (pMas->RefMaterial().m_staticFrictionCoef > -0.0000001)) ||
				((0.0000001 > pSub->RefMaterial().m_staticFrictionCoef) && (pSub->RefMaterial().m_staticFrictionCoef > -0.0000001))){
				m_sFric = 0;//どっちかが摩擦無視なら０
			}
			m_kFric = (pMas->RefMaterial().m_kinematicFricCoef + pSub->RefMaterial().m_kinematicFricCoef)*0.5;
			if (((0.0000001 > pMas->RefMaterial().m_kinematicFricCoef) && (pMas->RefMaterial().m_kinematicFricCoef > -0.0000001)) ||
				((0.0000001 > pSub->RefMaterial().m_kinematicFricCoef) && (pSub->RefMaterial().m_kinematicFricCoef > -0.0000001))){
				m_kFric = 0;//どっちかが摩擦無視なら０
			}

			const DsVec3d mV = pMas->GetVelocity();
			const DsVec3d mW = pMas->GetAngularVel();
			const DsVec3d sV = pSub->GetVelocity();
			const DsVec3d sW = pSub->GetAngularVel();

			DsVec3d nx;
			DsVec3d ny;
			m_colNormal.GetVerticalPlane(nx, ny);
			const DsVec3d ra_nx = DsVec3d::Cross(ra, nx);
			const DsVec3d rb_nx = DsVec3d::Cross(rb, nx);
			const DsVec3d ra_ny = DsVec3d::Cross(ra, ny);
			const DsVec3d rb_ny = DsVec3d::Cross(rb, ny);

			//拘束座標系から物体座標系に変換する行列。衝突専用なので衝突方向しか成分がない1*12
			m_J[0][0] = nx.x; m_J[0][1] = nx.y; m_J[0][2] = nx.z; m_J[0][3] = ra_nx.x; m_J[0][4] = ra_nx.y; m_J[0][5] = ra_nx.z; m_J[0][6] = -nx.x; m_J[0][7] = -nx.y; m_J[0][8] = -nx.z; m_J[0][9] = -rb_nx.x; m_J[0][10] = -rb_nx.y; m_J[0][11] = -rb_nx.z;
			m_J[1][0] = ny.x; m_J[1][1] = ny.y; m_J[1][2] = ny.z; m_J[1][3] = ra_ny.x; m_J[1][4] = ra_ny.y; m_J[1][5] = ra_ny.z; m_J[1][6] = -ny.x; m_J[1][7] = -ny.y; m_J[1][8] = -ny.z; m_J[1][9] = -rb_ny.x; m_J[1][10] = -rb_ny.y; m_J[1][11] = -rb_ny.z;
			m_J[2][0] = n.x;  m_J[2][1] = n.y;  m_J[2][2] = n.z;  m_J[2][3] = ra_n.x;  m_J[2][4] = ra_n.y;  m_J[2][5] = ra_n.z;  m_J[2][6] = -n.x;  m_J[2][7] = -n.y;  m_J[2][8] = -n.z;  m_J[2][9] = -rb_n.x;  m_J[2][10] = -rb_n.y;  m_J[2][11] = -rb_n.z;
			for (int i = 0; 12 > i; ++i){
				m_Jt[i][0] = m_J[0][i];
				m_Jt[i][1] = m_J[1][i];
				m_Jt[i][2] = m_J[2][i];
			}

			_SetVel(mV, mW, sV, sW);
			_SetMass(*pMas, *pSub);

			//double check[12][3];
			//DsMathUtil::MultiMatMN<12, 3, 12>(check, m_invM, m_Jt);//m_invMが対角成分しかないので省けるのでは？
#ifndef DS_SYS_USE_SIMD_
			double invMJt[12][3] = {};
			invMJt[0][0] = m_invM[0][0] * m_Jt[0][0];   invMJt[0][1] = m_invM[0][0] * m_Jt[0][1];   invMJt[0][2] = m_invM[0][0] * m_Jt[0][2];
			invMJt[1][0] = m_invM[1][1] * m_Jt[1][0];   invMJt[1][1] = m_invM[1][1] * m_Jt[1][1];   invMJt[1][2] = m_invM[1][1] * m_Jt[1][2];
			invMJt[2][0] = m_invM[2][2] * m_Jt[2][0];   invMJt[2][1] = m_invM[2][2] * m_Jt[2][1];   invMJt[2][2] = m_invM[2][2] * m_Jt[2][2];
			invMJt[3][0] = m_invM[3][3] * m_Jt[3][0] + m_invM[3][4] * m_Jt[4][0] + m_invM[3][5] * m_Jt[5][0];   invMJt[3][1] = m_invM[3][3] * m_Jt[3][1] + m_invM[3][4] * m_Jt[4][1] + m_invM[3][5] * m_Jt[5][1];   invMJt[3][2] = m_invM[3][3] * m_Jt[3][2] + m_invM[3][4] * m_Jt[4][2] + m_invM[3][5] * m_Jt[5][2];
			invMJt[4][0] = m_invM[4][3] * m_Jt[3][0] + m_invM[4][4] * m_Jt[4][0] + m_invM[4][5] * m_Jt[5][0];   invMJt[4][1] = m_invM[4][3] * m_Jt[3][1] + m_invM[4][4] * m_Jt[4][1] + m_invM[4][5] * m_Jt[5][1];   invMJt[4][2] = m_invM[4][3] * m_Jt[3][2] + m_invM[4][4] * m_Jt[4][2] + m_invM[4][5] * m_Jt[5][2];
			invMJt[5][0] = m_invM[5][3] * m_Jt[3][0] + m_invM[5][4] * m_Jt[4][0] + m_invM[5][5] * m_Jt[5][0];   invMJt[5][1] = m_invM[5][3] * m_Jt[3][1] + m_invM[5][4] * m_Jt[4][1] + m_invM[5][5] * m_Jt[5][1];   invMJt[5][2] = m_invM[5][3] * m_Jt[3][2] + m_invM[5][4] * m_Jt[4][2] + m_invM[5][5] * m_Jt[5][2];
			invMJt[6][0] = m_invM[6][6] * m_Jt[6][0];   invMJt[6][1] = m_invM[6][6] * m_Jt[6][1];   invMJt[6][2] = m_invM[6][6] * m_Jt[6][2];
			invMJt[7][0] = m_invM[7][7] * m_Jt[7][0];   invMJt[7][1] = m_invM[7][7] * m_Jt[7][1];   invMJt[7][2] = m_invM[7][7] * m_Jt[7][2];
			invMJt[8][0] = m_invM[8][8] * m_Jt[8][0];   invMJt[8][1] = m_invM[8][8] * m_Jt[8][1];   invMJt[8][2] = m_invM[8][8] * m_Jt[8][2];
			invMJt[ 9][0] = m_invM[ 9][9] * m_Jt[9][0] + m_invM[ 9][10] * m_Jt[10][0] + m_invM[ 9][11] * m_Jt[11][0];   invMJt[ 9][1] = m_invM[ 9][9] * m_Jt[9][1] + m_invM[ 9][10] * m_Jt[10][1] + m_invM[ 9][11] * m_Jt[11][1];   invMJt[ 9][2] = m_invM[ 9][9] * m_Jt[9][2] + m_invM[ 9][10] * m_Jt[10][2] + m_invM[ 9][11] * m_Jt[11][2];
			invMJt[10][0] = m_invM[10][9] * m_Jt[9][0] + m_invM[10][10] * m_Jt[10][0] + m_invM[10][11] * m_Jt[11][0];   invMJt[10][1] = m_invM[10][9] * m_Jt[9][1] + m_invM[10][10] * m_Jt[10][1] + m_invM[10][11] * m_Jt[11][1];   invMJt[10][2] = m_invM[10][9] * m_Jt[9][2] + m_invM[10][10] * m_Jt[10][2] + m_invM[10][11] * m_Jt[11][2];
			invMJt[11][0] = m_invM[11][9] * m_Jt[9][0] + m_invM[11][10] * m_Jt[10][0] + m_invM[11][11] * m_Jt[11][0];   invMJt[11][1] = m_invM[11][9] * m_Jt[9][1] + m_invM[11][10] * m_Jt[10][1] + m_invM[11][11] * m_Jt[11][1];   invMJt[11][2] = m_invM[11][9] * m_Jt[9][2] + m_invM[11][10] * m_Jt[10][2] + m_invM[11][11] * m_Jt[11][2];
#else
			//あんまり効果が見込めない
			double invMJt[12][4] = {};
			*(__m256d*)(invMJt[0]) = _mm256_mul_pd(_mm256_set1_pd(m_invM[0][0]), *(__m256d*)(m_Jt[0])); invMJt[0][3] = 0;
			*(__m256d*)(invMJt[1]) = _mm256_mul_pd(_mm256_set1_pd(m_invM[1][1]), *(__m256d*)(m_Jt[1])); invMJt[1][3] = 0;
			*(__m256d*)(invMJt[2]) = _mm256_mul_pd(_mm256_set1_pd(m_invM[2][2]), *(__m256d*)(m_Jt[2])); invMJt[2][3] = 0;
			const __m256d invM30 = _mm256_mul_pd(*(__m256d*)(&m_invM[3][3]), *(__m256d*)(&m_J[0][3]));
			const __m256d invM31 = _mm256_mul_pd(*(__m256d*)(&m_invM[3][3]), *(__m256d*)(&m_J[1][3]));
			const __m256d invM32 = _mm256_mul_pd(*(__m256d*)(&m_invM[3][3]), *(__m256d*)(&m_J[2][3]));
			const __m256d invM40 = _mm256_mul_pd(*(__m256d*)(&m_invM[4][3]), *(__m256d*)(&m_J[0][3]));
			const __m256d invM41 = _mm256_mul_pd(*(__m256d*)(&m_invM[4][3]), *(__m256d*)(&m_J[1][3]));
			const __m256d invM42 = _mm256_mul_pd(*(__m256d*)(&m_invM[4][3]), *(__m256d*)(&m_J[2][3]));
			const __m256d invM50 = _mm256_mul_pd(*(__m256d*)(&m_invM[5][3]), *(__m256d*)(&m_J[0][3]));
			const __m256d invM51 = _mm256_mul_pd(*(__m256d*)(&m_invM[5][3]), *(__m256d*)(&m_J[1][3]));
			const __m256d invM52 = _mm256_mul_pd(*(__m256d*)(&m_invM[5][3]), *(__m256d*)(&m_J[2][3]));
			invMJt[3][0] = invM30.m256d_f64[0] + invM30.m256d_f64[1] + invM30.m256d_f64[2];
			invMJt[3][1] = invM31.m256d_f64[0] + invM31.m256d_f64[1] + invM31.m256d_f64[2];
			invMJt[3][2] = invM32.m256d_f64[0] + invM32.m256d_f64[1] + invM32.m256d_f64[2];
			invMJt[4][0] = invM40.m256d_f64[0] + invM40.m256d_f64[1] + invM40.m256d_f64[2];
			invMJt[4][1] = invM41.m256d_f64[0] + invM41.m256d_f64[1] + invM41.m256d_f64[2];
			invMJt[4][2] = invM42.m256d_f64[0] + invM42.m256d_f64[1] + invM42.m256d_f64[2];
			invMJt[5][0] = invM50.m256d_f64[0] + invM50.m256d_f64[1] + invM50.m256d_f64[2];
			invMJt[5][1] = invM51.m256d_f64[0] + invM51.m256d_f64[1] + invM51.m256d_f64[2];
			invMJt[5][2] = invM52.m256d_f64[0] + invM52.m256d_f64[1] + invM52.m256d_f64[2];
			//invMJt[3][0] = m_invM[3][3] * m_Jt[3][0] + m_invM[3][4] * m_Jt[4][0] + m_invM[3][5] * m_Jt[5][0];   invMJt[3][1] = m_invM[3][3] * m_Jt[3][1] + m_invM[3][4] * m_Jt[4][1] + m_invM[3][5] * m_Jt[5][1];   invMJt[3][2] = m_invM[3][3] * m_Jt[3][2] + m_invM[3][4] * m_Jt[4][2] + m_invM[3][5] * m_Jt[5][2];
			//invMJt[4][0] = m_invM[4][3] * m_Jt[3][0] + m_invM[4][4] * m_Jt[4][0] + m_invM[4][5] * m_Jt[5][0];   invMJt[4][1] = m_invM[4][3] * m_Jt[3][1] + m_invM[4][4] * m_Jt[4][1] + m_invM[4][5] * m_Jt[5][1];   invMJt[4][2] = m_invM[4][3] * m_Jt[3][2] + m_invM[4][4] * m_Jt[4][2] + m_invM[4][5] * m_Jt[5][2];
			//invMJt[5][0] = m_invM[5][3] * m_Jt[3][0] + m_invM[5][4] * m_Jt[4][0] + m_invM[5][5] * m_Jt[5][0];   invMJt[5][1] = m_invM[5][3] * m_Jt[3][1] + m_invM[5][4] * m_Jt[4][1] + m_invM[5][5] * m_Jt[5][1];   invMJt[5][2] = m_invM[5][3] * m_Jt[3][2] + m_invM[5][4] * m_Jt[4][2] + m_invM[5][5] * m_Jt[5][2];
			*(__m256d*)(invMJt[6]) = _mm256_mul_pd(_mm256_set1_pd(m_invM[6][6]), *(__m256d*)(m_Jt[6])); invMJt[6][3] = 0;
			*(__m256d*)(invMJt[7]) = _mm256_mul_pd(_mm256_set1_pd(m_invM[7][7]), *(__m256d*)(m_Jt[7])); invMJt[7][3] = 0;
			*(__m256d*)(invMJt[8]) = _mm256_mul_pd(_mm256_set1_pd(m_invM[8][8]), *(__m256d*)(m_Jt[8])); invMJt[8][3] = 0;
			invMJt[ 9][0] = m_invM[ 9][9] * m_Jt[9][0] + m_invM[ 9][10] * m_Jt[10][0] + m_invM[ 9][11] * m_Jt[11][0];   invMJt[ 9][1] = m_invM[ 9][9] * m_Jt[9][1] + m_invM[ 9][10] * m_Jt[10][1] + m_invM[ 9][11] * m_Jt[11][1];   invMJt[ 9][2] = m_invM[ 9][9] * m_Jt[9][2] + m_invM[ 9][10] * m_Jt[10][2] + m_invM[ 9][11] * m_Jt[11][2];
			invMJt[10][0] = m_invM[10][9] * m_Jt[9][0] + m_invM[10][10] * m_Jt[10][0] + m_invM[10][11] * m_Jt[11][0];   invMJt[10][1] = m_invM[10][9] * m_Jt[9][1] + m_invM[10][10] * m_Jt[10][1] + m_invM[10][11] * m_Jt[11][1];   invMJt[10][2] = m_invM[10][9] * m_Jt[9][2] + m_invM[10][10] * m_Jt[10][2] + m_invM[10][11] * m_Jt[11][2];
			invMJt[11][0] = m_invM[11][9] * m_Jt[9][0] + m_invM[11][10] * m_Jt[10][0] + m_invM[11][11] * m_Jt[11][0];   invMJt[11][1] = m_invM[11][9] * m_Jt[9][1] + m_invM[11][10] * m_Jt[10][1] + m_invM[11][11] * m_Jt[11][1];   invMJt[11][2] = m_invM[11][9] * m_Jt[9][2] + m_invM[11][10] * m_Jt[10][2] + m_invM[11][11] * m_Jt[11][2];
			//配列数がオーバー
			//const __m256d invM90 = _mm256_mul_pd(*(__m256d*)(&m_invM[9][9]), *(__m256d*)(&m_J[0][9]));
			//const __m256d invM91 = _mm256_mul_pd(*(__m256d*)(&m_invM[9][9]), *(__m256d*)(&m_J[1][9]));
			//const __m256d invM92 = _mm256_mul_pd(*(__m256d*)(&m_invM[9][9]), *(__m256d*)(&m_J[2][9]));
			//const __m256d invM100= _mm256_mul_pd(*(__m256d*)(&m_invM[10][9]), *(__m256d*)(&m_J[0][9]));
			//const __m256d invM101= _mm256_mul_pd(*(__m256d*)(&m_invM[10][9]), *(__m256d*)(&m_J[1][9]));
			//const __m256d invM102= _mm256_mul_pd(*(__m256d*)(&m_invM[10][9]), *(__m256d*)(&m_J[2][9]));
			//const __m256d invM110= _mm256_mul_pd(*(__m256d*)(&m_invM[11][9]), *(__m256d*)(&m_J[0][9]));
			//const __m256d invM111= _mm256_mul_pd(*(__m256d*)(&m_invM[11][9]), *(__m256d*)(&m_J[1][9]));
			//const __m256d invM112= _mm256_mul_pd(*(__m256d*)(&m_invM[11][9]), *(__m256d*)(&m_J[2][9]));
			//invMJt[9][0]  = invM90.m256d_f64[0] + invM90.m256d_f64[1] + invM90.m256d_f64[2];
			//invMJt[9][1]  = invM91.m256d_f64[0] + invM91.m256d_f64[1] + invM91.m256d_f64[2];
			//invMJt[9][2]  = invM92.m256d_f64[0] + invM92.m256d_f64[1] + invM92.m256d_f64[2];
			//invMJt[10][0] = invM100.m256d_f64[0] + invM100.m256d_f64[1] + invM100.m256d_f64[2];
			//invMJt[10][1] = invM101.m256d_f64[0] + invM101.m256d_f64[1] + invM101.m256d_f64[2];
			//invMJt[10][2] = invM102.m256d_f64[0] + invM102.m256d_f64[1] + invM102.m256d_f64[2];
			//invMJt[11][0] = invM110.m256d_f64[0] + invM110.m256d_f64[1] + invM110.m256d_f64[2];
			//invMJt[11][1] = invM111.m256d_f64[0] + invM111.m256d_f64[1] + invM111.m256d_f64[2];
			//invMJt[11][2] = invM112.m256d_f64[0] + invM112.m256d_f64[1] + invM112.m256d_f64[2];
#endif

#ifndef DS_SYS_USE_SIMD_
			DsMathUtil::MultiMatMN<3, 3, 12>(m_cEq.A, m_J, invMJt);
			for (int i = 0; i < 3; ++i){
				for (int j = 0; j < 3; ++j){
					m_cEq.A[i][j] *= m_dt;
				}
			}
#else
			for (int i = 0; 3>i; ++i){
				for (int j = 0; 3>j; ++j){
					//m_cEq.A[i][j] = 0.0;
					//for (int k = 0; 12>k; ++k){
					//	m_cEq.A[i][j] += m_J[i][k] * invMJt[k][j];
					//}
					const double tmpR[12] = { invMJt[0][j], invMJt[1][j], invMJt[2][j], invMJt[3][j], invMJt[4][j], invMJt[5][j], invMJt[6][j], invMJt[7][j], invMJt[8][j], invMJt[9][j], invMJt[10][j], invMJt[11][j] };
					const __m256d* a1 = (__m256d*)(m_J[i]);
					const __m256d* b1 = (__m256d*)(tmpR);

					const __m256d res1 = _mm256_mul_pd(a1[0], b1[0]);
					const __m256d res2 = _mm256_mul_pd(a1[1], b1[1]);
					const __m256d res3 = _mm256_mul_pd(a1[2], b1[2]);

					const __m256d res4 = _mm256_add_pd(_mm256_add_pd(res1, res2), res3);
					m_cEq.A[i][j] = res4.m256d_f64[0] + res4.m256d_f64[1] + res4.m256d_f64[2] + res4.m256d_f64[3];
				}
			}
			__m256d dt256 = _mm256_set1_pd(m_dt);
			for (int i = 0; i < 3; ++i){
				const __m256d res = _mm256_mul_pd(*(__m256d*)(m_cEq.A[i]), dt256);
				m_cEq.A[i][0] = res.m256d_f64[0];
				m_cEq.A[i][1] = res.m256d_f64[1];
				m_cEq.A[i][2] = res.m256d_f64[2];
			}
#endif
			

#ifndef DS_SYS_USE_SIMD_
			DsMathUtil::MultiVec<3, 12>(m_w, m_J, m_u);
#else
			for (int i = 0; 3>i; ++i)
			{//simd版
				const __m256d* a1 = (__m256d*)(m_J[i]);
				const __m256d* b1 = (__m256d*)(m_u);

				const __m256d res1 = _mm256_mul_pd(a1[0], b1[0]);
				const __m256d res2 = _mm256_mul_pd(a1[1], b1[1]);
				const __m256d res3 = _mm256_mul_pd(a1[2], b1[2]);

				const __m256d res4 = _mm256_add_pd(_mm256_add_pd(res1, res2), res3);
				m_w[i] = res4.m256d_f64[0] + res4.m256d_f64[1] + res4.m256d_f64[2] + res4.m256d_f64[3];
			}
#endif
		}

		const DsVec3d& mF = (pMas->RefOption().isStatic) ? (DsVec3d::Zero()) : (pMas->GetForce());
		const DsVec3d& mT = (pMas->RefOption().isStatic) ? (DsVec3d::Zero()) : (pMas->GetTorque());
		const DsVec3d& sF = (pSub->RefOption().isStatic) ? (DsVec3d::Zero()) : (pSub->GetForce());
		const DsVec3d& sT = (pSub->RefOption().isStatic) ? (DsVec3d::Zero()) : (pSub->GetTorque());
		_SetExForce(mF, mT, sF, sT);

		m_isSetUp = true;
	}
}

//virtual
void DsCollisionConstraint::UpdateExForce()
{
	DsActor* pMas = m_world->GetActor(m_masterId);
	DsActor* pSub = m_world->GetActor(m_subId);
	if (pMas && pSub) {
		const DsVec3d& mF = (pMas->RefOption().isStatic) ? (DsVec3d::Zero()) : (pMas->GetForce());
		const DsVec3d& mT = (pMas->RefOption().isStatic) ? (DsVec3d::Zero()) : (pMas->GetTorque());
		const DsVec3d& sF = (pSub->RefOption().isStatic) ? (DsVec3d::Zero()) : (pSub->GetForce());
		const DsVec3d& sT = (pSub->RefOption().isStatic) ? (DsVec3d::Zero()) : (pSub->GetTorque());
		_SetExForce(mF, mT, sF, sT);
	}
}

void DsCollisionConstraint::_SetVel(const DsVec3d& mV, const DsVec3d& mW, const DsVec3d& sV, const DsVec3d& sW)
{
	m_u[0] = mV.x;
	m_u[1] = mV.y;
	m_u[2] = mV.z;
	m_u[3] = mW.x;
	m_u[4] = mW.y;
	m_u[5] = mW.z;
	m_u[6] = sV.x;
	m_u[7] = sV.y;
	m_u[8] = sV.z;
	m_u[9]  = sW.x;
	m_u[10] = sW.y;
	m_u[11] = sW.z;
}

void DsCollisionConstraint::_SetMass(const DsActor& master, const DsActor& sub)
{
	for (int i = 0; 12>i; ++i){
		for (int j = 0; 12>j; ++j){
			m_M[i][j] = 0.0;
			m_invM[i][j] = 0.0;
		}
	}

	//２個の物体の重さを12*12の対角行列に格納
	const DsMass& m1 = master.GetMass();
	const DsMass& m2 = sub.GetMass();
	m_M[0][0] = m1.mass; m_M[1][1] = m1.mass; m_M[2][2] = m1.mass;
	m_M[3][3] = m1.inertia[0]; m_M[3][4] = m1.inertia[1]; m_M[3][5] = m1.inertia[2];
	m_M[4][3] = m1.inertia[3]; m_M[4][4] = m1.inertia[4]; m_M[4][5] = m1.inertia[5];
	m_M[5][3] = m1.inertia[6]; m_M[5][4] = m1.inertia[7]; m_M[5][5] = m1.inertia[8];
	m_M[6][6] = m2.mass; m_M[7][7] = m2.mass; m_M[8][8] = m2.mass;
	m_M[9][9] = m2.inertia[0]; m_M[9][10] = m2.inertia[1]; m_M[9][11] = m2.inertia[2];
	m_M[10][9] = m2.inertia[3]; m_M[10][10] = m2.inertia[4]; m_M[10][11] = m2.inertia[5];
	m_M[11][9] = m2.inertia[6]; m_M[11][10] = m2.inertia[7]; m_M[11][11] = m2.inertia[8];
	
	//DsInverseMatrix<12>(m_M, m_invM);//対角成分しかないので、個別に求めた逆行列を繋げるでOK
	const double* im1 = master.GetMassInv10();
	const double* im2 = sub.GetMassInv10();
	m_invM[0][0]  = im1[0];  m_invM[1][1]   = im1[0];    m_invM[2][2]   = im1[0];
	m_invM[3][3]  = im1[1];  m_invM[3][4]   = im1[2];    m_invM[3][5]   = im1[3];
	m_invM[4][3]  = im1[4];  m_invM[4][4]   = im1[5];    m_invM[4][5]   = im1[6];
	m_invM[5][3]  = im1[7];  m_invM[5][4]   = im1[8];    m_invM[5][5]   = im1[9];
	m_invM[6][6]  = im2[0];  m_invM[7][7]   = im2[0];    m_invM[8][8]   = im2[0];
	m_invM[9][9]  = im2[1];  m_invM[9][10]  = im2[2];    m_invM[9][11]  = im2[3];
	m_invM[10][9] = im2[4];  m_invM[10][10] = im2[5];    m_invM[10][11] = im2[6];
	m_invM[11][9] = im2[7];  m_invM[11][10] = im2[8];    m_invM[11][11] = im2[9];
}

void DsCollisionConstraint::_SetExForce(const DsVec3d& mF, const DsVec3d& mT, const DsVec3d& sF, const DsVec3d& sT)
{
	//外力
	m_Fe[0] = mF.x; m_Fe[1] = mF.y; m_Fe[2] = mF.z; m_Fe[3] = mT.x; m_Fe[4] = mT.y; m_Fe[5] = mT.z;
	m_Fe[6] = sF.x; m_Fe[7] = sF.y; m_Fe[8] = sF.z; m_Fe[9] = sT.x; m_Fe[10] = sT.y; m_Fe[11] = sT.z;
}

void DsCollisionConstraint::_CalcConatraintForce()
{
	//ここで、拘束座標系での拘束条件(各軸の速度が0だとか)を物体座標系の拘束力に変換する
	double invMFe[12];
	//DsMathUtil::MultiVec<12, 12>(invMFe, m_invM, m_Fe);//m_invMが対角成分しかないので省けるのでは？
	invMFe[0] = m_invM[0][0] * m_Fe[0];
	invMFe[1] = m_invM[1][1] * m_Fe[1];
	invMFe[2] = m_invM[2][2] * m_Fe[2];
	invMFe[3] = m_invM[3][3] * m_Fe[3] + m_invM[3][4] * m_Fe[4] + m_invM[3][5] * m_Fe[5];
	invMFe[4] = m_invM[4][3] * m_Fe[3] + m_invM[4][4] * m_Fe[4] + m_invM[4][5] * m_Fe[5];
	invMFe[5] = m_invM[5][3] * m_Fe[3] + m_invM[5][4] * m_Fe[4] + m_invM[5][5] * m_Fe[5];
	invMFe[6] = m_invM[6][6] * m_Fe[6];
	invMFe[7] = m_invM[7][7] * m_Fe[7];
	invMFe[8] = m_invM[8][8] * m_Fe[8];
	invMFe[9]  = m_invM[9][9] * m_Fe[9] + m_invM[9][10] * m_Fe[10] + m_invM[9][11] * m_Fe[11];
	invMFe[10] = m_invM[10][9] * m_Fe[9] + m_invM[10][10] * m_Fe[10] + m_invM[10][11] * m_Fe[11];
	invMFe[11] = m_invM[11][9] * m_Fe[9] + m_invM[11][10] * m_Fe[10] + m_invM[11][11] * m_Fe[11];

	double JinvMFe[3];
#ifndef DS_SYS_USE_SIMD_
	DsMathUtil::MultiVec<3, 12>(JinvMFe, m_J, invMFe);
#else
	for (int i = 0; 3>i; ++i)
	{
		//JinvMFe[i] = 0.0;
		//for (int j = 0; 12>j; ++j)
		//{
		//	JinvMFe[i] += m_J[i][j] * invMFe[j];
		//}
		{//simd版
			const __m256d* a1 = (__m256d*)(m_J[i]);
			const __m256d* b1 = (__m256d*)(invMFe);
			
			const __m256d res1 = _mm256_mul_pd(a1[0], b1[0]);
			const __m256d res2 = _mm256_mul_pd(a1[1], b1[1]);
			const __m256d res3 = _mm256_mul_pd(a1[2], b1[2]);

			const __m256d res4 = _mm256_add_pd(_mm256_add_pd(res1, res2), res3);
			JinvMFe[i] = res4.m256d_f64[0] + res4.m256d_f64[1] + res4.m256d_f64[2] + res4.m256d_f64[3];
		}
	}
#endif

	//Jが衝突方向にしか対応してないので、w[t+1]は衝突方向の速度（スカラー）になる

	//A = JinvMJt * dt
	//b = w[t] + JinvMFe * dt
	//w[t+1] = Ax+b		xが求めたい拘束力

	//const double A[3][3] = {
	//	{ m_JinvMJt[0][0], m_JinvMJt[0][1], m_JinvMJt[0][2]},
	//	{ m_JinvMJt[1][0], m_JinvMJt[1][1], m_JinvMJt[1][2]},
	//	{ m_JinvMJt[2][0], m_JinvMJt[2][1], m_JinvMJt[2][2]},
	//};

	const double b[3] = { 
		m_w[0] + JinvMFe[0] * m_dt,
		m_w[1] + JinvMFe[1] * m_dt,
		m_w[2] + JinvMFe[2] * m_dt,
	};

	//これが最終的に求めたい式
	//x = (w[t+1]-b)/A
	//x=lambda として↓記述

	//w[t+1] は次の目標速度なので、
	//w[t+1] = w(今の速度)*e(跳ね返り係数) として解く

	double staticC = 1.0;
	DsActor* pMas = m_world->GetActor(m_masterId);
	DsActor* pSub = m_world->GetActor(m_subId);
	if (pMas && pSub){
		if (pMas->RefOption().isStatic || pSub->RefOption().isStatic
			/*|| pMas->IsRestContagion() || pSub->IsRestContagion()*/){
			staticC = 2.0;//一方が静的な場合、動かないので、動くほうを２倍
			pMas->SetRestContagion(true);
			pSub->SetRestContagion(true);
		}
	}

	
	//跳ね返る速度を求める
	const double vel = fabs(m_w[2]);
	//めり込み解消速度。バネダンパ
	double requestVel = max( ((20.0 * m_error) - (0.05*m_w[2])), 0.0);
	
	//静的じゃない方を2倍
	//重さも考慮して力がかかるので、もし静的がとても重く設定されているなら、ほとんどがrequestVelは動的の方の速度変化分が占める。
	//それを2倍するとさらに加速してしまうので飛んでしまう
	//同じ重さなら2倍でちょうどいいのだから、重さで比率かける方がいい？でも回転が分からないのでなんとも。
	//軽い静的物があるとめり込んでしまうが、めり込み解消でなんとかする。か静的になった瞬間重くするか
	//requestVel *= staticC;

	//跳ね返り速度を求める
	//const double km_s = 10.0;
	//const double dv = (km_s*1000.0*m_dt)/(60.0*60.0);
	if( 0.04 < vel){//振動防止のため止まろうとしているときはめり込み分しかかけない
		const double boundVel = (vel*(m_bound));
		requestVel = max(boundVel, requestVel);//めり込み解消より小さくならないように
	}

	{//摩擦
		//zが衝突方向で、垂直抗力方向なので注意

		m_cEq.min_wplus1[0] = 0; m_cEq.min_wplus1[1] = 0;
		m_cEq.max_wplus1[0] = 0; m_cEq.max_wplus1[1] = 0;

		double JFe[3];
		DsMathUtil::MultiVec<3, 12>(JFe, m_J, invMFe);
		const bool isDown = (JFe[2] < 0.0);//沈む方向に力が加わってる
		if (isDown){
			const double Fxy = (JFe[0] * JFe[0] + JFe[1] * JFe[1]);
			const double Vxy = (b[0] * b[0] + b[1] * b[1]);
			const bool isStop = ( Vxy < (0.01));
			const double nFs = fabs(JFe[2] * m_sFric);
			if (isStop || (Fxy < nFs*nFs) ){//静止摩擦
				m_cEq.isStaticByFric[0] = true;//垂直抗力*摩擦係数より低い力しかかかってないので静止させる
				m_cEq.isStaticByFric[1] = true;
				//速度0
				m_cEq.min_wplus1[0] = 0; m_cEq.min_wplus1[1] = 0;
				m_cEq.max_wplus1[0] = 0; m_cEq.max_wplus1[1] = 0;
			}
			else if ( ( DBL_EPSILON < Vxy)){//動摩擦
				//吹っ飛ぶので無効化
				//m_cEq.isStaticByFric[0] = false;
				//m_cEq.isStaticByFric[1] = false;
				//
				//const double VxySp = sqrt(Vxy);
				//const double nFk = fabs(JFe[2] * m_kFric);
				//DsVec2d fricF = -DsVec2d(b[0] / VxySp, b[1] / VxySp);//速度と反対方向にかける
				//
				////現在の速度を超えないように
				//double m = fabs(JFe[2] / JinvMFe[2]);
				//double clampF = min(nFk, (VxySp/m_dt)*m);
				////平面の力を超えないように
				//clampF = min(clampF, sqrt(Fxy));
				////最大値//あばれるので無効化
				////clampF = min(clampF, 10);//多分重量差があると飛ぶ
				//fricF = fricF*clampF;
				//
				//m_cEq.fricLambda[0] = fricF.x;
				//m_cEq.fricLambda[1] = fricF.y;

				m_cEq.isStaticByFric[0] = false;
				m_cEq.isStaticByFric[1] = false;
				m_cEq.fricLambda[0] = 0;
				m_cEq.fricLambda[1] = 0;
			}
		}
		else{
			//浮く方向に力が加わっているので摩擦無し
			m_cEq.isStaticByFric[0] = false;
			m_cEq.isStaticByFric[1] = false;
			m_cEq.fricLambda[0] = 0;
			m_cEq.fricLambda[1] = 0;
		}
	}

	//memcpy(m_cEq.A, m_JinvMJt, sizeof(double) * 9);//セットアップ以降変わらないので。
	memcpy(m_cEq.b, b, sizeof(double) * 3);
	m_cEq.min_wplus1[2] = 0;
	m_cEq.max_wplus1[2] = requestVel;
	m_cEq.lambda[0] = 0; m_cEq.lambda[1] = 0; m_cEq.lambda[2] = 0;

}

//virtual 
void DsCollisionConstraint::CalclateConstraintForce()
{
	_CalcConatraintForce();
}

//virtual 
void DsCollisionConstraint::ApplyConstraintForce()
{
	DsActor* pMas = m_world->GetActor(m_masterId);
	DsActor* pSub = m_world->GetActor(m_subId);

	if (pMas && pSub)
	{
		double Fc[12];
		//DsMathUtil::MultiVec<12, 3>(Fc, m_Jt, RefEquation().lambda);
		const double lambda[3] = {
			RefEquation().lambda[0],
			RefEquation().lambda[1],
			RefEquation().lambda[2],
		};

#ifndef DS_SYS_USE_SIMD_
		Fc[0] = m_Jt[0][0] * lambda[0] + m_Jt[0][1] * lambda[1] + m_Jt[0][2] * lambda[2];
		Fc[1] = m_Jt[1][0] * lambda[0] + m_Jt[1][1] * lambda[1] + m_Jt[1][2] * lambda[2];
		Fc[2] = m_Jt[2][0] * lambda[0] + m_Jt[2][1] * lambda[1] + m_Jt[2][2] * lambda[2];
		Fc[3] = m_Jt[3][0] * lambda[0] + m_Jt[3][1] * lambda[1] + m_Jt[3][2] * lambda[2];
		Fc[4] = m_Jt[4][0] * lambda[0] + m_Jt[4][1] * lambda[1] + m_Jt[4][2] * lambda[2];
		Fc[5] = m_Jt[5][0] * lambda[0] + m_Jt[5][1] * lambda[1] + m_Jt[5][2] * lambda[2];
		Fc[6] = m_Jt[6][0] * lambda[0] + m_Jt[6][1] * lambda[1] + m_Jt[6][2] * lambda[2];
		Fc[7] = m_Jt[7][0] * lambda[0] + m_Jt[7][1] * lambda[1] + m_Jt[7][2] * lambda[2];
		Fc[8] = m_Jt[8][0] * lambda[0] + m_Jt[8][1] * lambda[1] + m_Jt[8][2] * lambda[2];
		Fc[9] = m_Jt[9][0] * lambda[0] + m_Jt[9][1] * lambda[1] + m_Jt[9][2] * lambda[2];
		Fc[10] = m_Jt[10][0] * lambda[0] + m_Jt[10][1] * lambda[1] + m_Jt[10][2] * lambda[2];
		Fc[11] = m_Jt[11][0] * lambda[0] + m_Jt[11][1] * lambda[1] + m_Jt[11][2] * lambda[2];
#else
		const __m256d b1 = { lambda[0], lambda[0], lambda[0], lambda[0] };
		const __m256d b2 = { lambda[1], lambda[1], lambda[1], lambda[1] };
		const __m256d b3 = { lambda[2], lambda[2], lambda[2], lambda[2] };
		
		const __m256d* a1 = (__m256d*)m_J[0];
		const __m256d res11 = _mm256_mul_pd(a1[0], b1);
		const __m256d res21 = _mm256_mul_pd(a1[1], b1);
		const __m256d res31 = _mm256_mul_pd(a1[2], b1);

		const __m256d* a2 = (__m256d*)m_J[1];
		const __m256d res12 = _mm256_mul_pd(a2[0], b2);
		const __m256d res22 = _mm256_mul_pd(a2[1], b2);
		const __m256d res32 = _mm256_mul_pd(a2[2], b2);

		const __m256d* a3 = (__m256d*)m_J[2];
		const __m256d res13 = _mm256_mul_pd(a3[0], b3);
		const __m256d res23 = _mm256_mul_pd(a3[1], b3);
		const __m256d res33 = _mm256_mul_pd(a3[2], b3);

		const __m256d ret1 = _mm256_add_pd(_mm256_add_pd(res11, res12), res13);
		const __m256d ret2 = _mm256_add_pd(_mm256_add_pd(res21, res22), res23);
		const __m256d ret3 = _mm256_add_pd(_mm256_add_pd(res31, res32), res33);

		Fc[0] = ret1.m256d_f64[0];
		Fc[1] = ret1.m256d_f64[1];
		Fc[2] = ret1.m256d_f64[2];
		Fc[3] = ret1.m256d_f64[3];
		Fc[4] = ret2.m256d_f64[0];
		Fc[5] = ret2.m256d_f64[1];
		Fc[6] = ret2.m256d_f64[2];
		Fc[7] = ret2.m256d_f64[3];
		Fc[8] = ret3.m256d_f64[0];
		Fc[9] = ret3.m256d_f64[1];
		Fc[10] = ret3.m256d_f64[2];
		Fc[11] = ret3.m256d_f64[3];
#endif

		DsVec3d masterF;
		DsVec3d masterT;
		DsVec3d subF;
		DsVec3d subT;

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

		pMas->AddForce(masterF);
		pMas->AddTorque(masterT);
		pSub->AddForce(subF);
		pSub->AddTorque(subT);
	}
}

void DsCollisionConstraintSolver::Solve(DsCollisionConstraint& c, const int iterationNum)
{
	DsCollisionConstraintEquation& eq = c.RefEquation();
	const double maxLambda[3] = {  FLT_MAX,  FLT_MAX, FLT_MAX };
	const double minLambda[3] = { -FLT_MAX, -FLT_MAX, -FLT_MAX };
	const bool isUseInverseMat = false;//衝突はがんばって跳ね返り係数分の速度出そうとすると、干渉は考慮されてないので発散しやすい。繰り返しで諦めるようにした方がいいかも。

	if (isUseInverseMat){
		// w[t+1] = Aλ+b

		// w[t+1]-b = Aλ
		const DsVec3d w_b = DsVec3d::ToVec3(eq.max_wplus1) - DsVec3d::ToVec3(eq.b);
		// A~1(w[t+1]-b) = λ
		const DsMat33d invMat= DsMat33d::Inverse( DsMat33d::ToMat33(eq.A) );
		const DsVec3d lambda = invMat*w_b;
		
		eq.lambda[0] = (eq.isStaticByFric[0]) ? Clamp(lambda.x, minLambda[0], maxLambda[0]) : (eq.fricLambda[0]);
		eq.lambda[1] = (eq.isStaticByFric[1]) ? Clamp(lambda.y, minLambda[1], maxLambda[1]) : (eq.fricLambda[1]);
		eq.lambda[2] = Clamp(lambda.z, minLambda[2], maxLambda[2]);
		 
	}
	else{
		double preLambda[3] = {0,0,0};

		for (int iteIdx = 0; iteIdx < iterationNum; ++iteIdx){

			//１行あたり3*3あるのでそれを解いていく。Aの中の行ループ
			for (int row = 0; row < 3; ++row){
				//計算する拘束の軸を判定する。2(z)は衝突方向なので絶対。1,2は静止摩擦を上回った外力が加わったかで計算の有無決める
				if ((row == 2) || (eq.isStaticByFric[row]))
				{
					//衝突力
					if (DBL_EPSILON < fabs(eq.A[row][row])){
						double forward = 0;//更新済みK+1
						for (int col = 0; col < row; ++col){//一つ一つのA成分の列のループ。forwardは、既に解いてあるrow成分まで加算
							forward += eq.A[row][col] * eq.lambda[col];
						}

						double back = 0;//未更新分k
						for (int col = row + 1; col < 3; ++col){ //forward+自分をスキップしてそれ以降の列を加算
							back += eq.A[row][col] * eq.lambda[col];
						}

						//dst[col] = (srcVec[col] - forward - back) / srcMat[col][col];
						const double w_b = eq.max_wplus1[row] - eq.b[row];
						const double weight = 1.1;
						const double old_lamdba = eq.lambda[row];
						const double new_lambda = (w_b - forward - back) / eq.A[row][row];
						eq.lambda[row] = old_lamdba + weight*(new_lambda - old_lamdba);//SOR法。ガウスザイデルの解の変化量をweight倍加速

						eq.lambda[row] = Clamp(eq.lambda[row], minLambda[row], maxLambda[row]);
					}
				}
				else if (row < 2){
					eq.lambda[row] = eq.fricLambda[row];
				}
			}

			//収束したのでやめる
			//if ( (fabs(preLambda[0] - eq.lambda[0]) < 0.0001) &&
			//	(fabs(preLambda[1] - eq.lambda[1]) < 0.0001) &&
			//	(fabs(preLambda[2] - eq.lambda[2]) < 0.0001)
			//	){
			//	break;
			//}
			//preLambda[0] = eq.lambda[0];
			//preLambda[1] = eq.lambda[1];
			//preLambda[2] = eq.lambda[2];
		}
	}
}
