#include "DsPhysicsPch.h"
#ifndef _DS_POSITION_DEPTH_SOLVER_
#include "Constraint/ConstraintSolver/DsPositionDepthSolver.h"
#endif
//その他のヘッダ
#ifndef __DS_ACTOR__
#include "Actor/DsActor.h"
#endif
#ifndef __DS_PHYSICS_WORLD__
#include "DsPhysicsWorld.h"
#endif
#ifndef __DS_COLLISION_RESULT__
#include "Collision/DsCollisionResult.h"
#endif

using namespace DsPhysics;

DsPositionDepthSolver::DsPositionDepthSolver(const DsPhysicsWorld& world)
	:m_world(world)
{
}

DsPositionDepthSolver::~DsPositionDepthSolver()
{
}


void DsPositionDepthSolver::AddDepth(const DsCollisionResult& c)
{
	const int colNum = c.GetColNum();
	for (int i = 0; i < colNum; ++i) {

		const DsVec3d depth = c.GefNormal()[i]*c.GefDepth()[i];
		//方向はowner1がめり込み解消する方向なので、めり込み量としては反対側になる

		//合計ベクトルにしようかと思ったが、同じ方向に複数めり込んでいた場合過剰に長くなってしまうので一番量が多いのを採用する
		//成分ごとにやった方がいい？よく分からん方向になるが、問題ない？
		{
			DsActor* pOwner1 = m_world.GetActor(c.GefOwnerId1()[i]);
			const DsVec3d add = -depth*0.5;
			const DsVec3d cur = pOwner1->GetCollisionDepth();
			//if (cur.LengthSq() < add.LengthSq()) {
			//	pOwner1->SetCollisionDepth(add);//めり込み量を追加
			//}
			DsVec3d set = cur;
			if ( fabs(cur.x) < fabs(add.x)) {
				set.x = add.x;
			}
			if (fabs(cur.y) < fabs(add.y)) {
				set.y = add.y;
			}
			if (fabs(cur.z) < fabs(add.z)) {
				set.z = add.z;
			}
			pOwner1->SetCollisionDepth(set);
		}
		{
			DsActor* pOwner2 = m_world.GetActor(c.GefOwnerId2()[i]);
			const DsVec3d add = depth*0.5;
			const DsVec3d cur = pOwner2->GetCollisionDepth();
			//if (cur.LengthSq() < add.LengthSq()) {
			//	pOwner2->SetCollisionDepth(add);//めり込み量を追加
			//}
			DsVec3d set = cur;
			if (fabs(cur.x) < fabs(add.x)) {
				set.x = add.x;
			}
			if (fabs(cur.y) < fabs(add.y)) {
				set.y = add.y;
			}
			if (fabs(cur.z) < fabs(add.z)) {
				set.z = add.z;
			}
			pOwner2->SetCollisionDepth(set);
		}

		//相手の速度のことも考えると、めり込み方向に進んでも問題ない場合もある
		//最終的な速度を出し、移動量の予測を出し、それでもめり込んでしまうなら、その成分を移動禁止にするとか？

		//それでも落下で追い越すときに急停止しちゃう気がするが、1フレでめり込み解決できる速度を拘束計算を出せば、
		//めり込み解決できない速度になるのは上下に挟まれているようなどうしても解決できない場合なので、急停止しても問題ない？
	}
}