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
			//�ʒu�͐e�̍��W���猩���ʒu���ۂ�
			DsVec3d bonePos = bRot*(child->localPose.GetPos());
			bonePos = bonePos + bPos;

			//���W�͐e����̗ݐ�
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
		// FBX�ϊ��������ĂȂ����Ȃ̂ł����Ŋ|����(���̂����o�̓c�[�����ł�肽��)
		//const DsMat33d fbxR = DsMat33d::RotateX(-M_PI_2);
		//intWorld��Fbx�ϊ��������Ă�BlocalPose�͂������ĂȂ��BFbx�ϊ����������𔲂��o���B�������AlocalPose�̎p�����L�[�t���[���ŕς���Ă�ƃ_��
		const DsMat33d fbxR = root->initWorldPose.ToMat33()*root->localPose.ToMat33().ToTransposition();
		const DsMat33d m = GetRootRot()*fbxR*root->localPose.ToMat33();	//�}�X�^�[�̎p���͕ς����Ȃ���������l�����Ȃ���OK
		//localPose�̓G�N�X�|�[�g����Fbx���W�ϊ����������ĂȂ�
		//localPose��ł���������l���������W�ƈ�v�����B�Ƃ肠��������ōs��
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
			//�ʒu�͐e�̍��W���猩���ʒu���ۂ�
			DsVec3d local = pBone->localPose.GetPos();
			DsVec3d p = rot*local;
			p = p + pos;

			//���W�͐e����̗ݐ�
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
		DsMat33d r = pBone->initWorldPose.ToMat33();//�}�X�^�[�̎p���͕ς����Ȃ���������l�����Ȃ���OK
		//loaclPose�̓G�N�X�|�[�g����Fbx���W�ϊ����������ĂȂ�
		//localPose��ł���������l���������W�ƈ�v�����B�Ƃ肠��������ōs��
		DsVec3d  p = pBone->initWorldPose.GetPos();// +(pBone->localPose.ToMat33().ToTransposition()*pBone->localPose.GetPos());
		_DbgDraw(pBone, r, p);
	}
}