#include "DsPhysicsPch.h"

#ifndef _DS_COLLISION_SPHERE_CUBE_H_
#include "Collision/EachGeom/DsCollisionSphereCube.h"
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
	//odeの

	struct _ColideResult
	{
		DsVec3d pos;
		DsVec3d normal;
		double depth;
	};

	bool dCollideSphereBox(const DsCollisionContext* pSphere, const DsCollisionContext* pCube, _ColideResult& result )
	{
		bool onborder = false;

		const DsVec3d p = pSphere->GetBasePos() - pCube->GetBasePos();
		const DsVec3d& cubeSize = pCube->GetSide();

		const DsVec3d R[3]=
		{
			pCube->GetRot().GetAxisX(),
			pCube->GetRot().GetAxisY(),
			pCube->GetRot().GetAxisZ(),
		};

		DsVec3d cubeL;//cubeの中心からsphereの中心までの、cube内に収まるベクトル

		cubeL.x = DsVec3d::Dot(p, R[0]);
		if (cubeL.x < -cubeSize.x) { cubeL.x = -cubeSize.x; onborder = true; }//cube内に収まるように、cubeLをクランプ
		if (cubeL.x >  cubeSize.x) { cubeL.x =  cubeSize.x; onborder = true; }

		cubeL.y = DsVec3d::Dot(p, R[1]);
		if (cubeL.y < -cubeSize.y) { cubeL.y = -cubeSize.y; onborder = true; }
		if (cubeL.y >  cubeSize.y) { cubeL.y =  cubeSize.y; onborder = true; }

		cubeL.z = DsVec3d::Dot(p, R[2]);
		if (cubeL.z < -cubeSize.x) { cubeL.z = -cubeSize.z; onborder = true; }
		if (cubeL.z >  cubeSize.x) { cubeL.z =  cubeSize.z; onborder = true; }

		if (!onborder) {
			// 球の中心が箱の中にある。
			// 一番浅い方向を求める
			double min_distance = cubeSize.x - fabs(cubeL.x);
			int mini = 0;
			for (int i = 1; i<3; i++) {
				const double face_distance = cubeSize.v[i] - fabs(cubeL.v[i]);
				if (face_distance < min_distance) {
					min_distance = face_distance;
					mini = i;
				}
			}
			// contact position = sphere center
			result.pos = pSphere->GetBasePos();
			// contact normal points to closest face

			DsVec3d tmp = DsVec3d::Zero();
			tmp.v[mini] = (cubeL.v[mini] > 0.0) ? (1.0) : (-1.0);
			result.normal = pCube->RefOwnerId().GetActor()->GetRotation()*tmp;//tmpはcubeの座標系なのでグローバルに直す
			// contact depth = distance to wall along normal plus radius
			result.depth = min_distance + pSphere->GetSide().x;
			return true;
		}

		//中心点が球の外
		//cubeLはcubeの座標系なのでグローバルへ
		const DsVec3d q = pCube->RefOwnerId().GetActor()->GetRotation()*cubeL;
		const DsVec3d r = p - q;//cube側の一番外側から、球の中心までのベクトル。これが最接近ベクトル
		result.depth = pSphere->GetSide().x - r.Length();
		//最接近ベクトルが球半径より長いので衝突なし
		if (result.depth < 0.0) {
			return false;
		}

		result.pos = q + pCube->GetBasePos();
		result.normal = DsVec3d::Normalize(r);
		return 1;
	}
}


DsCollisionResult& DsCollisionSphereCube::Collide()
{
	m_info.Clear();
	if (_ColideAABB()){

		if (m_world.GetCollisionCallback()) {
			if (!m_world.GetCollisionCallback()->IsCollide(*m_pSphere->RefOwnerId().GetActor(), *m_pCube->RefOwnerId().GetActor())) {
				return m_info;
			}
		}

		_ColideResult result;
		if (dCollideSphereBox(m_pSphere, m_pCube, result)) {
			const DsActorId& o1 = m_pSphere->RefOwnerId();
			const DsActorId& o2 = m_pCube->RefOwnerId();
			m_info.AddInfo(result.pos, result.normal, result.depth, o1, o2);
		}
	}

	return m_info;
}

bool DsCollisionSphereCube::_ColideAABB()
{
	const DsBoundingTreeBase* pMTree = m_pCube->GetBoungingTree();
	const DsBoundingTreeBase* pBTree = m_pSphere->GetBoungingTree();

	if (pMTree && pBTree){
		return pBTree->IsContain(*pMTree);
	}
	else{
		return true;
	}
}