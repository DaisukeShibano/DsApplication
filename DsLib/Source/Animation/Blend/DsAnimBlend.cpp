#include "DsPch.h"
#ifndef _DS_ANIM_BLEND_H_
#include "Animation/Blend/DsAnimBlend.h"
#endif
#ifndef _DS_ANIM_CLIP_H_
#include "Animation/DsAnimClip.h"
#endif
#include "Animation/DsAnimSkeleton.h"


using namespace DsLib;


DsAnimBlend::DsAnimBlend()
	:m_pBlendAnim(NULL)
{
}

DsAnimBlend::~DsAnimBlend()
{
	delete m_pBlendAnim;
	m_pBlendAnim = NULL;
}

void DsAnimBlend::Initialize(const DsKeyframeAnim& src)
{
	m_pBlendAnim = new DsKeyframeAnim();
	DS_ASSERT(m_pBlendAnim, "�������m�ێ��s");
	m_pBlendAnim->m_name = "blend";
	m_pBlendAnim->m_boneNum = src.m_boneNum;
	m_pBlendAnim->m_pBone = new DsKeyframeAnim::Pose[src.m_boneNum];
	DS_ASSERT(m_pBlendAnim->m_pBone, "�������m�ێ��s");
	for (int bi = 0; bi < src.m_boneNum; ++bi)
	{
		m_pBlendAnim->m_pBone[bi].m_pPos = new DsKeyframeAnim::Vec3Key();
		DS_ASSERT(m_pBlendAnim->m_pBone[bi].m_pPos, "�������m�ێ��s");
		*(m_pBlendAnim->m_pBone[bi].m_pPos) = src.m_pBone[bi].m_pPos[0];
		m_pBlendAnim->m_pBone[bi].m_currentIdxPos = 0;

		m_pBlendAnim->m_pBone[bi].m_pRot = new DsKeyframeAnim::Vec4Key();
		DS_ASSERT(m_pBlendAnim->m_pBone[bi].m_pRot, "�������m�ێ��s");
		*(m_pBlendAnim->m_pBone[bi].m_pRot) = src.m_pBone[bi].m_pRot[0];
		m_pBlendAnim->m_pBone[bi].m_currentIdxRot = 0;

		m_pBlendAnim->m_pBone[bi].m_pScale = new DsKeyframeAnim::Vec3Key();
		DS_ASSERT(m_pBlendAnim->m_pBone[bi].m_pScale, "�������m�ێ��s");
		*(m_pBlendAnim->m_pBone[bi].m_pScale) = src.m_pBone[bi].m_pScale[0];
		m_pBlendAnim->m_pBone[bi].m_currentIdxScale = 0;
		
	}

	
}

void DsAnimBlend::_BlendPoseModel(const DsAnimBone* bone, const DsKeyframeAnim& animA, const DsKeyframeAnim& animB, const DsMat33d& parentRot, const DsVec3d& parentPos, double blendRate)
{
	//���ŏ�����Ō�܂ŃN�H�[�^�j�I���ɂ���

	for (const DsAnimBone* pChild : bone->child){
		const int idx = pChild->arrayIdx;
			
		//�A�j��A�̃O���[�o�����W
		const DsMat33d localRotA = DsMathUtil::ToMat33d<double>(DsQuaterniond(animA.RefCurrentRot(idx)));
		const DsVec3d localPosA = animA.RefCurrentPos(idx);
		const DsMat33d modelRotA = parentRot * localRotA;
		const DsVec3d modelPosA = (parentRot * localPosA) + parentPos;
	
		//�A�j��B�̃O���[�o�����W
		const DsMat33d localRotB = DsMathUtil::ToMat33d<double>(DsQuaterniond(animB.RefCurrentRot(idx)));
		const DsVec3d localPosB = animB.RefCurrentPos(idx);
		const DsMat33d modelRotB = parentRot * localRotB;
		const DsVec3d modelPosB = (parentRot * localPosB) + parentPos;


		//�u�����h
		const double rateA = blendRate;
		const double rateB = (1.0 - blendRate);
		DsMat33d blendRot;
		DsVec3d blendPos;
		
		{//�ʒu�̃u�����h
			blendPos = (modelPosA*rateA) + (modelPosB*rateB);//m_pBlendAnim->m_pBone[bi].m_pPos�͂P�t���������m�ۂ��Ă��Ȃ��A���̓s�xA��B���狁�߂�
			const DsVec3d localPos = parentRot.ToTransposition() * (blendPos - parentPos);
			m_pBlendAnim->m_pBone[idx].m_pPos[0].val = localPos;//���[�J�����W�֕ϊ�
		}
		{//��]�̃u�����h
			const DsQuaterniond qA(modelRotA);
			const DsQuaterniond qB(modelRotB);
			const DsQuaterniond q = qA.LinearInterpolation(qB, rateB);
			blendRot = DsMathUtil::ToMat33d<double>(q);
			const DsQuaterniond localQ = DsQuaterniond(parentRot.ToTransposition()*blendRot);//���[�J�����W�֕ϊ�
			m_pBlendAnim->m_pBone[idx].m_pRot[0].val = DsVec4d::ToVec4(localQ.x, localQ.y, localQ.z, localQ.w);
		}
		{//�X�P�[���̃u�����h
			const DsVec3d& sA = animA.RefCurrentScale(idx);
			const DsVec3d& sB = animB.RefCurrentScale(idx);
			m_pBlendAnim->m_pBone[idx].m_pScale[0].val = (sA*rateA) + (sB*rateB);
		}

		//�u�����h��̃O���[�o�����W���q����
		_BlendPoseModel(pChild, animA, animB, blendRot, blendPos, blendRate);
	}	
}

