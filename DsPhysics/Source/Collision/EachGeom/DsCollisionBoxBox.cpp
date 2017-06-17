#include "DsPhysicsPch.h"
#ifndef __DS_COLLISION_BOX_BOX__
#include "Collision/EachGeom/DsCollisionBoxBox.h"
#endif

#include "Actor/DsRigidBox.h"
#ifndef __DS_COLLISION_DETECTION__
#include "Collision/DsCollisionDetection.h"
#endif
#ifndef __DS_COLLISION_CALLBACK__
#include "Collision/DsCollisionCallback.h"
#endif
#ifndef __DS_PHYSICS_WORLD__
#include "DsPhysicsWorld.h"
#endif

using namespace DsPhysics;

namespace
{
	enum class _N_AXIS : char
	{
		NONE,
		AX,
		AY,
		AZ,
		B_AXIS,
		BX,
		BY,
		BZ,

		CROSS,
		AX_BX,
		AX_BY,
		AX_BZ,
		AY_BX,
		AY_BY,
		AY_BZ,
		AZ_BX,
		AZ_BY,
		AZ_BZ,
	};


	inline double _Dot(const DsVec3d& a, const DsVec3d& b)
	{
		return DsVec3d::Dot(a, b);
	}

	//�Փˏ�������O�ꂽ��true
	inline bool _MaxDepth3Axis(_N_AXIS type, double expr1, double expr2, double& depth, double& sign, _N_AXIS& axis){
		double s2 = fabs(expr1) - expr2;
		if (depth < s2){
			depth = s2;
			axis = type;
			sign = (0.0 < expr1) ? (1.0) : (-1.0);
		}
		return (0.0 < s2);//true�œ������ĂȂ��̊m��Ȃ̂Ŕ�����I����
	}

	//�Փˏ�������O�ꂽ��treu
	inline bool _MaxDeoth9Axis(_N_AXIS type, double expr1, double expr2, const DsVec3d& cross, double& depth, DsVec3d& normal, _N_AXIS& axis){
		double s2 = fabs(expr1) - expr2;
		double len = cross.Length();
		if (0.0 < len){
			s2 /= len;
			if (depth < (s2*1.05)){//�قڕ��s�ɏՓ˂����Ƃ��́A�R���̕��Ɉ����������ė~�����̂ŁA�኱���茵��������
				depth = s2;
				double sign = (0.0 < expr1) ? (1.0) : (-1.0);
				normal = cross / (len*sign);
				axis = type;
			}
		}
		return (0.0 < s2);//true�œ������ĂȂ��̊m��Ȃ̂Ŕ�����I����
	}

	inline DsVec3d _GetNormal(_N_AXIS type, const DsVec3d Ra[3], const DsVec3d Rb[3])
	{
		switch (type){
		case _N_AXIS::AX: return Ra[0];
		case _N_AXIS::AY: return Ra[1];
		case _N_AXIS::AZ: return Ra[2];
		case _N_AXIS::BX: return Rb[0];
		case _N_AXIS::BY: return Rb[1];
		case _N_AXIS::BZ: return Rb[2];
		default: DS_ASSERT(false, "�����~�X"); return DsVec3d(0,0,0);
		}
	}

	inline void _GetAxis(_N_AXIS type, const DsVec3d Ra[3], const DsVec3d Rb[3], DsVec3d& dstA, DsVec3d& dstB)
	{
		switch (type){
		case _N_AXIS::AX_BX: dstA = Ra[0]; dstB = Rb[0]; return;
		case _N_AXIS::AX_BY: dstA = Ra[0]; dstB = Rb[1]; return;
		case _N_AXIS::AX_BZ: dstA = Ra[0]; dstB = Rb[2]; return;
		case _N_AXIS::AY_BX: dstA = Ra[1]; dstB = Rb[0]; return;
		case _N_AXIS::AY_BY: dstA = Ra[1]; dstB = Rb[1]; return;
		case _N_AXIS::AY_BZ: dstA = Ra[1]; dstB = Rb[2]; return;
		case _N_AXIS::AZ_BX: dstA = Ra[2]; dstB = Rb[0]; return;
		case _N_AXIS::AZ_BY: dstA = Ra[2]; dstB = Rb[1]; return;
		case _N_AXIS::AZ_BZ: dstA = Ra[2]; dstB = Rb[2]; return;
		default: DS_ASSERT(false, "�����~�X");  return;
		}
	}

