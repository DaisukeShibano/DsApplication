#include "DsPhysicsPch.h"

#ifndef _DS_COLLISION_CAPSULE_MESH_H_
#include "Collision/EachGeom/DsCollisionCapsuleMesh.h"
#endif
#ifndef __DS_COLLISION_DETECTION__
#include "Collision/DsCollisionDetection.h"
#endif
#ifndef __DS_COLLISION_CONTEXT__
#include "Collision/DsCollisionContext.h"
#endif
#ifndef __DS_BOUNDING_TREE_BASE__
#include "Collision/BoundingTree/DsBoundingTreeBase.h"
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
	//ode��

	class _TrimeshCapsuleCollider
	{
	public:
		_TrimeshCapsuleCollider(const DsCollisionContext* s)
			:m_pSphere(s)
		{

		}

	public:
		//�P�̎O�p�`���ƂɎg���܂킷
		void Initialize(const DsVec3d* pV, const DsVec3d& normal){
			m_pV = pV;
			m_vN = normal;
		}

		bool IsColide(DsVec3d& resultColPos, DsVec3d& resultNormal, double& resultDepth)
		{
			return _cldTestOneTriangleVSCapsule(resultColPos, resultNormal, resultDepth);
		}

	private:

		bool _cldTestOneTriangleVSCapsule(DsVec3d& resultColPos, DsVec3d& resultNormal, double& resultDepth)
		{
			// create plane from triangle
			const double plDistance = DsVec3d::Dot(m_pV[0], m_vN);
			// calculate capsule distance to plane
			const double fDistanceCapsuleCenterToPlane = DsVec3d::Dot(m_vN, m_pSphere->GetBasePos()) - plDistance;

			// Capsule must be over positive side of triangle
			if (fDistanceCapsuleCenterToPlane < 0.0)
			{
				// if not don't generate contacts
				return false;
			}

			m_iBestAxis = 0;
			m_fBestDepth = -DBL_MAX;
			m_fBestCenter = 0;
			m_fBestrt = 0;
			m_iBestAxis = 0;


			const DsVec3d vCapsulePosition = m_pSphere->GetBasePos();
			// Translate triangle to Cc cord.
			m_vV0 = m_pV[0] - vCapsulePosition;
			m_vV1 = m_pV[1] - vCapsulePosition;
			m_vV2 = m_pV[2] - vCapsulePosition;
			
			m_vCapsuleAxis = m_pSphere->GetRot().GetAxisY();

			m_vE0 = m_pV[1] - m_pV[0];
			m_vE1 = m_pV[2] - m_pV[1];
			m_vE2 = m_pV[0] - m_pV[2];

			// do intersection test and find best separating axis
			if (!_cldTestSeparatingAxesOfCapsule())
			{
				// if not found do nothing
				return false;
			}

			if (m_iBestAxis == 0)
			{
				return false;
			}


			const double vCapsuleRadius = m_pSphere->GetSide().x;
			const double fCapsuleSize = m_pSphere->GetSide().y;
			//�O�p�`���ђʂ��Ă�J�v�Z���̃x�N�g�������߂�
			// vCposTrans�͑��������A�J�v�Z���̗��[�͋��Ȃ̂ŁA���̕����Ɋւ��Ă͂ǂꂾ�����������ʂɂ߂荞��ł邩�́A��ɔ��a���ň��B
			// m_vCapsuleAxis[0] * (m_fCapsuleSize*REAL(0.5) - m_vCapsuleRadius); �͒[�̋��̕������������A�~���̒��S��ʂ��
			// ��������̔��a�����������ʂɃI�t�Z�b�g���邱�ƂŁA�O�p�`���ђʂ���A���������ʂɈ�Ԑ[���A�J�v�Z���̃x�N�g���ɂȂ�̂��Ǝv���B
			// ���������̗̍p��͈�Ԑ󂢂��̂ŁA������̗p�����Ƃ��A�J�v�Z���̂ǂ������(������������)�[���߂荞��ł���x�N�g���Ȃ̂���m�肽�����Ă���
			DsVec3d vCposTrans;
			vCposTrans = vCapsulePosition + m_vNormal * vCapsuleRadius;

			DsVec3d vCEdgePoint0;
			vCEdgePoint0 = vCposTrans + m_vCapsuleAxis * (fCapsuleSize*(0.5) - vCapsuleRadius);

			DsVec3d vCEdgePoint1;
			vCEdgePoint1 = vCposTrans - m_vCapsuleAxis * (fCapsuleSize*(0.5) - vCapsuleRadius);

			// transform capsule edge points into triangle space
			vCEdgePoint0 -= m_pV[0];
			vCEdgePoint1 -= m_pV[0];

			DsVec4d plPlane;
			// triangle plane
			plPlane.Set(- m_vN, 0.0);

			if (!_cldClipEdgeToPlane(vCEdgePoint0, vCEdgePoint1, plPlane))
			{
				return false;
			}

			// plane with edge 0
			DsVec3d vTemp;
			vTemp = DsVec3d::Cross(m_vN, m_vE0);
			plPlane.Set(vTemp, (1e-5));
			if (!_cldClipEdgeToPlane(vCEdgePoint0, vCEdgePoint1, plPlane))
			{
				return false;
			}

			vTemp = DsVec3d::Cross( m_vN, m_vE1);
			plPlane.Set( vTemp, -(DsVec3d::Dot(m_vE0, vTemp) - (1e-5)));
			if (!_cldClipEdgeToPlane(vCEdgePoint0, vCEdgePoint1, plPlane))
			{
				return false;
			}

			vTemp = DsVec3d::Cross( m_vN, m_vE2);
			plPlane.Set( vTemp,(1e-5));
			if (!_cldClipEdgeToPlane(vCEdgePoint0, vCEdgePoint1, plPlane)) {
				return false;
			}

			// return capsule edge points into absolute space
			vCEdgePoint0 += m_pV[0];
			vCEdgePoint1 += m_pV[0];

			// calculate depths for both contact points
			vTemp = vCEdgePoint0 - vCapsulePosition;
			double fDepth0 = DsVec3d::Dot(vTemp, m_vNormal) - (m_fBestCenter - m_fBestrt);
			vTemp = vCEdgePoint1 - vCapsulePosition;
			double fDepth1 = DsVec3d::Dot(vTemp, m_vNormal) - (m_fBestCenter - m_fBestrt);

			// clamp depths to zero
			if (fDepth0 < 0.0)
			{
				fDepth0 = 0.0;
			}

			if (fDepth1 < 0.0)
			{
				fDepth1 = 0.0;
			}

			resultColPos = vCEdgePoint1;
			resultNormal = m_vNormal;
			resultDepth = fDepth1;

			return true;
		}


		bool _cldTestSeparatingAxesOfCapsule()
		{
			const double vCapsuleRadius = m_pSphere->GetSide().x;
			const double fCapsuleSize = m_pSphere->GetSide().y;
			const DsVec3d vCapsulePosition = m_pSphere->GetBasePos();

			//�~���̏�
			const DsVec3d vCp0 = vCapsulePosition + m_vCapsuleAxis * (fCapsuleSize*(0.5) - vCapsuleRadius);
			//�~���̉�
			const DsVec3d vCp1 = vCapsulePosition - m_vCapsuleAxis * (fCapsuleSize*(0.5) - vCapsuleRadius);

			// reset best axis
			int iBestAxis = 0;
			// reset best depth
			double fBestDepth = -DBL_MAX;
			// reset separating axis vector
			DsVec3d vAxis = -m_vN;


			//����������B�S����19��
			//vAxis��������

			if (!_cldTestAxis(vAxis, 1, true))
			{
				return false;
			}

			
			// axis CxE0 - Edge 0
			vAxis = DsVec3d::Cross(m_vCapsuleAxis, m_vE0);
			if (vAxis.LengthSq() > DBL_EPSILON) {
				if (!_cldTestAxis(vAxis, 2)) {
					return false;
				}
			}

			
			// axis CxE1 - Edge 1
			vAxis = DsVec3d::Cross(m_vCapsuleAxis, m_vE1);
			if (vAxis.LengthSq() > DBL_EPSILON) {
				if (!_cldTestAxis(vAxis, 3)) {
					return false;
				}
			}

			
			// axis CxE2 - Edge 2
			vAxis = DsVec3d::Cross( m_vCapsuleAxis, m_vE2);
			if (vAxis.LengthSq() > DBL_EPSILON) {
				if (!_cldTestAxis(vAxis, 4)) {
					return false;
				}
			}

			// first capsule point
			_CalculateAxis(vCp0, m_pV[0], m_vE0, vAxis);
			//	vAxis = ( ( vCp0-v0) cross vE0 ) cross vE0;
			if (vAxis.LengthSq() > DBL_EPSILON) {
				if (!_cldTestAxis(vAxis, 5)) {
					return false;
				}
			}

			// axis ((Cp0-V1) x E1) x E1
			_CalculateAxis(vCp0, m_pV[1], m_vE1, vAxis);
			//vAxis = ( ( vCp0-v1) cross vE1 ) cross vE1;
			if (vAxis.LengthSq() > DBL_EPSILON) {
				if (!_cldTestAxis(vAxis, 6)) {
					return false;
				}
			}

			// axis ((Cp0-V2) x E2) x E2
			_CalculateAxis(vCp0, m_pV[2], m_vE2, vAxis);
			//vAxis = ( ( vCp0-v2) cross vE2 ) cross vE2;
			if (vAxis.LengthSq() > DBL_EPSILON) {
				if (!_cldTestAxis(vAxis, 7)) {
					return false;
				}
			}

			// second capsule point
			// axis ((Cp1-V0) x E0) x E0
			_CalculateAxis(vCp1, m_pV[0], m_vE0, vAxis);
			//vAxis = ( ( vCp1-v0 ) cross vE0 ) cross vE0;
			if (vAxis.LengthSq() > DBL_EPSILON) {
				if (!_cldTestAxis(vAxis, 8)) {
					return false;
				}
			}

			// axis ((Cp1-V1) x E1) x E1
			_CalculateAxis(vCp1, m_pV[1], m_vE1, vAxis);
			//vAxis = ( ( vCp1-v1 ) cross vE1 ) cross vE1;
			if (vAxis.LengthSq() > DBL_EPSILON) {
				if (!_cldTestAxis(vAxis, 9)) {
					return false;
				}
			}

			// axis ((Cp1-V2) x E2) x E2
			_CalculateAxis(vCp1, m_pV[2], m_vE2, vAxis);
			//vAxis = ( ( vCp1-v2 ) cross vE2 ) cross vE2;
			if (vAxis.LengthSq() > DBL_EPSILON) {
				if (!_cldTestAxis( vAxis, 10)) {
					return false;
				}
			}

			// first vertex on triangle
			// axis ((V0-Cp0) x C) x C
			_CalculateAxis(m_pV[0], vCp0, m_vCapsuleAxis, vAxis);
			//vAxis = ( ( v0-vCp0 ) cross m_vCapsuleAxis ) cross m_vCapsuleAxis;
			if (vAxis.LengthSq() > DBL_EPSILON) {
				if (!_cldTestAxis(vAxis, 11)) {
					return false;
				}
			}

			// second vertex on triangle
			// axis ((V1-Cp0) x C) x C
			_CalculateAxis(m_pV[1], vCp0, m_vCapsuleAxis, vAxis);
			//vAxis = ( ( v1-vCp0 ) cross vCapsuleAxis ) cross vCapsuleAxis;
			if (vAxis.LengthSq() > DBL_EPSILON) {
				if (!_cldTestAxis(vAxis, 12)) {
					return false;
				}
			}

			// third vertex on triangle
			// axis ((V2-Cp0) x C) x C
			_CalculateAxis(m_pV[2], vCp0, m_vCapsuleAxis, vAxis);
			//vAxis = ( ( v2-vCp0 ) cross vCapsuleAxis ) cross vCapsuleAxis;
			if ( vAxis.LengthSq() > DBL_EPSILON) {
				if (!_cldTestAxis( vAxis, 13)) {
					return false;
				}
			}

			// Test as separating axes direction vectors between each triangle
			// edge and each capsule's cap center

			// first triangle vertex and first capsule point
			//vAxis = v0 - vCp0;
			vAxis = m_pV[0]-vCp0;
			if (vAxis.LengthSq() > DBL_EPSILON) {
				if (!_cldTestAxis(vAxis, 14)) {
					return false;
				}
			}

			// second triangle vertex and first capsule point
			//vAxis = v1 - vCp0;
			vAxis = m_pV[1]- vCp0;
			if (vAxis.LengthSq() > DBL_EPSILON) {
				if (!_cldTestAxis(vAxis, 15)) {
					return false;
				}
			}

			// third triangle vertex and first capsule point
			//vAxis = v2 - vCp0;
			vAxis = m_pV[2]-vCp0;
			if (vAxis.LengthSq() > DBL_EPSILON) {
				if (!_cldTestAxis( vAxis, 16)) {
					return false;
				}
			}

			// first triangle vertex and second capsule point
			//vAxis = v0 - vCp1;
			vAxis = m_pV[0]- vCp1;
			if (vAxis.LengthSq() > DBL_EPSILON) {
				if (!_cldTestAxis(vAxis, 17)) {
					return false;
				}
			}

			// second triangle vertex and second capsule point
			//vAxis = v1 - vCp1;
			vAxis = m_pV[1]-vCp1;
			if (vAxis.LengthSq() > DBL_EPSILON) {
				if (!_cldTestAxis(vAxis, 18)) {
					return false;
				}
			}

			// third triangle vertex and second capsule point
			//vAxis = v2 - vCp1;
			vAxis = m_pV[2]- vCp1;
			if (vAxis.LengthSq() > DBL_EPSILON) {
				if (!_cldTestAxis(vAxis, 19)) {
					return false;
				}
			}

			return true;
		}

		/*
		vAxis ������
		vV �O�p���_- �J�v�Z���ʒu
		*/
		bool _cldTestAxis(DsVec3d vAxis, int iAxis, bool bNoFlip = false)
		{
			//��������vAxis�ɁA�J�v�Z���ƎO�p�`�𓊉e���A�d�Ȃ蔻��

			const double fL = vAxis.Length();

			// if not long enough
			// TODO : dReal epsilon please
			if (fL < (1e-5))
			{
				// do nothing
				//iLastOutAxis = 0;
				return true;
			}

			vAxis = vAxis / fL;

			const double capsuleRadius = m_pSphere->GetSide().x;
			const double capsuleSize = m_pSphere->GetSide().y;

			// project capsule on vAxis
			//vAxis�ɃJ�v�Z���𓊉e
			const double frc = fabs(DsVec3d::Dot(m_vCapsuleAxis, vAxis))*(capsuleSize*0.5 - capsuleRadius) + capsuleRadius;

			// project triangle on vAxis
			// ���_��vAxis�ɓ��e
			double afv[3];
			afv[0] = DsVec3d::Dot(m_vV0, vAxis);
			afv[1] = DsVec3d::Dot(m_vV1, vAxis);
			afv[2] = DsVec3d::Dot(m_vV2, vAxis);

			double fMin = DBL_MAX;
			double fMax = -DBL_MAX;

			// �������Ɉ�Ԓ����������Z�o
			for (int i = 0; i < 3; i++)
			{
				fMin = min(fMin, afv[i]);
				fMax = max(fMax, afv[i]);
			}

			// �O�p�`�̓��e��̒��S�u���W�v
			const double fCenter = (fMin + fMax)*(0.5);
			// �O�p�`�̓��e��̑S�������̔����B 
			const double fTriangleRadius = (fMax - fMin)*(0.5);

			// calculate depth 
			const double fDepth = fabs(fCenter) - (frc + fTriangleRadius);

			if (fDepth > 0.0)
			{
				// ����́A�O�p�`���S���W���A�J�v�Z�����a�ƎO�p�`���a�̍��v���������̂ł��̎��ł͏Փ˂Ȃ�
				return false;
			}

			//��Ԑ󂢂̂ŁA���̂Ƃ��낱�ꂪ�Փˉ����������
			if (fDepth > m_fBestDepth)
			{
				// remember depth
				m_fBestDepth = fDepth;
				m_fBestCenter = fCenter;
				m_fBestrt = fTriangleRadius;

				m_vNormal = vAxis;

				m_iBestAxis = iAxis;

				// flip normal if interval is wrong faced
				if (fCenter < 0.0 && (!bNoFlip) )
				{
					m_vNormal = -m_vNormal;
					m_fBestCenter = -fCenter;
				}
			}
			return true;
		}

		/*
			v1-v2 �� v3 �̐������ʂ́Av3���̊O�֌������x�N�g��
		*/
		inline void _CalculateAxis(const DsVec3d& v1, const DsVec3d& v2, const DsVec3d& v3, DsVec3d& r)
		{
			const DsVec3d t1 = v1-v2;
			const DsVec3d t2 = DsVec3d::Cross( t1, v3);
			r = DsVec3d::Cross( t2, v3);
		}

		//plane��vEpnt0-vEpnt1��ؒf�BvEpnt0��vEpnt1�ɂ͐ؒf�������Z���Ȃ����̂�����
		bool _cldClipEdgeToPlane(DsVec3d& vEpnt0, DsVec3d& vEpnt1, const DsVec4d& plPlane)
		{
			// calculate distance of edge points to plane
			const double fDistance0 = DsVec4d::PointDistance(plPlane, vEpnt0);
			const double fDistance1 = DsVec4d::PointDistance(plPlane, vEpnt1);

			// if both points are behind the plane
			if (fDistance0 < 0 && fDistance1 < 0)
			{
				// do nothing
				return false;
				// if both points in front of the plane
			}
			else if (fDistance0 > 0 && fDistance1 > 0)
			{
				// accept them
				return true;
				// if we have edge/plane intersection
			}
			else if ((fDistance0 > 0 && fDistance1 < 0) || (fDistance0 < 0 && fDistance1 > 0))
			{
				//�����_
				// find intersection point of edge and plane
				const DsVec3d vIntersectionPoint = vEpnt0 - (vEpnt0 - vEpnt1)*fDistance0 / (fDistance0 - fDistance1);

				// clamp correct edge to intersection point
				if (fDistance0 < 0.0)
				{
					vEpnt0 = vIntersectionPoint;
				}
				else
				{
					vEpnt1 = vIntersectionPoint;
				}
				return true;
			}
			return true;
		}

	private:
		//�ŏ�����Ō�܂ŕς��Ȃ��A�\�ߕ������Ă��
		const DsCollisionContext* m_pSphere;
		const DsVec3d* m_pV;
		DsVec3d m_vN;
		//���ƍŏ��̕��ɕ�����A�ȍ~�ς��Ȃ���
		DsVec3d m_vE0;//�O�p�`�G�b�W
		DsVec3d m_vE1;
		DsVec3d m_vE2;
		DsVec3d m_vCapsuleAxis;//�J�v�Z��Y��
		DsVec3d m_vV0;//�J�v�Z�����S����O�p�`���_�܂ł̃x�N�g��
		DsVec3d m_vV1;
		DsVec3d m_vV2;
		//�p�ɂɕς���
		double m_fBestDepth;//�Փˌ���
		double m_fBestCenter;//�Փˌ���
		double m_fBestrt;//�Փˌ���
		double m_iBestAxis;//�Փˌ���
		DsVec3d m_vNormal;//�Փˌ���

	};
}


