#include "DsPhysicsPch.h"

#ifndef _DS_COLLISION_SPHERE_BOX_H_
#include "Collision/EachGeom/DsCollisionSphereBox.h"
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
	//ode��

	struct _ColideResult
	{
		DsVec3d pos;
		DsVec3d normal;
		double depth;
	};

	bool dCollideSphereBox(const DsCollisionGeometry* pSphere, const DsCollisionGeometry* pBox, _ColideResult& result )
	{
		bool onborder = false;

		const DsVec3d p = pSphere->GetBasePos() - pBox->GetBasePos();
		const DsVec3d& boxSize = pBox->GetSide();

		const DsVec3d R[3]=
		{
			pBox->GetRot().GetAxisX(),
			pBox->GetRot().GetAxisY(),
			pBox->GetRot().GetAxisZ(),
		};

		DsVec3d boxL;//box�̒��S����sphere�̒��S�܂ł́Abox���Ɏ��܂�x�N�g��

		boxL.x = DsVec3d::Dot(p, R[0]);
		if (boxL.x < -boxSize.x) { boxL.x = -boxSize.x; onborder = true; }//box���Ɏ��܂�悤�ɁAboxL���N�����v
		if (boxL.x >  boxSize.x) { boxL.x =  boxSize.x; onborder = true; }

		boxL.y = DsVec3d::Dot(p, R[1]);
		if (boxL.y < -boxSize.y) { boxL.y = -boxSize.y; onborder = true; }
		if (boxL.y >  boxSize.y) { boxL.y =  boxSize.y; onborder = true; }

		boxL.z = DsVec3d::Dot(p, R[2]);
		if (boxL.z < -boxSize.x) { boxL.z = -boxSize.z; onborder = true; }
		if (boxL.z >  boxSize.x) { boxL.z =  boxSize.z; onborder = true; }

		if (!onborder) {
			// ���̒��S�����̒��ɂ���B
			// ��Ԑ󂢕��������߂�
			double min_distance = boxSize.x - fabs(boxL.x);
			int mini = 0;
			for (int i = 1; i<3; i++) {
				const double face_distance = boxSize.v[i] - fabs(boxL.v[i]);
				if (face_distance < min_distance) {
					min_distance = face_distance;
					mini = i;
				}
			}
			// contact position = sphere center
			result.pos = pSphere->GetBasePos();
			// contact normal points to closest face

			DsVec3d tmp = DsVec3d::Zero();
			tmp.v[mini] = (boxL.v[mini] > 0.0) ? (1.0) : (-1.0);
			result.normal = pBox->RefOwnerId().GetActor()->GetRotation()*tmp;//tmp��box�̍��W�n�Ȃ̂ŃO���[�o���ɒ���
			// contact depth = distance to wall along normal plus radius
			result.depth = min_distance + pSphere->GetSide().x;
			return true;
		}

		//���S�_�����̊O
		//boxL��box�̍��W�n�Ȃ̂ŃO���[�o����
		const DsVec3d q = pBox->RefOwnerId().GetActor()->GetRotation()*boxL;
		const DsVec3d r = p - q;//box���̈�ԊO������A���̒��S�܂ł̃x�N�g���B���ꂪ�Őڋ߃x�N�g��
		result.depth = pSphere->GetSide().x - r.Length();
		//�Őڋ߃x�N�g���������a��蒷���̂ŏՓ˂Ȃ�
		if (result.depth < 0.0) {
			return false;
		}

		result.pos = q + pBox->GetBasePos();
		result.normal = DsVec3d::Normalize(r);
		return 1;
	}
}


DsCollisionResult& DsCollisionSphereBox::Collide()
{
	m_info.Clear();
	if (_ColideAABB()){

		if (m_world.GetCollisionCallback()) {
			if (!m_world.GetCollisionCallback()->IsCollide(*m_pSphere->RefOwnerId().GetActor(), *m_pBox->RefOwnerId().GetActor())) {
				return m_info;
			}
		}

		_ColideResult result;
		if (dCollideSphereBox(m_pSphere, m_pBox, result)) {
			const DsActorId& o1 = m_pSphere->RefOwnerId();
			const DsActorId& o2 = m_pBox->RefOwnerId();
			m_info.AddInfo(result.pos, result.normal, result.depth, o1, o2);
		}
	}

	return m_info;
}

bool DsCollisionSphereBox::_ColideAABB()
{
	const DsBoundingTreeBase* pMTree = m_pBox->GetBoungingTree();
	const DsBoundingTreeBase* pBTree = m_pSphere->GetBoungingTree();

	if (pMTree && pBTree){
		return pBTree->IsContain(*pMTree);
	}
	else{
		return true;
	}
}