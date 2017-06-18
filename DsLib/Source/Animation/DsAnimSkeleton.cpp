#include "DsPch.h"
#ifndef _DS_AMIM_SKELETON_
#include "Animation/DsAnimSkeleton.h"
#endif

#ifndef _DS_DRAW_COMMAND_H_
#include "Graphics/Command/DsDrawCommand.h"
#endif

using namespace DsLib;

DsAnimSkeleton::DsAnimSkeleton(const std::vector<DsAnimBone*>& pRootBone, const std::vector<DsAnimBone*>& boneArray)
:m_pRootBone(pRootBone)
,m_boneArray(boneArray)
,m_rootPos(DsVec3d::Zero())
,m_rootRot(DsMat33d::Identity())
{

}

DsAnimSkeleton::~DsAnimSkeleton()
{
	for each(DsAnimBone* pBone in m_pRootBone)
	{
		delete pBone;
	}
}


namespace
{
	void _UpdateWorldPose(DsAnimBone* bone, const DsMat33d& bRot, const DsVec3d& bPos)
	{
		bone->worldPose = DsMat44d::Get(bRot, bPos);
		for each(DsAnimBone* child in bone->child)
		{
			//位置は親の座標から見た位置っぽい
			DsVec3d bonePos = bRot*(child->localPose.GetPos());
			bonePos = bonePos + bPos;

			//座標は親からの累積
			const DsMat33d&& boneRot = bRot*child->localPose.ToMat33();
			_UpdateWorldPose(child, boneRot, bonePos);
		}
	}
}

void DsAnimSkeleton::UpdateWorldPose()
{
	const std::vector<DsAnimBone*>& roots = RefRootBone();
	for each( DsAnimBone* root in roots)
	{
		// FBX変換かかってなさげなのでここで掛ける(そのうち出力ツール側でやりたい)
		//const DsMat33d fbxR = DsMat33d::RotateX(-M_PI_2);
		//intWorldはFbx変換かかってる。localPoseはかかってない。Fbx変換成分だけを抜き出す。ただし、localPoseの姿勢がキーフレームで変わってるとダメ
		const DsMat33d fbxR = root->initWorldPose.ToMat33()*root->localPose.ToMat33().ToTransposition();
		const DsMat33d m = GetRootRot()*fbxR*root->localPose.ToMat33();	//マスターの姿勢は変えられなかったから考慮しなくてOK
		//localPoseはエクスポート時のFbx座標変換がかかってない
		//localPoseを打ち消したら考慮した座標と一致した。とりあえずこれで行く
		const DsVec3d offset = (fbxR*root->localPose.GetPos());
		const DsVec3d p = offset + GetRootPos() + root->initWorldPose.GetPos();
		_UpdateWorldPose(root, m, p);
	}
}


namespace
{
	void _DbgDraw(const DsAnimBone* pParent, const DsMat33d& rot, const DsVec3d& pos)
	{	
		DsDrawCommand& draw = DsDbgSys::GetIns().RefDrawCom();
		draw.DrawSphere(pos, 0.1);
		draw.DrawLine(pos, pos + rot.GetAxisY());
		
		for each(const DsAnimBone* pBone in pParent->child)
		{
			//位置は親の座標から見た位置っぽい
			DsVec3d local = pBone->localPose.GetPos();
			DsVec3d p = rot*local;
			p = p + pos;

			//座標は親からの累積
			DsMat33d r = rot*pBone->localPose.ToMat33();
			_DbgDraw(pBone, r, p);
		}
	}
}

void DsAnimSkeleton::DbgDraw() const
{
	for each(const DsAnimBone* pBone in m_pRootBone)
	{
		//_DbgDraw(pBone, pBone->localPose.ToMat33(), pBone->localPose.GetPos());
		//_DbgDraw(pBone, pBone->initWorldPose.ToMat33(), pBone->initWorldPose.GetPos());

		DsMat44d pose = pBone->initWorldPose * pBone->localPose;
		DsMat33d r = pBone->initWorldPose.ToMat33();//マスターの姿勢は変えられなかったから考慮しなくてOK
		//loaclPoseはエクスポート時のFbx座標変換がかかってない
		//localPoseを打ち消したら考慮した座標と一致した。とりあえずこれで行く
		DsVec3d  p = pBone->initWorldPose.GetPos();// +(pBone->localPose.ToMat33().ToTransposition()*pBone->localPose.GetPos());
		_DbgDraw(pBone, r, p);
	}
}