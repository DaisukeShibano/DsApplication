#include "DsPhysicsPch.h"

#include "Collision/EachGeom/DsCollisionCapsuleRay.h"

#ifndef __DS_COLLISION_DETECTION__
#include "Collision/DsCollisionDetection.h"
#endif
#ifndef __DS_COLLISION_GEOMETRY__
#include "Collision/DsCollisionGeometry.h"
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
	//ode�Ɣ����𑵂��邽�߂̍\����
	struct _Ray
	{
		_Ray(const DsVec3d& start, const DsVec3d _dir, const double len)
			: pos(start)
			, dir(_dir)
			, length(len)
		{}
		const DsVec3d pos;//���C�̎n�_
		const DsVec3d dir;//�p����Y��
		const double length;
	};

	struct _Capsule
	{
		_Capsule(const DsVec3d& centerPos, const DsVec3d _dir, double r, double len)
			: pos(centerPos)
			, dir(_dir)
			, radius(r)
			, lz(len)
		{}
		const DsVec3d pos;//�J�v�Z���̒��S
		const DsVec3d dir;//�p����Y��
		const double radius;//�J�v�Z���̋��̔��a
		const double lz;//���̔��a���܂܂Ȃ��J�v�Z���̒���
	};

	struct _Result
	{
		DsVec3d colPos;
		DsVec3d normal;
		double depth;
	};

	//ode�̂܂��
	bool _ray_sphere_helper(const _Ray& ray, DsVec3d sphere_pos, double radius, bool mode, _Result& result)
	{
		const DsVec3d q = ray.pos - sphere_pos;
		const double B = DsVec3d::Dot(q, ray.dir);
		const double C = q.LengthSq() - radius * radius;
		// note: if C <= 0 then the start of the ray is inside the sphere
		double k = B * B - C;
		if (k < 0.0) {
			return false;
		}
		k = sqrt(k);

		double alpha;
		if (mode && (C >= 0.0) ) {
			alpha = -B + k;
			if (alpha < 0) {
				return false;
			}
		}
		else {
			alpha = -B - k;
			if (alpha < 0.0) {
				alpha = -B + k;
				if (alpha < 0) {
					return false;
				}
			}
		}
		if (alpha > ray.length) {
			return 0;
		}

		const DsVec3d colPos = ray.pos + (ray.dir*alpha);
		const double nsign = ( (C < 0.0) || mode) ? (-1.0) : (1.0);
		const DsVec3d colNormal = DsVec3d::Normalize( (colPos - sphere_pos)*nsign );
		const double depth = alpha;
		
		result.colPos = colPos;
		result.normal = colNormal;
		result.depth = depth;

		return true;
	}

	bool _dCollideRayCapsule(const _Ray& ray, const _Capsule& ccyl, _Result& result)
	{
		//ccyl->lz�̓J�v�Z���̒��������A���L�̔��������ƁA�[�̋��̔��a�͏����������ɂȂ��Ă���͗l
		double lz2 = ccyl.lz * (0.5);

		DsVec3d cs = ray.pos - ccyl.pos;
		double k = DsVec3d::Dot(ccyl.dir, cs);	// position of ray start along ccyl axis

		//q�̓��C�̎n�_����k*R�̓_�ւ̃x�N�g��
		DsVec3d q = ccyl.dir*k - cs;
		double C = DsVec3d::Dot(q, q) - ccyl.radius*ccyl.radius;
		// if C < 0 then ray start position within infinite extension of cylinder

		// see if ray start position is inside the capped cylinder
		bool inside_ccyl = false;
		if (C < 0.0) {//���C�̎n�_���V�����_�[�̑����̓����B���̎��_�ł͒����͌��ĂȂ��B

			if (k < -lz2) {
				k = -lz2;
			}
			else if (k > lz2) {
				k = lz2;
			}

			//�N�����v����k��r���΍��W�ցB
			DsVec3d r = ccyl.pos + (ccyl.dir*k);

			//ray�n�_��r���猩�����W��
			//k���N�����v���ꂽ�Ă���̂ŁA���x�͒������܂߂ăJ�v�Z���̓����B
			const DsVec3d toRay = ray.pos - r;
			if (toRay.LengthSq() < ccyl.radius*ccyl.radius) {
				inside_ccyl = true;
			}
		}

		// compute ray collision with infinite cylinder, except for the case where
		// the ray is outside the capped cylinder but within the infinite cylinder
		// (it that case the ray can only hit endcaps)
		if (!inside_ccyl && C < 0) {

			//�����ɂ�������́A���C�̎n�_���J�v�Z���́u�����v�̒��ɓ����Ă��邪�A�u�����v�̊O�ɂ���Ƃ��B
			/*
			_______
			�_   �E�������Ƃ�
			|
			-------/
			�E�������͑����̒��ɂ�������ĂȂ�inside_ccyl��false��C��0�ȏ�
			*/


			// set k to cap position to check
			if (k < 0.0) {
				k = -lz2;
			}
			else {
				k = lz2;
			}
		}
		else {
			double uv = DsVec3d::Dot(ccyl.dir, ray.dir);
			DsVec3d r = ccyl.dir*uv - ray.dir;
			double A = r.LengthSq();
			double B = 2.0 * DsVec3d::Dot(q, r);
			k = B * B - 4.0 * A*C;
			if (k < 0.0) {
				// the ray does not intersect the infinite cylinder, but if the ray is
				// inside and parallel to the cylinder axis it may intersect the end
				// caps. set k to cap position to check.
				if (!inside_ccyl) {
					return false;
				}

				if (uv < 0.0) {
					k = -lz2;
				}
				else {
					k = lz2;
				}
			}
			else {
				k = sqrt(k);
				A = 1.0/(2.0 * A);
				double alpha = (-B - k)*A;
				if (alpha < 0) {
					alpha = (-B + k)*A;
					if (alpha < 0.0) {
						return false;
					}
				}

				if (alpha > ray.length) {
					return false;
				}

				// the ray intersects the infinite cylinder. check to see if the
				// intersection point is between the caps
				const DsVec3d colPos = ray.pos + (ray.dir*alpha);
				q = colPos - ccyl.pos;
				k = DsVec3d::Dot(q, ccyl.dir);
				double nsign = inside_ccyl ? (-1.0) : (1.0);
				if ( (k >= -lz2) && (k <= lz2) ) {
					const DsVec3d colNormal = DsVec3d::Normalize( ( colPos - ( ccyl.pos + (ccyl.dir*k) ) ) * nsign );
					const double depth = alpha;

					result.colPos = colPos;
					result.normal = colNormal;
					result.depth = depth;

					return true;
				}

				// the infinite cylinder intersection point is not between the caps.
				// set k to cap position to check.
				if (k < 0.0) {
					k = -lz2;
				}
				else {
					k = lz2;
				}
			}
		}

		// check for ray intersection with the caps. k must indicate the cap
		// position to check
		q = ccyl.pos + (ccyl.dir*k);
		return _ray_sphere_helper(ray, q, ccyl.radius, inside_ccyl, result);
	}
}