void DsAnimBlend::_BlendPoseLocal(const DsKeyframeAnim& animA, const DsKeyframeAnim& animB, double blendRate)
{
	const double rateA = blendRate;
	const double rateB = (1.0 - rateA);

	const int bn = animA.GetBoneNum();
	for (int bi = 0; bi < bn; ++bi)
	{
		const DsKeyframeAnim::Pose& poseA = animA.m_pBone[bi];
		const DsKeyframeAnim::Pose& poseB = animB.m_pBone[bi];

		const double toLocalTime = poseA.m_pPos[poseA.m_currentIdxPos].localTime;

		{//�ʒu�̃u�����h
			const DsVec3d& posA = poseA.RefCurrentPos();
			const DsVec3d& posB = poseB.RefCurrentPos();
			//m_pBlendAnim->m_pBone[bi].m_pPos[0].localTime = toLocalTime; //�g���ĂȂ�
			m_pBlendAnim->m_pBone[bi].m_pPos[0].val = (posA*rateA) + (posB*rateB);//m_pBlendAnim->m_pBone[bi].m_pPos�͂P�t���������m�ۂ��Ă��Ȃ��A���̓s�xA��B���狁�߂�
		}
		{//��]�̃u�����h
			const DsVec4d& rotA = poseA.RefCurrentRot();
			const DsVec4d& rotB = poseB.RefCurrentRot();
			const DsQuaterniond qA(rotA.x, rotA.y, rotA.z, rotA.w);
			const DsQuaterniond qB(rotB.x, rotB.y, rotB.z, rotB.w);
			const DsQuaterniond q = qA.LinearInterpolation(qB, rateB);//���[�J�����m�̕�Ԃ��ƃ|�[�Y����������Ă��܂������B
			//const DsQuaterniond q = DsQuaterniond::LinearInterpolation(qB, qA, rateA);//���[�J�����m�̕�Ԃ��ƃ|�[�Y����������Ă��܂������B
			m_pBlendAnim->m_pBone[bi].m_pRot[0].val = DsVec4d::ToVec4(q.x, q.y, q.z, q.w);
		}
		{//�X�P�[���̃u�����h
			const DsVec3d& sA = poseA.RefCurrentScale();
			const DsVec3d& sB = poseB.RefCurrentScale();
			m_pBlendAnim->m_pBone[bi].m_pScale[0].val = (sA*rateA) + (sB*rateB);
		}
	}
}


//blendRate = 1 �� A�B blendRate = 0 �� B
const DsKeyframeAnim& DsAnimBlend::Blend(const DsAnimSkeleton& skeleton, const DsAnimClip* pClipA, const DsAnimClip* pClipB, double blendRate)
{
	if (pClipA && pClipB)
	{
		const DsKeyframeAnim& animA = pClipA->RefAnim();
		const DsKeyframeAnim& animB = pClipB->RefAnim();
		
		//���ʂȌv�Z���Ȃ��悤�Ƀu�����h����0or1�Ȃ牽�������Ԃ�
		if (0.999999 <= blendRate)
		{
			return animA;
		}
		else if (0.000001 >= blendRate)
		{
			return animB;
		}

		//���[�J�����W�Ńu�����h
		_BlendPoseLocal(animA, animB, blendRate);
		
		//���f�����W���s���R������
		//for (const DsAnimBone* pBone : skeleton.RefRootBone()) {
		//	_BlendPoseModel(pBone, animA, animB, DsMat33d::Identity(), DsVec3d::Zero(), blendRate);
		//}

		return *m_pBlendAnim;
	}
	else if (pClipA)
	{
		return pClipA->RefAnim();
	}
	else if (pClipB)
	{
		return pClipB->RefAnim();
	}
	else
	{
		printf("�u�����h�A�j��A�AB���ɑ��݂��܂���\n");
		return *m_pBlendAnim;
	}
}