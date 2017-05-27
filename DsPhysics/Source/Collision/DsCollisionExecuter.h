#ifndef __DS_COLLISION_EXECUTER__
#define __DS_COLLISION_EXECUTER__

#ifndef __DS_COLLISION_CUBE_CUBE__
#include "Collision/EachGeom/DsCollisionCubeCube.h"
#endif
#ifndef _DS_COLLISION_CUBE_MESH_H_
#include "Collision/EachGeom/DsCollisionCubeMesh.h"
#endif
#ifndef __DS_COLLISION_CUBE_RAY__
#include "Collision/EachGeom/DsCollisionCubeRay.h"
#endif
#ifndef __DS_COLLISION_MESH_RAY__
#include "Collision/EachGeom/DsCollisionMeshRay.h"
#endif
#ifndef _DS_COLLISION_SPHERE_MESH_H_
#include "Collision/EachGeom/DsCollisionSphereMesh.h"
#endif
#ifndef _DS_COLLISION_SPHERE_CUBE_H_
#include "Collision/EachGeom/DsCollisionSphereCube.h"
#endif

#ifndef __DS_ACTOR__
#include "Actor/DsActor.h"
#endif
#ifndef __DS_COLLISION_CONTEXT__
#include "Collision/DsCollisionContext.h"
#endif
#ifndef __DS_COLLISION_RESULT__
#include "Collision/DsCollisionResult.h"
#endif
#ifndef _DS_COLLISION_SPHERE_SPHERE_H_
#include "Collision/EachGeom/DsCollisionSphereSphere.h"
#endif
#ifndef _DS_COLLISION_CAPSULE_MESH_H_
#include "Collision/EachGeom/DsCollisionCapsuleMesh.h"
#endif


namespace DsPhysics
{
	class DsActor;
	class DsCollisionResult;
}

namespace DsPhysics
{
	class DsCollisionExecuter
	{
	private:
		static DsCollisionResult m_noCollide;

	public:
		DsCollisionExecuter()
			: m_cubeCube()
			, m_rayCube()
			, m_cubeMesh()
			, m_meshRay()
			, m_sphereMesh()
			, m_sphereCube()
			, m_sphereSphere()
			, m_capsuleMesh()
		{}
		virtual ~DsCollisionExecuter()
		{}

		const DsCollisionResult& Exe(const DsActor& actor1, const DsActor& actor2)
		{
			//キューブとキューブ
			if ((actor1.GetType() == DsActor::RIGID_CUBE) && (actor2.GetType() == DsActor::RIGID_CUBE))
			{
				m_cubeCube.Initialize(actor1.GetCollContext(), actor2.GetCollContext());
				return m_cubeCube.Colide();
			}

			//キューブとレイ
			else if ((actor1.GetType() == DsActor::RIGID_CUBE) && (actor2.GetType() == DsActor::RAY))
			{
				m_rayCube.Initialize(actor1.GetCollContext(), actor2.GetCollContext());
				return m_rayCube.Colide();
			}
			else if ((actor2.GetType() == DsActor::RIGID_CUBE) && (actor1.GetType() == DsActor::RAY))
			{
				m_rayCube.Initialize(actor1.GetCollContext(), actor2.GetCollContext());
				return m_rayCube.Colide();
			}

			//キューブとメッシュ
			else if ((actor1.GetType() == DsActor::RIGID_CUBE) && (actor2.GetType() == DsActor::RIGID_MESH))
			{
				m_cubeMesh.Initialize(actor1.GetCollContext(), actor2.GetCollContext());
				return m_cubeMesh.Colide();
			}
			else if ((actor2.GetType() == DsActor::RIGID_CUBE) && (actor1.GetType() == DsActor::RIGID_MESH))
			{
				m_cubeMesh.Initialize(actor2.GetCollContext(), actor1.GetCollContext());
				return m_cubeMesh.Colide();
			}

			//メッシュとレイ
			else if ((actor1.GetType() == DsActor::RIGID_MESH) && (actor2.GetType() == DsActor::RAY))
			{
				m_meshRay.Initialize(actor1.GetCollContext(), actor2.GetCollContext());
				return m_meshRay.Colide();
			}
			else if ((actor2.GetType() == DsActor::RIGID_CUBE) && (actor1.GetType() == DsActor::RAY))
			{
				m_meshRay.Initialize(actor2.GetCollContext(), actor1.GetCollContext());
				return m_meshRay.Colide();
			}

			//スフィアとスフィア
			else if ((actor1.GetType() == DsActor::RIGID_SPHERE) && (actor2.GetType() == DsActor::RIGID_SPHERE)){
				m_sphereSphere.Initialize(actor1.GetCollContext(), actor2.GetCollContext());
				return m_sphereSphere.Colide();
			}

			//スフィアとメッシュ
			else if ((actor1.GetType() == DsActor::RIGID_SPHERE) && (actor2.GetType() == DsActor::RIGID_MESH))
			{
				m_sphereMesh.Initialize(actor1.GetCollContext(), actor2.GetCollContext());
				return m_sphereMesh.Colide();
			}
			else if ((actor1.GetType() == DsActor::RIGID_MESH) && (actor2.GetType() == DsActor::RIGID_SPHERE))
			{
				m_sphereMesh.Initialize(actor2.GetCollContext(), actor1.GetCollContext());
				return m_sphereMesh.Colide();
			}

			//スフィアとキューブ
			else if ((actor1.GetType() == DsActor::RIGID_SPHERE) && (actor2.GetType() == DsActor::RIGID_CUBE))
			{
				m_sphereCube.Initialize(actor1.GetCollContext(), actor2.GetCollContext());
				return m_sphereCube.Colide();
			}
			else if ((actor1.GetType() == DsActor::RIGID_CUBE) && (actor2.GetType() == DsActor::RIGID_SPHERE))
			{
				m_sphereCube.Initialize(actor2.GetCollContext(), actor1.GetCollContext());
				return m_sphereCube.Colide();
			}


			//カプセルとメッシュ
			else if ((actor1.GetType() == DsActor::RIGID_CAPSULE) && (actor2.GetType() == DsActor::RIGID_MESH))
			{
				m_capsuleMesh.Initialize(actor1.GetCollContext(), actor2.GetCollContext());
				return m_capsuleMesh.Colide();
			}
			else if ((actor1.GetType() == DsActor::RIGID_MESH) && (actor2.GetType() == DsActor::RIGID_CAPSULE))
			{
				m_capsuleMesh.Initialize(actor2.GetCollContext(), actor1.GetCollContext());
				return m_capsuleMesh.Colide();
			}


			return m_noCollide;
		}

	private:
		//一時クラスとして使ってたがインスタンス生成に時間かかってたので、メンバで持っとく
		DsCollisionCubeCube m_cubeCube;
		DsCollisionCubeRay m_rayCube;
		DsCollisionCubeMesh m_cubeMesh;
		DsCollisionMeshRay m_meshRay;
		DsCollisionSphereMesh m_sphereMesh;
		DsCollisionSphereCube m_sphereCube;
		DsCollisionSphereSphere m_sphereSphere;
		DsCollisionCapsuleMesh m_capsuleMesh;
	};
}

#endif