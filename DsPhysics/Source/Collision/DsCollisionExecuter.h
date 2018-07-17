#ifndef __DS_COLLISION_EXECUTER__
#define __DS_COLLISION_EXECUTER__
#ifndef __DS_ACTOR__
#include "Actor/DsActor.h"
#endif
#ifndef __DS_COLLISION_GEOMETRY__
#include "Collision/DsCollisionGeometry.h"
#endif
#ifndef __DS_COLLISION_RESULT__
#include "Collision/DsCollisionResult.h"
#endif
#ifndef __DS_COLLISION_BOX_BOX__
#include "Collision/EachGeom/DsCollisionBoxBox.h"
#endif
#ifndef _DS_COLLISION_BOX_MESH_H_
#include "Collision/EachGeom/DsCollisionBoxMesh.h"
#endif
#ifndef __DS_COLLISION_BOX_RAY__
#include "Collision/EachGeom/DsCollisionBoxRay.h"
#endif
#ifndef __DS_COLLISION_MESH_RAY__
#include "Collision/EachGeom/DsCollisionMeshRay.h"
#endif
#ifndef _DS_COLLISION_SPHERE_MESH_H_
#include "Collision/EachGeom/DsCollisionSphereMesh.h"
#endif
#ifndef _DS_COLLISION_SPHERE_BOX_H_
#include "Collision/EachGeom/DsCollisionSphereBox.h"
#endif
#ifndef _DS_COLLISION_MESH_MESH_H_
#include "Collision/EachGeom/DsCollisionMeshMesh.h"
#endif
#ifndef _DS_COLLISION_SPHERE_SPHERE_H_
#include "Collision/EachGeom/DsCollisionSphereSphere.h"
#endif
#ifndef _DS_COLLISION_CAPSULE_MESH_H_
#include "Collision/EachGeom/DsCollisionCapsuleMesh.h"
#endif
#ifndef _DS_COLLISION_BOX_CAPSULE_
#include "Collision/EachGeom/DsCollisionBoxCapsule.h"
#endif

namespace DsPhysics
{
	class DsActor;
	class DsCollisionResult;
	class DsPhysicsWorld;
}

namespace DsPhysics
{
	class DsCollisionExecuter
	{
	private:
		static DsCollisionResult m_noCollide;

	public:
		DsCollisionExecuter(const DsPhysicsWorld& world)
			: m_boxBox(world)
			, m_rayBox(world)
			, m_boxMesh(world)
			, m_meshRay(world)
			, m_sphereMesh(world)
			, m_sphereBox(world)
			, m_sphereSphere(world)
			, m_capsuleMesh(world)
			, m_meshMesh(world)
			, m_boxCapsule(world)
		{}
		virtual ~DsCollisionExecuter()
		{}

