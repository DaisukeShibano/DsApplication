#include "DsPhysicsPch.h"

#ifndef _DS_COLLISION_SPHERE_SPHERE_H_
#include "Collision/EachGeom/DsCollisionSphereSphere.h"
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



DsCollisionResult& DsCollisionSphereSphere::Collide()
{
	m_info.Clear();
	//if (_ColideAABB())//�������Ȃ��ق�������
	{
		if (m_world.GetCollisionCallback()) {
			if (!m_world.GetCollisionCallback()->IsCollide(*m_pSphere1->RefOwnerId().GetActor(), *m_pSphere2->RefOwnerId().GetActor())) {
				return m_info;
			}
		}

		const DsVec3d dist = m_pSphere1->GetBasePos() - m_pSphere2->GetBasePos();
		const double lenSq = dist.LengthSq();
		const double r1 = m_pSphere1->GetSide().x;
		const double r2 = m_pSphere2->GetSide().x;
		const double rSq = pow(r1 + r2, 2.0);

		if (lenSq <= 0.0) {
			//�҂�����d�Ȃ��Ă��܂����Ƃ�
			//�K���ȕ����ɕԂ�
			//���x�Ƃ��l�����������������������ǁA���������_�ł҂�����d�Ȃ�Ȃ�ď����z�u�Ƃ����[�v�����Ƃ����炢���Ǝv���̂ŁA���x�͎Q�l�ɂȂ�Ȃ��B�ʓ|�Ȃ̂œK��
			const DsVec3d normal(1.0, 0.0, 0.0);
			const double depth = r1;
			m_info.AddInfo(m_pSphere1->GetBasePos(), normal, depth, m_pSphere1->RefOwnerId(), m_pSphere2->RefOwnerId());
		}else if (lenSq < rSq) {
			//
			const double len = sqrt(lenSq);
			const DsVec3d normal = dist / len;
			const double depth = (r1 + r2 - len);//�d�Ȃ����������N�����ꂽ����
			const DsVec3d pos = m_pSphere1->GetBasePos() + (normal*depth);//�N�����ꂽ�Ƃ��܂ł��Փ˓_
			m_info.AddInfo(pos, normal, depth, m_pSphere1->RefOwnerId(), m_pSphere2->RefOwnerId());
		}
	}

	return m_info;
}

bool DsCollisionSphereSphere::_ColideAABB()
{
	const DsBoundingTreeBase* pMTree = m_pSphere1->GetBoungingTree();
	const DsBoundingTreeBase* pBTree = m_pSphere2->GetBoungingTree();

	if (pMTree && pBTree){
		return pBTree->IsContain(*pMTree);
	}
	else{
		return true;
	}
}