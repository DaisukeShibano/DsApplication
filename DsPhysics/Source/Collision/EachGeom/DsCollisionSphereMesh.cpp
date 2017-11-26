#include "DsPhysicsPch.h"

#ifndef _DS_COLLISION_SPHERE_MESH_H_
#include "Collision/EachGeom/DsCollisionSphereMesh.h"
#endif
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
	//bulletの当たり判定。odeのはよく分からんかった。一見odeの方が計算量少なくて良いように見える。理解できれば採用したい

	//衝突判定結果
	struct _ColideResult
	{
		DsVec3d normal;
		DsVec3d point;
		double depth;
	};

	/*
	@brief 球の中心点が三角形内にいるか判定
	@param[in] 三角形の頂点
	@param[in] 三角形の法線
	@param[in] 球の中心点
	@return 三角形内にいるか
	*/
	bool IsPointInTriangle(const DsVec3d* pVertices, const DsVec3d& normal, const DsVec3d& p)
	{
		const DsVec3d& p1 = pVertices[0];
		const DsVec3d& p2 = pVertices[1];
		const DsVec3d& p3 = pVertices[2];

		const DsVec3d edge1(p2 - p1);
		const DsVec3d edge2(p3 - p2);
		const DsVec3d edge3(p1 - p3);

		const DsVec3d p1_to_p(p - p1);
		const DsVec3d p2_to_p(p - p2);
		const DsVec3d p3_to_p(p - p3);

		const DsVec3d edge1_normal( DsVec3d::Cross(edge1, normal));
		const DsVec3d edge2_normal( DsVec3d::Cross(edge2, normal));
		const DsVec3d edge3_normal( DsVec3d::Cross(edge3, normal));

		const double r1 = DsVec3d::Dot( edge1_normal, p1_to_p);
		const double r2 = DsVec3d::Dot( edge2_normal, p2_to_p);
		const double r3 = DsVec3d::Dot( edge3_normal, p3_to_p);
		if (( (r1 > 0.0) && (r2 > 0.0) && (r3 > 0.0) ) ||
			( (r1 <= 0.0) && (r2 <= 0.0) && (r3 <= 0.0) )) {//裏表どっちでもいけるようにしているのだと思う
			return true;
		}

		return false;
	}

	/*
	@brief 判定点と線分の最接近点を求める
	@param[in] 始点
	@param[in] 終点
	@param[in] 判定点
	@param[out] 最接近点
	@return 辺と点の最接近距離の２乗
	*/
	double GetSegmentSqrDistance(const DsVec3d& from, const DsVec3d& to, const DsVec3d &p, DsVec3d &nearest)
	{
		DsVec3d diff = p - from;
		const DsVec3d v = to - from;
		double t = DsVec3d::Dot( v, diff);

		if (t > 0.0) {
			const double dotVV = DsVec3d::Dot( v, v);
			if (t < dotVV) {
				t /= dotVV;
				diff -= v*t;//辺に垂直な成分が最接近距離
			}
			else {
				//辺をはみ出てた
				t = 1;//to全振り
				diff -= v;//to側の端っこからpまでが最接近距離
			}
		}
		else {
			t = 0.0;
			//最接近距離はfrom側でdiffで既に出しているのですることなし
		}
		nearest = from + v*t;
		return DsVec3d::Dot( diff, diff);
	}

	/*
	@brief 球と三角形の当たり判定
	@param[in] 球
	@param[in] 三角形の法線
	@param[in] 三角形の頂点
	@param[in] 三角形の頂点数
	@param[out] 衝突判定結果
	@return 当たってるか
	*/
	bool IsColide(const DsCollisionGeometry* pSphere, DsVec3d normal, const DsVec3d* pVertex, const int vertexNum, _ColideResult& result)
	{
		const double radius = pSphere->GetSide().x;
		const double radiusWithThreshold = radius /*+ contactBreakingThreshold 0.02くらい */;

		const DsVec3d sphereCenter = pSphere->GetBasePos();
		const DsVec3d p1ToCentre = sphereCenter - pVertex[0];
		double distanceFromPlane = DsVec3d::Dot( p1ToCentre, normal);

		if (distanceFromPlane < 0.0)
		{
			//triangle facing the other way
			distanceFromPlane *= -1.0;
			normal *= -1.0;
		}

		//三角形と球の高さチェック
		const bool isInsideContactPlane = distanceFromPlane < radiusWithThreshold;

		bool hasContact = false;
		DsVec3d contactPoint;
		if (isInsideContactPlane) {//高さ的には衝突の可能性あり。
			if (IsPointInTriangle(pVertex, normal, sphereCenter)) {
				// Inside the contact wedge - touches a point on the shell plane
				//球の中心点が三角形の内側にあった。
				//中心点が内側なので、衝突点は中心点から面に垂直な方向に垂らして面と交差したとこ
				hasContact = true;
				contactPoint = sphereCenter - normal*distanceFromPlane;
			}
			else {
				// Could be inside one of the contact capsules
				// 中心点が三角形の外側だったので、それぞれ3辺の最接近距離を求める
				double contactCapsuleRadiusSqr = radiusWithThreshold*radiusWithThreshold;
				DsVec3d nearestOnEdge;
				for (int i = 0; i < vertexNum; i++) {

					DsVec3d pa = pVertex[i];
					DsVec3d pb = pVertex[(i + 1) % 3];

					const double distanceSqr = GetSegmentSqrDistance(pa, pb, sphereCenter, nearestOnEdge);
					if (distanceSqr < contactCapsuleRadiusSqr) {
						// Yep, we're inside a capsule
						// 最接近距離が半径以下だったので衝突
						// 衝突点は辺と中心点の最接近点
						hasContact = true;
						contactPoint = nearestOnEdge;
					}

				}
			}
		}

		if (hasContact) {
			DsVec3d contactToCentre = sphereCenter - contactPoint;
			const double distanceSqr = contactToCentre.LengthSq();

			if (distanceSqr < radiusWithThreshold*radiusWithThreshold)
			{
				if (distanceSqr>DBL_EPSILON)
				{
					const double distance = sqrt(distanceSqr);
					result.normal = contactToCentre;
					DsVec3d::Normalize(result.normal );
					result.point = contactPoint;
					result.depth = (radius - distance);
				}
				else
				{
					result.normal = normal;
					result.point = contactPoint;
					result.depth = radius;
				}
				return true;
			}
		}

		return false;
	}

}