	/*
		ODE�ŏd�Ȃ��Ă�̈攻��
		h �ʂP�̎l�p�`�̐��@�i�ړ�����]�����ĂȂ����ƑO��j
		p �ʂQ�̎l�p�`�̊e���_
		ret �������W �ő��8�_�Ȃ̂ŁA16�v�f
		@return �����_��
		@par	����Ă邱�Ƃ����܂����s��
	*/
	int _intersectRectQuad(const double h[2], const double p[8], double ret[16])
	{
		// q (and r) contain nq (and nr) coordinate points for the current (and
		// chopped) polygons
		int nq = 4, nr;
		double buffer[16];
		const double *q = p;
		double *r = ret;
		for (int dir = 0; dir <= 1; dir++) {
			// direction notation: xy[0] = x axis, xy[1] = y axis
			for (int sign = -1; sign <= 1; sign += 2) {
				// chop q along the line xy[dir] = sign*h[dir]
				const double *pq = q;
				double *pr = r;
				nr = 0;
				for (int i = nq; i > 0; i--) {
					// go through all points in q and all lines between adjacent points
					if (sign*pq[dir] < h[dir]) {
						// this point is inside the chopping line
						pr[0] = pq[0];
						pr[1] = pq[1];
						pr += 2;
						nr++;
						if (nr & 8) {
							q = r;
							goto done;
						}
					}
					const double *nextq = (i > 1) ? pq + 2 : q;
					if ((sign*pq[dir] < h[dir]) ^ (sign*nextq[dir] < h[dir])) {
						// this line crosses the chopping line
						pr[1 - dir] = pq[1 - dir] + (nextq[1 - dir] - pq[1 - dir]) /
							(nextq[dir] - pq[dir]) * (sign*h[dir] - pq[dir]);
						pr[dir] = sign*h[dir];
						pr += 2;
						nr++;
						if (nr & 8) {
							q = r;
							goto done;
						}
					}
					pq += 2;
				}
				q = r;
				r = (q == ret) ? buffer : ret;
				nq = nr;
			}
		}
	done:
		if (q != ret) memcpy(ret, q, nr * 2 * sizeof(double));
		return nr;
	}
}



DsCollisionResult& DsCollisionBoxBox::Collide()
{
	//return m_info;
	DS_ASSERT(m_pBox1, "�Փ˃I�[�i�[1��NULL");
	DS_ASSERT(m_pBox2, "�Փ˃I�[�i�[2��NULL");
	DS_ASSERT(m_pBox1 != m_pBox2, "�������̓��m�œ����蔻��");
	
	m_info.Clear();
	const bool isContain = DsAabb::IsContain(*m_pBox1->GetAabb(), *m_pBox2->GetAabb());
	if (isContain){
		if (m_world.GetCollisionCallback()) {
			if (!m_world.GetCollisionCallback()->IsCollide(*m_pBox1->RefOwnerId().GetActor(), *m_pBox2->RefOwnerId().GetActor())) {
				return m_info;
			}
		}
		return _ColideFinal();
	}

	return m_info;
}

