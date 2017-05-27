#ifndef __DS_COLLISION_DETECTION__
#define __DS_COLLISION_DETECTION__

namespace DsPhysics
{
	struct DsQuad;
	struct DsLine;
	class DsCollisionResult;
	class DsActorId;
	class DsCollisionContext;
}

namespace DsPhysics
{
	bool DsCD_PointFace( const DsCollisionContext* pCube1, const DsCollisionContext* pCube2, DsCollisionResult& info );
	bool DsCD_PointFace( const DsActorId& p1Id, const DsVec3d& p1, const DsCollisionContext* pCube2, DsCollisionResult& info);
	bool DsCD_LineFace( const DsCollisionContext* pCube1, const DsCollisionContext* pCube2, DsCollisionResult& info );
	bool DsCD_LineFaceVelocity(const DsCollisionContext* pCube1, const DsCollisionContext* pCube2, DsCollisionResult& info);
	bool DsCD_TriangleFace(const DsCollisionContext* pMesh, const DsCollisionContext* pCube, DsCollisionResult& info);

	//�ʒup1����Ƃ���pConvex��normal�����̈�Ԑ[���[�x�����߂�
	double DsCD_NormalFaceDepthMax(const DsVec3d& p1, const DsVec3d& normal, const DsCollisionContext* pConvex);

	//�֗��n
	/*
		v1��v2�̍Őڋߓ_
		p1 v1��̍Őڋߓ_
		p2 v2��̍Őڋߓ_
		v1start ��1�̎n�_
		v1end ��1�̏I�_
		v2start ��2�̎n�_
		v2end ��2�̏I�_
	*/
	inline void CalcNearestPoint(DsVec3d& p1, DsVec3d& p2, const DsVec3d& v1start, const DsVec3d& v1end, const DsVec3d& v2start, const DsVec3d& v2end)
	{
		const DsVec3d v1Vec = v1start - v1end;
		const DsVec3d v2Vec = v2start - v2end;
		const DsVec3d v1n = DsVec3d::Normalize(v1Vec);
		const DsVec3d v2n = DsVec3d::Normalize(v2Vec);

		const DsVec3d v2_v1 = v2start - v1start;
		const double d1 = DsVec3d::Dot(v2_v1, v1n);
		const double d2 = DsVec3d::Dot(v2_v1, v2n);
		const double dv = DsVec3d::Dot(v1n, v2n);

		const double dvdv = dv*dv;
		if (1.0 <= dvdv) return;
		const double t1 = min( v1Vec.Length(), (d1 - d2*dv) / (1.0-dvdv) );
		const double t2 = min( v2Vec.Length(), (d2 - d1*dv) / (dvdv-1.0) );

		p1 = v1start + (v1n*t1);
		p2 = v2start + (v2n*t2);
	}
}

#endif