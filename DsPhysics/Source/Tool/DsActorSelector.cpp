#include "DsPhysicsPch.h"

#ifndef _DS_ACTOR_SELECTOR_
#include "Tool/DsActorSelector.h"
#endif

#include "DsPhysicsManager.h"
#include "DsPhysicsWorld.h"
#include "PhysicsUtil/DsPhysicsUtil.h"
#include "Mouse/DsMouse.h"
#ifndef _DS_WINDOW_H_
#include "System/DsWindow.h"
#endif

using namespace DsPhysics;

static DsActorId s_selectId;
static DsVec3d s_hitPosOffset;
static double s_selectDepth;

namespace
{
	DsVec3d _RayClickPos( const DsVec3d& clickPos, const DsCamera& cam, const double zLen )
	{
		//視錐台補正を打ち消して、クリックした箇所からそのまま奥に行くようにレイを飛ばしたい。
		//http://marupeke296.com/DXG_No70_perspective.html
		const double aspect = cam.GetAspect();
		const double fovy = cam.GetFovy();
		const double COT = 1.0/tan(DegToRad(fovy/2.0));
		DsVec3d ret = clickPos;
		ret.z = zLen;
		ret.x *= zLen*(aspect / COT);
		ret.y *= zLen*(1.0/COT);
		ret = (cam.GetRot()*ret) + ToVec3d(cam.GetPos());
		return ret;
	}
}
/*
	@param[out] depth 採用衝突点までの長さ
*/
DsActorId DsActorSelector::Select( const DsPhysicsWorld& world, double* depth/* = NULL*/ )
{
	DsActorId ret = s_selectId;
	const DsMouse& mouse = DsDbgSys::GetIns().RefMouse();
	const DsMouseClickState clickState = mouse.GetClickState();

	if (clickState == DsMouseClickState::DS_NON_CLICK)
	{
		s_selectId = DsActorId();
	}
	else if (clickState == DsMouseClickState::DS_LEFT_CLICK)
	{
		if( DsActorId() == s_selectId )
		{
			const DsWindow& win = DsDbgSys::GetIns().RefWindow();
			const DsVec3d clickPos = win.GetHalfNormalizeScreenCoord( mouse.GetPos2i() );
			const DsCamera& cam = DsDbgSys::GetIns().RefCam();

			const double length = cam.GetFar();
			const DsVec3d start = _RayClickPos(clickPos, cam, cam.GetNear());
			const DsVec3d end = _RayClickPos(clickPos, cam, cam.GetFar());
			double tmpDepth;
			DsActor* pActor = world.RayCast_CollectNear(start, end, &tmpDepth);
			if(pActor)
			{
				ret = pActor->GetId();
				s_selectId = pActor->GetId();
				s_hitPosOffset = start - pActor->GetPosition(); //当たった場所に位置合わせするため
				s_selectDepth = tmpDepth;
			}
			else
			{
				s_selectId = DsActorId();
				s_selectDepth = 0;
			}

			//デバッグ表示
			//DsDrawCommand& com = DsDbgSys::GetIns().RefDrawCom();
			//com.SetColor(DsVec3f::ToVec3(1, 0, 0));
			//com.DrawCapsule(start, end, 0.1);
			//if(ret.IsValid())printf("Hit!\n");
		}
	}
	if (depth) *depth = s_selectDepth;
	return ret;
}

void DsActorSelector::MoveActor( const DsPhysicsWorld& world )
{
	/*
	const DsCamera& cam = DsCameraManager::RenderCam();
	const DsVec3d targetPos = _RayClickPos(DsMouse::GetMousePos(), cam, cam.GetZNear());
	DsActor* pActor = world.GetActor(s_selectId);
	if(pActor)
	{
		DsDrawManager::GetIns().DrawSphere(targetPos, 0.02);
		DsDrawManager::GetIns().DrawSphere(pActor->GetPosition()+s_hitPosOffset, 0.02);
		DsVec3d addForce = DsPhysicsUtil::CalcForceTargetPos(targetPos, world, s_selectId, s_hitPosOffset );
		addForce = addForce.Flat(cam.GetLookDir()); //カメラ方向のxy平面成分の力だけ加算する
		pActor->AddForce(addForce);
	}
	*/
}