DsCollisionResult& DsCollisionBoxBox::_ColideFinal()
{
	/*
	//ODE(���Ȃ݂�bullet������)���Q�l�B
	�������𔻒肵�A��Ԑ󂢕����֕Ԃ�

	�P�D�˂���̕����̕������������ꍇ
	�ӓ��m�̍Őڋߓ_�����߂�B���ύ��W���Փ˓_(bullet�͂ǂ������̍Őڋߓ_)
	�߂荞�ݗʂ͕������̏d�Ȃ��
	�����͕ӂ̊O�ς̕���

	�Q�Dxyz�̂R�����������������ꍇ
	�ʂƖʂ��d�Ȃ�̈���Z�o���A�߂荞��ł�̈�̒��_���Փ˓_�ɂ���
	�߂荞�ݗʂ͏Փ˓_�̂߂荞�ݗ�
	�����͕�����
	*/
	const DsVec3d sideA = m_pBox1->GetSide();
	const DsVec3d sideB = m_pBox2->GetSide();
	const DsVec3d Ra[3] = {
		m_pBox1->GetRot().GetAxisX(),
		m_pBox1->GetRot().GetAxisY(),
		m_pBox1->GetRot().GetAxisZ(),
	};
	const DsVec3d Rb[3] = {
		m_pBox2->GetRot().GetAxisX(),
		m_pBox2->GetRot().GetAxisY(),
		m_pBox2->GetRot().GetAxisZ(),
	};
	const DsVec3d posA = m_pBox1->GetBasePos();
	const DsVec3d posB = m_pBox2->GetBasePos();
	const DsVec3d AtoB = posB - posA;
	const DsMat33d RaT = m_pBox1->GetRot().ToTransposition();
	const DsMat33d RbT = m_pBox2->GetRot().ToTransposition();
	const DsVec3d RaTAtoB = RaT*AtoB;
	const DsVec3d RbTAtoB = RbT*AtoB;


	double depth = -DBL_MAX;
	double sign = 1.0;
	_N_AXIS axType = _N_AXIS::NONE;

	//A��B�̎��̓��ς�������s���Ȃ��悤�Ɋe�v�f���g���܂킷���߁A���ꂼ��ۑ����Ă���
	double R11, R12, R13, R21, R22, R23, R31, R32, R33;
	double Q11, Q12, Q13, Q21, Q22, Q23, Q31, Q32, Q33;
	R11 = _Dot(Ra[0], Rb[0]); R12 = _Dot(Ra[0], Rb[1]); R13 = _Dot(Ra[0], Rb[2]);
	R21 = _Dot(Ra[1], Rb[0]); R22 = _Dot(Ra[1], Rb[1]); R23 = _Dot(Ra[1], Rb[2]);
	R31 = _Dot(Ra[2], Rb[0]); R32 = _Dot(Ra[2], Rb[1]); R33 = _Dot(Ra[2], Rb[2]);
	Q11 = fabs(R11); Q12 = fabs(R12); Q13 = fabs(R13);
	Q21 = fabs(R21); Q22 = fabs(R22); Q23 = fabs(R23);
	Q31 = fabs(R31); Q32 = fabs(R32); Q33 = fabs(R33);

	//xyz�R���̕�����
	if (   (_MaxDepth3Axis(_N_AXIS::AX, RaTAtoB.x, sideA.x + (Q11 * sideB.x) + (Q12 * sideB.y) + (Q13 * sideB.z), depth, sign, axType))
		|| (_MaxDepth3Axis(_N_AXIS::AY, RaTAtoB.y, sideA.y + (Q21 * sideB.x) + (Q22 * sideB.y) + (Q23 * sideB.z), depth, sign, axType))
		|| (_MaxDepth3Axis(_N_AXIS::AZ, RaTAtoB.z, sideA.z + (Q31 * sideB.x) + (Q32 * sideB.y) + (Q33 * sideB.z), depth, sign, axType))
		|| (_MaxDepth3Axis(_N_AXIS::BX, RbTAtoB.x, sideB.x + (Q11 * sideA.x) + (Q21 * sideA.y) + (Q31 * sideA.z), depth, sign, axType))
		|| (_MaxDepth3Axis(_N_AXIS::BY, RbTAtoB.y, sideB.y + (Q12 * sideA.x) + (Q22 * sideA.y) + (Q32 * sideA.z), depth, sign, axType))
		|| (_MaxDepth3Axis(_N_AXIS::BZ, RbTAtoB.z, sideB.z + (Q13 * sideA.x) + (Q23 * sideA.y) + (Q33 * sideA.z), depth, sign, axType)))
	{
		return m_info;//�Փ˖���
	}
	DsVec3d normal = _GetNormal(axType, Ra, Rb)*sign;

	//�˂���̕����� 
	//OBB�̕ӓ��m�̕������v�Z�Ǝ����悤�Ȋ����B�v�Z���ʂ��g���܂킷�B
	//�������́A�Ⴆ�΁A(A�̃��[�J��x��)�~(A���猩��B��x��) = (1,0,0)�~(R11,R21,R31) = (0, -R31, R21) �̂悤�ɋ��߂���B
	//������������A-B�Ԃ̋����A������������A��B�̐��@�̍ő咷���A������ ����́B������A�̃��[�J�����W�ł���̂�(0,1,0)*(0, -R31, R21) = -R31 �ƂȂ�
	if (   _MaxDeoth9Axis(_N_AXIS::AX_BX, RaTAtoB.z * R21 - RaTAtoB.y * R31, (sideA.y * Q31 + sideA.z * Q21 + sideB.y * Q13 + sideB.z * Q12), DsVec3d(0, -R31, R21), depth, normal, axType)
		|| _MaxDeoth9Axis(_N_AXIS::AX_BY, RaTAtoB.z * R22 - RaTAtoB.y * R32, (sideA.y * Q32 + sideA.z * Q22 + sideB.x * Q13 + sideB.z * Q11), DsVec3d(0, -R32, R22), depth, normal, axType)
		|| _MaxDeoth9Axis(_N_AXIS::AX_BZ, RaTAtoB.z * R23 - RaTAtoB.y * R33, (sideA.y * Q33 + sideA.z * Q23 + sideB.x * Q12 + sideB.y * Q11), DsVec3d(0, -R33, R23), depth, normal, axType)

		|| _MaxDeoth9Axis(_N_AXIS::AY_BX, RaTAtoB.x * R31 - RaTAtoB.z * R11, (sideA.x * Q31 + sideA.z * Q11 + sideB.y * Q23 + sideB.z * Q22), DsVec3d(R31, 0, -R11), depth, normal, axType)
		|| _MaxDeoth9Axis(_N_AXIS::AY_BY, RaTAtoB.x * R32 - RaTAtoB.z * R12, (sideA.x * Q32 + sideA.z * Q12 + sideB.x * Q23 + sideB.z * Q21), DsVec3d(R32, 0, -R12), depth, normal, axType)
		|| _MaxDeoth9Axis(_N_AXIS::AY_BZ, RaTAtoB.x * R33 - RaTAtoB.z * R13, (sideA.x * Q33 + sideA.z * Q13 + sideB.x * Q22 + sideB.y * Q21), DsVec3d(R33, 0, -R13), depth, normal, axType)

		|| _MaxDeoth9Axis(_N_AXIS::AZ_BX, RaTAtoB.y * R11 - RaTAtoB.x * R21, (sideA.x * Q21 + sideA.y * Q11 + sideB.y * Q33 + sideB.z * Q32), DsVec3d(-R21, R11, 0), depth, normal, axType)
		|| _MaxDeoth9Axis(_N_AXIS::AZ_BY, RaTAtoB.y * R12 - RaTAtoB.x * R22, (sideA.x * Q22 + sideA.y * Q12 + sideB.x * Q33 + sideB.z * Q31), DsVec3d(-R22, R12, 0), depth, normal, axType)
		|| _MaxDeoth9Axis(_N_AXIS::AZ_BZ, RaTAtoB.y * R13 - RaTAtoB.x * R23, (sideA.x * Q23 + sideA.y * Q13 + sideB.x * Q32 + sideB.y * Q31), DsVec3d(-R23, R13, 0), depth, normal, axType))
	{
		return m_info;//�Փ˖���
	}

	if (axType == _N_AXIS::NONE){
		DS_ASSERT(false, "�����ɂ͗��Ȃ��͂�");
		return m_info;
	}

	/*
	ODE�ƈႤ�Ƃ���
	ODE:�@�������́AA����B�ւƌ������������v���X�ɂȂ�x�N�g��
	�����ł�A�����˕Ԃ������Ԃ��˂΂Ȃ�Ȃ��̂ŋt�����ɂȂ�
	*/
	//normal *= -1.0;//���̐�ODE�Ƃ̐��������Ƃ�Ȃ��Ȃ�̂ōŌ�Ŕ��]������

	//�˂���̕���������Ԑ󂩂���
	if (_N_AXIS::CROSS < axType){
		normal = m_pBox1->GetRot()*normal;
		DsVec3d pa = posA;//���������ӂ̎n�_�����߂���
		for (int i = 0; i < 3; ++i){
			const double _sign = (_Dot(normal, Ra[i]) > 0.0) ? (1.0) : (-1.0);
			pa += Ra[i] * (sideA.v[i] * _sign);
		}

		DsVec3d pb = posB;//���������ӂ̎n�_�����߂���
		for (int i = 0; i < 3; ++i){
			const double _sign = (_Dot(normal, Rb[i]) > 0.0) ? (-1.0) : (1.0);
			pb += Rb[i] * (sideB.v[i] * _sign);
		}

		DsVec3d ua;
		DsVec3d ub;
		_GetAxis(axType, Ra, Rb, ua, ub);
		double alpha;
		double beta;
		CalcNearestPoint(pa, ua, pb, ub, alpha, beta);//OBB�œ������Ă��邱�Ƃ͊m��Ȃ̂ł͂ݏo�邱�Ƃ͂Ȃ��͂�
		//alpha��beta�͂P�𒴂��Ă����Ȃ��B�ӂ̒����𒴂���̂͂܂������ǁA����͂��肦�Ȃ��͂�
		pa = pa + ua*alpha;
		pb = pb + ub*beta;

		const DsVec3d colPos = (pa + pb)*0.5;
		m_info.AddInfo(colPos, -normal, -depth, m_pBox1->RefOwnerId(), m_pBox2->RefOwnerId());
		//DsDbgSys::GetIns().RefDrawCom().SetColor(DsVec3d(0, 1, 0)).DrawSphere(colPos, 0.1);
		//DsDbgSys::GetIns().RefDrawCom().SetColor(DsVec3d(0, 1, 1)).DrawLine(colPos, colPos +(normal*10.0));
		return m_info;
	}


	//��������R�����������������ꍇ�̌v�Z
	//�����͑S��ODE

	//A�̎���B�̎����Ŋ�����ւ���
	const DsVec3d* Raa;
	const DsVec3d* Rbb;
	const DsVec3d* paa;
	const DsVec3d* pbb;
	const DsVec3d* Sa;
	const DsVec3d* Sb;
	if (axType < _N_AXIS::B_AXIS){
		Raa = Ra;
		Rbb = Rb;
		paa = &posA;
		pbb = &posB;
		Sa = &sideA;
		Sb = &sideB;
	}
	else{
		Raa = Rb;
		Rbb = Ra;
		paa = &posB;
		pbb = &posA;
		Sa = &sideB;
		Sb = &sideA;
	}
	const DsVec3d normal2 = (axType < _N_AXIS::B_AXIS) ? (normal) : (-normal);
	
	//�Փ˖ʂ��d�Ȃ�̈�������狁�߂�B

	//Raa�̒��S����ARbb�̏Փ˖ʂ̒��S�ւ̃x�N�g�������߂���
	const DsVec3d nr = DsVec3d(_Dot(Rbb[0], normal2), _Dot(Rbb[1], normal2), _Dot(Rbb[2], normal2));
	const DsVec3d anr = DsVec3d(fabs(nr.x), fabs(nr.y), fabs(nr.z));

	//Rbb�̏Փ˖ʕ����Ɛ������������߂�
	int lanr;//Rbb�Փ˖ʂ̕���
	int a1;//Rbb�Փ˖ʂɐ����ȕ���1
	int a2;//Rbb�Փ˖ʂɐ����ȕ���2
	if (anr.v[1] > anr.v[0]) {
		if (anr.v[1] > anr.v[2]) {
			a1 = 0;
			lanr = 1;
			a2 = 2;
		}
		else {
			a1 = 0;
			a2 = 1;
			lanr = 2;
		}
	}
	else {
		if (anr.v[0] > anr.v[2]) {
			lanr = 0;
			a1 = 1;
			a2 = 2;
		}
		else {
			a1 = 0;
			a2 = 1;
			lanr = 2;
		}
	}

	//Raa�̒��S����Rbb�̏Փ˖ʂ܂ł̃x�N�g��
	DsVec3d center;
	if (nr.v[lanr] < 0.0) {
		center = (*pbb) - (*paa) + (Rbb[lanr] * Sb->v[lanr]);
	}
	else {
		center = (*pbb) - (*paa) - (Rbb[lanr] * Sb->v[lanr]);
	}
	
	//const_cast<DsActor*>(pActorA)->SetLineColor(DsVec4d(1, 0, 0, 1));
	//const_cast<DsActor*>(pActorB)->SetLineColor(DsVec4d(0, 0, 1, 1));
	//DsDbgSys::GetIns().RefDrawCom().SetColor(DsVec3d(0, 1, 1)).DrawSphere(center+(*paa), 0.1);
	//DsDbgSys::GetIns().RefDrawCom().SetColor(DsVec3d(0, 1, 1)).DrawLine(center + (*paa), center + (*paa)+(normal2*10.0));

	//���x��Raa���̏Փ˖ʕ����Ɛ������������߂�
	int codeN;
	int code1;
	int code2;
	if (axType == _N_AXIS::AX) codeN = 0;
	else if (axType == _N_AXIS::AY) codeN = 1;
	else if (axType == _N_AXIS::AZ) codeN = 2;
	else if (axType == _N_AXIS::BX) codeN = 0;
	else if (axType == _N_AXIS::BY) codeN = 1;
	else if (axType == _N_AXIS::BZ) codeN = 2;
	else DS_ASSERT(false, "�����~�X");
	if (codeN == 0) {
		code1 = 1;
		code2 = 2;
	}
	else if (codeN == 1) {
		code1 = 0;
		code2 = 2;
	}
	else {
		code1 = 0;
		code2 = 1;
	}

	//Raa��Rbb�̏d�Ȃ��Ă���̈�����߂�
	//Raa�̍��W�n�ōl����
	//Rbb�̏Փ˖ʂ�Raa�̏Փ˖ʍ��W�ɓ��e���A�Q�����Ōv�Z����

	//Rbb�̏Փ˖ʂ����߂�
	double quad[8]; //���e���Rbb�̖ʂ̒��_���W
	double c1, c2;	//���e���Rbb�̒��S���W
	double m11, m12, m21, m22;//���e���Rbb�̍��W�n
	c1 = _Dot(center, Raa[code1]);
	c2 = _Dot(center, Raa[code2]);
	m11 = _Dot(Raa[code1], Rbb[a1]);
	m12 = _Dot(Raa[code1], Rbb[a2]);
	m21 = _Dot(Raa[code2], Rbb[a1]);
	m22 = _Dot(Raa[code2], Rbb[a2]);
	{
		//Ra����݂��A�ՓˈȊO�̂Q����Rb�̐}�`�̒���
		double k1 = m11*Sb->v[a1];
		double k2 = m21*Sb->v[a1];
		double k3 = m12*Sb->v[a2];
		double k4 = m22*Sb->v[a2];

		//Rbb�̖ʂ̊e���_�����߂�
		//���S�_ + Rbb�̊e���̒����iRbb�͉�]���Ă���̂�Raa�̎���Rbb�̏c���������ꂼ�ꑫ���j
		quad[0] = c1 - k1 - k3;
		quad[1] = c2 - k2 - k4;
		quad[2] = c1 - k1 + k3;
		quad[3] = c2 - k2 + k4;
		quad[4] = c1 + k1 + k3;
		quad[5] = c2 + k2 + k4;
		quad[6] = c1 + k1 - k3;
		quad[7] = c2 + k2 - k4;
	}

	//Raa�̏Փ˖ʂ����߂�BRaa������W�ɂȂ��Ă���̂ŁA���@���̂܂܁B
	const double rect[2] = {
		Sa->v[code1],
		Sa->v[code2],
	};

	double ret[16];
	const int n = _intersectRectQuad(rect, quad, ret);//�d�Ȃ�̈�
	if (n < 1){
		//DS_ASSERT(false, "box box �̓����蔻��ŖʂƖʂ̌��������O");
		return m_info;		// OBB�Ŕ���ς݂Ȃ̂ŁA�������ĂȂ����ƂȂ�Ă��肦�Ȃ��͂��B
	}


	DsVec3d point[8];//�R�����ɖ߂��������_�B�̗p���ꂽ���̂����i�[�����
	double dep[8];//�ђʐ[�x
	double det1 = 1.0/(m11*m22 - m12*m21);
	m11 *= det1;
	m12 *= det1;
	m21 *= det1;
	m22 *= det1;

	int cnum = 0;
	for (int j = 0; j < n; j++) {
		//ret �� Rbb�̃��[�J�����W�ɕϊ�
		//mxx ��Raa���W�n����݂�Rbb���W�n�Bmxx�̋t�s���Rbb�̕����(=�P�ʍs��)�Ɏ����Ă���ϊ�
		double k1 = m22*(ret[j * 2] - c1) - m12*(ret[j * 2 + 1] - c2);
		double k2 = -m21*(ret[j * 2] - c1) + m11*(ret[j * 2 + 1] - c2);

		//���S�_+Rbb���|���邱�Ƃɂ��O���[�o�����W��(center��Raa�̒��S����Rbb�̖ʂ̒��S�܂ł̃x�N�g���Ȃ̂ł܂����S�ɃO���[�o���ł͂Ȃ�)
		//Raa�̒��S��������_(Rbb�Փ˖ʏ��)�܂ł̃x�N�g��
		point[cnum] = center + (Rbb[a1]*k1) + (Rbb[a2]*k2);
		
		//Sa�̖ʂ܂ł̍��� -�����_�̖@�������̍����B���܂��Ă�Ȃ�v���X�B�}�C�i�X�͌����_�̕�������=�Փ˓_�ł͂Ȃ�
		dep[cnum] = Sa->v[codeN] - _Dot(normal2, point[cnum]);
		if (dep[cnum] >= 0.0) {
			cnum++;
		}
	}
	if (cnum < 1) {
		return m_info;//�Փ˂��Ă�������_���P���Ȃ������̂ŏՓ˖���
	}

	for (int j = 0; j < cnum; j++) {
		const DsVec3d colPos = point[j] + (*paa);
		m_info.AddInfo(colPos, -normal, dep[j], m_pBox1->RefOwnerId(), m_pBox2->RefOwnerId());
		//DsDbgSys::GetIns().RefDrawCom().SetColor(DsVec3d(0, 0, 1)).DrawSphere(colPos, 0.1);
	}

	return m_info;
}