DsCollisionResult& DsCollisionCapsuleRay::Collide()
{
	m_info.Clear();
	if (_ColideAABB()){
		const DsActorId& o1 = m_pCapsule->RefOwnerId();
		const DsActorId& o2 = m_pRay->RefOwnerId();

		if (m_world.GetCollisionCallback()) {
			if (!m_world.GetCollisionCallback()->IsCollide(*o1.GetActor(), *o2.GetActor())) {
				return m_info;
			}
		}

		const DsVec3d start = m_pRay->GetVertex()[0];
		const DsVec3d end = m_pRay->GetVertex()[1];
		const double rayLen = (start - end).Length();
		_Ray ray(start, m_pRay->GetRot().GetAxisY(), rayLen);

		const DsVec3d centerPos = m_pCapsule->GetBasePos();
		const double radius = m_pCapsule->GetSide().x;
		const double capLen = m_pCapsule->GetSide().y;
		_Capsule capsule(centerPos, m_pCapsule->GetRot().GetAxisY(), radius, capLen);

		_Result result;
		bool isHit = _dCollideRayCapsule(ray, capsule, result);
		if (isHit) {
			m_info.AddInfo(result.colPos, result.normal, result.depth, o1, o2);
		}
	}

	return m_info;
}

bool DsCollisionCapsuleRay::_ColideAABB()
{
	const DsAabb* pAabb1 = m_pRay->GetAabb();
	const DsAabb* pAabb2 = m_pCapsule->GetAabb();

	if (pAabb1 && pAabb2){
		return DsAabb::IsContain(*pAabb1, *pAabb2);
	}
	else{
		return true;
	}
}