DsCollisionResult& DsCollisionSphereMesh::Collide()
{
	m_info.Clear();
	if (_ColideAABB()){
		const DsActorId& o1 = m_pSphere->RefOwnerId();
		const DsActorId& o2 = m_pMesh->RefOwnerId();

		if (m_world.GetCollisionCallback()) {
			if (!m_world.GetCollisionCallback()->IsCollide(*o1.GetActor(), *o2.GetActor())) {
				return m_info;
			}
		}

		const double r = m_pSphere->GetSide().x;
		const DsVec3d sPos = m_pSphere->GetBasePos();
		const DsVec3d* pVertex = m_pMesh->GetVertex();
		const int faceNum = m_pMesh->GetFaceNum();
		const DsQuad* pFace = m_pMesh->GetFace();
		for (int fi = 0; fi < faceNum; ++fi, ++pFace){
			const DsVec3d vertex[3] =
			{
				pVertex[pFace->index[0]],
				pVertex[pFace->index[1]],
				pVertex[pFace->index[2]],
			};
			_ColideResult result;
			if (IsColide(m_pSphere, pFace->normal, vertex, pFace->vn, result)) {
				m_info.AddInfo(result.point, result.normal, result.depth, o1, o2);
			}
		}
	}

	return m_info;
}

bool DsCollisionSphereMesh::_ColideAABB()
{
	const DsBoundingTreeBase* pMTree = m_pMesh->GetBoungingTree();
	const DsAabb* pAabb = m_pSphere->GetAabb();

	if (pMTree && pAabb){
		return pMTree->IsContain(*pAabb);
	}
	else{
		return true;
	}
}