DsCollisionResult& DsCollisionCapsuleMesh::Collide()
{
	m_info.Clear();
	if (_ColideAABB()){
		const DsActorId& o1 = m_pCapsule->RefOwnerId();
		const DsActorId& o2 = m_pMesh->RefOwnerId();

		if (m_world.GetCollisionCallback()) {
			if (!m_world.GetCollisionCallback()->IsCollide(*o1.GetActor(), *o2.GetActor())) {
				return m_info;
			}
		}

		_TrimeshCapsuleCollider colider(m_pCapsule);

		const DsVec3d* pVertex = m_pMesh->GetVertex();
		const int fn = m_pMesh->GetFaceNum();
		const DsQuad* pFace = m_pMesh->GetFace();
		for (int i = 0; i < fn; ++i, ++pFace) {
			const DsVec3d v[3] = {
				pVertex[pFace->index[0]],
				pVertex[pFace->index[1]],
				pVertex[pFace->index[2]],
			};

			colider.Initialize(v, pFace->normal);
			DsVec3d colPos;
			DsVec3d normal;
			double depth;
			if (colider.IsColide(colPos, normal, depth)) {
				m_info.AddInfo(colPos, normal, depth, o1, o2);
			}
		}

	}

	return m_info;
}

bool DsCollisionCapsuleMesh::_ColideAABB()
{
	const DsBoundingTreeBase* pMTree = m_pMesh->GetBoungingTree();
	const DsBoundingTreeBase* pBTree = m_pCapsule->GetBoungingTree();

	if (pMTree && pBTree){
		return pBTree->IsContain(*pMTree);
	}
	else{
		return true;
	}
}