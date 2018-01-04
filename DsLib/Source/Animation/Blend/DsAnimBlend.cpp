#include "DsPch.h"
#ifndef _DS_ANIM_BLEND_H_
#include "Animation/Blend/DsAnimBlend.h"
#endif
#ifndef _DS_ANIM_CLIP_H_
#include "Animation/DsAnimClip.h"
#endif


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
	DS_ASSERT(m_pBlendAnim, "メモリ確保失敗");
	m_pBlendAnim->m_name = "blend";
	m_pBlendAnim->m_boneNum = src.m_boneNum;
	m_pBlendAnim->m_pBone = new DsKeyframeAnim::Pose[src.m_boneNum];
	DS_ASSERT(m_pBlendAnim->m_pBone, "メモリ確保失敗");
	for (int bi = 0; bi < src.m_boneNum; ++bi)
	{
		m_pBlendAnim->m_pBone[bi].m_pPos = new DsKeyframeAnim::Vec3Key();
		DS_ASSERT(m_pBlendAnim->m_pBone[bi].m_pPos, "メモリ確保失敗");
		*(m_pBlendAnim->m_pBone[bi].m_pPos) = src.m_pBone[bi].m_pPos[0];
		m_pBlendAnim->m_pBone[bi].m_currentIdxPos = 0;

		m_pBlendAnim->m_pBone[bi].m_pRot = new DsKeyframeAnim::Vec4Key();
		DS_ASSERT(m_pBlendAnim->m_pBone[bi].m_pRot, "メモリ確保失敗");
		*(m_pBlendAnim->m_pBone[bi].m_pRot) = src.m_pBone[bi].m_pRot[0];
		m_pBlendAnim->m_pBone[bi].m_currentIdxRot = 0;

		m_pBlendAnim->m_pBone[bi].m_pScale = new DsKeyframeAnim::Vec3Key();
		DS_ASSERT(m_pBlendAnim->m_pBone[bi].m_pScale, "メモリ確保失敗");
		*(m_pBlendAnim->m_pBone[bi].m_pScale) = src.m_pBone[bi].m_pScale[0];
		m_pBlendAnim->m_pBone[bi].m_currentIdxScale = 0;
		
	}

	
}

//blendRate = 1 で A。 blendRate = 0 で B
const DsKeyframeAnim& DsAnimBlend::Blend(const DsAnimClip* pClipA, const DsAnimClip* pClipB, double blendRate)
{
	if (pClipA && pClipB)
	{
		const double rateA = blendRate;
		const double rateB = (1.0 - blendRate);

		const DsKeyframeAnim& animA = pClipA->RefAnim();
		const DsKeyframeAnim& animB = pClipB->RefAnim();
		
		//無駄な計算しないようにブレンド率が0or1なら何もせず返す
		if (0.999999 <= blendRate)
		{
			return animA;
		}
		else if (0.000001 >= blendRate)
		{
			return animB;
		}

		const int bn = animA.GetBoneNum();
		for (int bi = 0; bi < bn; ++bi)
		{
			const DsKeyframeAnim::Pose& poseA = animA.m_pBone[bi];
			const DsKeyframeAnim::Pose& poseB = animB.m_pBone[bi];

			const double toLocalTime = poseA.m_pPos[poseA.m_currentIdxPos].localTime;
			const int toBoneIdx = poseA.m_currentIdxPos;

			{
				const DsVec3d& posA = poseA.RefCurrentPos();
				const DsVec3d& posB = poseB.RefCurrentPos();
				//m_pBlendAnim->m_pBone[bi].m_pPos[0].localTime = toLocalTime; //使ってない
				m_pBlendAnim->m_pBone[bi].m_pPos[0].val = (posA*rateA) + (posB*rateB);//m_pBlendAnim->m_pBone[bi].m_pPosは１フレ分しか確保していなく、その都度AとBから求める
			}
			{
				const DsVec4d& rotA = poseA.RefCurrentRot();
				const DsVec4d& rotB = poseB.RefCurrentRot();
				const DsQuaterniond qA(rotA.x, rotA.y, rotA.z, rotA.w);
				const DsQuaterniond qB(rotB.x, rotB.y, rotB.z, rotB.w);
				const DsQuaterniond q = qA.LinearInterpolation(qB, rateB);//ローカル同士の補間だとポーズがかけ離れてしまうかも。
				m_pBlendAnim->m_pBone[bi].m_pRot[0].val = DsVec4d::ToVec4(q.x, q.y, q.z, q.w);
			}
			{
				const DsVec3d& sA = poseA.RefCurrentScale();
				const DsVec3d& sB = poseB.RefCurrentScale();
				m_pBlendAnim->m_pBone[bi].m_pScale[0].val = (sA*rateA) + (sB*rateB);
			}
			const DsVec3d& sclA = animA.RefCurrentScale(bi);
		}


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
		printf("ブレンドアニメA、B共に存在しません\n");
		return *m_pBlendAnim;
	}
}