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
	//ode��

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

		DsVec3d cubeL;//cube�̒��S����sphere�̒��S�܂ł́Acube���Ɏ��܂�x�N�g��

		cubeL.x = DsVec3d::Dot(p, R[0]);
		if (cubeL.x < -cubeSize.x) { cubeL.x = -cubeSize.x; onborder = true; }//cube���Ɏ��܂�悤�ɁAcubeL���N�����v
		if (cubeL.x >  cubeSize.x) { cubeL.x =  cubeSize.x; onborder = true; }

		cubeL.y = DsVec3d::Dot(p, R[1]);
		if (cubeL.y < -cubeSize.y) { cubeL.y = -cubeSize.y; onborder = true; }
		if (cubeL.y >  cubeSize.y) { cubeL.y =  cubeSize.y; onborder = true; }

		cubeL.z = DsVec3d::Dot(p, R[2]);
		if (cubeL.z < -cubeSize.x) { cubeL.z = -cubeSize.z; onborder = true; }
		if (cubeL.z >  cubeSize.x) { cubeL.z =  cubeSize.z; onborder = true; }

		if (!onborder) {
			// ���̒��S�����̒��ɂ���B
			// ��Ԑ󂢕��������߂�
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
			result.normal = pCube->RefOwnerId().GetActor()->GetRotation()*tmp;//tmp��cube�̍��W�n�Ȃ̂ŃO���[�o���ɒ���
			// contact depth = distance to wall along normal plus radius
			result.depth = min_distance + pSphere->GetSide().x;
			return true;
		}

		//���S�_�����̊O
		//cubeL��cube�̍��W�n�Ȃ̂ŃO���[�o����
		const DsVec3d q = pCube->RefOwnerId().GetActor()->GetRotation()*cubeL;
		const DsVec3d r = p - q;//cube���̈�ԊO������A���̒��S�܂ł̃x�N�g���B���ꂪ�Őڋ߃x�N�g��
		result.depth = pSphere->GetSide().x - r.Length();
		//�Őڋ߃x�N�g���������a��蒷���̂ŏՓ˂Ȃ�
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