		const DsCollisionResult& Exe(const DsActor& actor1, const DsActor& actor2)
		{
			//ここは１フレでかなり高頻度で呼ばれる場所

			//キューブとキューブ
			if ((actor1.GetType() == DsActor::RIGID_BOX) && (actor2.GetType() == DsActor::RIGID_BOX))
			{
				m_boxBox.Initialize(actor1.GetCollisionGeometry(), actor2.GetCollisionGeometry());
				return m_boxBox.Collide();
			}

			//キューブとレイ
			else if ((actor1.GetType() == DsActor::RIGID_BOX) && (actor2.GetType() == DsActor::RAY))
			{
				m_rayBox.Initialize(actor1.GetCollisionGeometry(), actor2.GetCollisionGeometry());
				return m_rayBox.Collide();
			}
			else if ((actor2.GetType() == DsActor::RIGID_BOX) && (actor1.GetType() == DsActor::RAY))
			{
				m_rayBox.Initialize(actor1.GetCollisionGeometry(), actor2.GetCollisionGeometry());
				return m_rayBox.Collide();
			}

			//キューブとメッシュ
			else if ((actor1.GetType() == DsActor::RIGID_BOX) && (actor2.GetType() == DsActor::RIGID_MESH))
			{
				m_boxMesh.Initialize(actor1.GetCollisionGeometry(), actor2.GetCollisionGeometry());
				return m_boxMesh.Collide();
			}
			else if ((actor2.GetType() == DsActor::RIGID_BOX) && (actor1.GetType() == DsActor::RIGID_MESH))
			{
				m_boxMesh.Initialize(actor2.GetCollisionGeometry(), actor1.GetCollisionGeometry());
				return m_boxMesh.Collide();
			}

			//キューブとカプセル
			else if ((actor1.GetType() == DsActor::RIGID_BOX) && (actor2.GetType() == DsActor::RIGID_CAPSULE))
			{
				m_boxCapsule.Initialize(actor1.GetCollisionGeometry(), actor2.GetCollisionGeometry());
				return m_boxCapsule.Collide();
			}
			else if ((actor2.GetType() == DsActor::RIGID_BOX) && (actor1.GetType() == DsActor::RIGID_CAPSULE))
			{
				m_boxCapsule.Initialize(actor2.GetCollisionGeometry(), actor1.GetCollisionGeometry());
				return m_boxCapsule.Collide();
			}

			//メッシュとレイ
			else if ((actor1.GetType() == DsActor::RIGID_MESH) && (actor2.GetType() == DsActor::RAY))
			{
				m_meshRay.Initialize(actor1.GetCollisionGeometry(), actor2.GetCollisionGeometry());
				return m_meshRay.Collide();
			}
			else if ((actor2.GetType() == DsActor::RIGID_BOX) && (actor1.GetType() == DsActor::RAY))
			{
				m_meshRay.Initialize(actor2.GetCollisionGeometry(), actor1.GetCollisionGeometry());
				return m_meshRay.Collide();
			}

			//メッシュとメッシュ
			else if ((actor1.GetType() == DsActor::RIGID_MESH) && (actor2.GetType() == DsActor::RIGID_MESH))
			{
				m_meshMesh.Initialize(actor1.GetCollisionGeometry(), actor2.GetCollisionGeometry());
				return m_meshMesh.Collide();
			}

			//スフィアとスフィア
			else if ((actor1.GetType() == DsActor::RIGID_SPHERE) && (actor2.GetType() == DsActor::RIGID_SPHERE)){
				m_sphereSphere.Initialize(actor1.GetCollisionGeometry(), actor2.GetCollisionGeometry());
				return m_sphereSphere.Collide();
			}

			//スフィアとメッシュ
			else if ((actor1.GetType() == DsActor::RIGID_SPHERE) && (actor2.GetType() == DsActor::RIGID_MESH))
			{
				m_sphereMesh.Initialize(actor1.GetCollisionGeometry(), actor2.GetCollisionGeometry());
				return m_sphereMesh.Collide();
			}
			else if ((actor1.GetType() == DsActor::RIGID_MESH) && (actor2.GetType() == DsActor::RIGID_SPHERE))
			{
				m_sphereMesh.Initialize(actor2.GetCollisionGeometry(), actor1.GetCollisionGeometry());
				return m_sphereMesh.Collide();
			}

			//スフィアとキューブ
			else if ((actor1.GetType() == DsActor::RIGID_SPHERE) && (actor2.GetType() == DsActor::RIGID_BOX))
			{
				m_sphereBox.Initialize(actor1.GetCollisionGeometry(), actor2.GetCollisionGeometry());
				return m_sphereBox.Collide();
			}
			else if ((actor1.GetType() == DsActor::RIGID_BOX) && (actor2.GetType() == DsActor::RIGID_SPHERE))
			{
				m_sphereBox.Initialize(actor2.GetCollisionGeometry(), actor1.GetCollisionGeometry());
				return m_sphereBox.Collide();
			}


			//カプセルとメッシュ
			else if ((actor1.GetType() == DsActor::RIGID_CAPSULE) && (actor2.GetType() == DsActor::RIGID_MESH))
			{
				m_capsuleMesh.Initialize(actor1.GetCollisionGeometry(), actor2.GetCollisionGeometry());
				return m_capsuleMesh.Collide();
			}
			else if ((actor1.GetType() == DsActor::RIGID_MESH) && (actor2.GetType() == DsActor::RIGID_CAPSULE))
			{
				m_capsuleMesh.Initialize(actor2.GetCollisionGeometry(), actor1.GetCollisionGeometry());
				return m_capsuleMesh.Collide();
			}


			return m_noCollide;
		}

	private:
		//todo:呼び出す処理は予め分かってるので登録しておいて呼ぶだけにする。ifの判定回数分お得
		DsCollisionBoxBox m_boxBox;
		DsCollisionBoxRay m_rayBox;
		DsCollisionBoxMesh m_boxMesh;
		DsCollisionMeshRay m_meshRay;
		DsCollisionSphereMesh m_sphereMesh;
		DsCollisionSphereBox m_sphereBox;
		DsCollisionSphereSphere m_sphereSphere;
		DsCollisionCapsuleMesh m_capsuleMesh;
		DsCollisionMeshMesh m_meshMesh;
		DsCollisionBoxCapsule m_boxCapsule;
	};
}

#endif