#ifndef __DS_COLLISION_DETECTION__
#define __DS_COLLISION_DETECTION__

namespace DsPhysics
{
	struct DsQuad;
	struct DsLine;
	class DsCollisionResult;
	class DsActorId;
	class DsCollisionGeometry;
}

namespace DsPhysics
{
	bool DsCD_PointFace( const DsCollisionGeometry* pCube1, const DsCollisionGeometry* pCube2, DsCollisionResult& info );
	bool DsCD_PointFace( const DsActorId& p1Id, const DsVec3d& p1, const DsCollisionGeometry* pCube2, DsCollisionResult& info);
	bool DsCD_LineFace( const DsCollisionGeometry* pCube1, const DsCollisionGeometry* pCube2, DsCollisionResult& info );
	bool DsCD_LineFaceVelocity(const DsCollisionGeometry* pCube1, const DsCollisionGeometry* pCube2, DsCollisionResult& info);
	bool DsCD_TriangleFace(const DsCollisionGeometry* pMesh, const DsCollisionGeometry* pCube, DsCollisionResult& info);

	//�ʒup1����Ƃ���pConvex��normal�����̈�Ԑ[���[�x�����߂�
	double DsCD_NormalFaceDepthMax(const DsVec3d& p1, const DsVec3d& normal, const DsCollisionGeometry* pConvex);

	//�֗��n
	/*
		@param[in] pa �ӂP�̎n�_
		@param[in] ua �ӂP
		@param[in] pb �ӂQ�̎n�_
		@param[in] ub �ӂQ
		@param[out] alpha �ӂP�̌����_�ւ̒���
		@param[out] beta �ӂQ�̌����_�ւ̒���
		@par �������͂ݏo���Ƃ��̂��Ƃ͍l������Ă��Ȃ�
	*/
	inline bool CalcNearestPoint(const DsVec3d& pa, const DsVec3d& ua, const DsVec3d& pb, const DsVec3d& ub, double& alpha, double& beta)
	{
		const DsVec3d p = pb - pa;
		double uaub = DsVec3d::Dot(ua, ub);
		double q1 = DsVec3d::Dot(ua, p);
		double q2 = -DsVec3d::Dot(ub, p);
		double d = 1.0 - uaub*uaub;
		if (d <= DBL_EPSILON) {
			alpha = 0;
			beta = 0;
			return false;
		}
		else {
			d = 1.0 / d;
			alpha = (q1 + uaub*q2)*d;
			beta = (uaub*q1 + q2)*d;
			return true;
		}
	}

}

#endif