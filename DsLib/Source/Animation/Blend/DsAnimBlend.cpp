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

void DsAnimBlend::_BlendPoseModel(const DsAnimBone* bone,
								const DsKeyframeAnim& animA, const DsKeyframeAnim& animB, double blendRate)
{
	const int idx = bone->arrayIdx;

	//アニメAのモデル座標
	const DsMat33d localRotA = DsMathUtil::ToMat33d<double>(DsQuaterniond(animA.RefCurrentRot(idx)));
	const DsVec3d localPosA = animA.RefCurrentPos(idx);
	const DsMat33d modelRotA = localRotA;
	const DsVec3d modelPosA = localPosA;

	//アニメBのモデル座標
	const DsMat33d localRotB = DsMathUtil::ToMat33d<double>(DsQuaterniond(animB.RefCurrentRot(idx)));
	const DsVec3d localPosB = animB.RefCurrentPos(idx);
	const DsMat33d modelRotB = localRotB;
	const DsVec3d modelPosB = localPosB;

	//ブレンド
	const double rateA = blendRate;
	const double rateB = (1.0 - blendRate);
	DsMat33d blendRot;
	DsVec3d blendPos;

	{//位置のブレンド
		blendPos = (modelPosA*rateA) + (modelPosB*rateB);//m_pBlendAnim->m_pBone[bi].m_pPosは１フレ分しか確保していなく、その都度AとBから求める
		const DsVec3d localPos = blendPos;
		m_pBlendAnim->m_pBone[idx].m_pPos[0].val = localPos;//ローカル座標へ変換
	}
	{//回転のブレンド
		const DsQuaterniond qA(modelRotA);
		const DsQuaterniond qB(modelRotB);
		const DsQuaterniond q = qA.LinearInterpolation(qB, rateB);
		//const DsQuaterniond q = DsQuaterniond::LinearInterpolation(qB, qA, rateA);
		blendRot = DsMathUtil::ToMat33d<double>(q);
		const DsQuaterniond localQ = DsQuaterniond(blendRot);//ローカル座標へ変換
		m_pBlendAnim->m_pBone[idx].m_pRot[0].val = DsVec4d::ToVec4(localQ.x, localQ.y, localQ.z, localQ.w);
	}
	{//スケールのブレンド
		const DsVec3d& sA = animA.RefCurrentScale(idx);
		const DsVec3d& sB = animB.RefCurrentScale(idx);
		m_pBlendAnim->m_pBone[idx].m_pScale[0].val = (sA*rateA) + (sB*rateB);
	}

	for (const DsAnimBone* pChild : bone->child) {
		_BlendPoseModel(pChild, animA, animB, modelRotA, modelPosA, modelRotB, modelPosB, blendRot, blendPos, blendRate);
	}

}


void DsAnimBlend::_BlendPoseModel(const DsAnimBone* bone,
								const DsKeyframeAnim& animA, const DsKeyframeAnim& animB,
								const DsMat33d& parentARot, const DsVec3d& parentAPos,
								const DsMat33d& parentBRot, const DsVec3d& parentBPos,
								const DsMat33d& parentBlendRot, const DsVec3d& parentBlendPos,
								double blendRate)
{
	//↓最初から最後までクォータニオンにする

	for (const DsAnimBone* pChild : bone->child){
		const int idx = pChild->arrayIdx;
			
		//アニメAのモデル座標
		const DsMat33d localRotA = DsMathUtil::ToMat33d<double>(DsQuaterniond(animA.RefCurrentRot(idx)));
		const DsVec3d localPosA = animA.RefCurrentPos(idx);
		const DsMat33d modelRotA = parentARot * localRotA;//回転はブレンドがかかっていないもの同士を補間した姿勢にすることで親がブレンドされたことによる余計な回転を防ぐ
		const DsVec3d modelPosA = (parentBlendRot * localPosA) + parentBlendPos;//位置は実際に骨の位置になるブレンドされたPos,Rotを使わないと長さが一定に保てない
	
		//アニメBのモデル座標
		const DsMat33d localRotB = DsMathUtil::ToMat33d<double>(DsQuaterniond(animB.RefCurrentRot(idx)));
		const DsVec3d localPosB = animB.RefCurrentPos(idx);
		const DsMat33d modelRotB = parentBRot * localRotB;
		const DsVec3d modelPosB = (parentBlendRot * localPosB) + parentBlendPos;

		//ブレンド
		const double rateA = blendRate;
		const double rateB = (1.0 - blendRate);
		DsMat33d blendRot;
		DsVec3d blendPos;
		
		{//位置のブレンド
			blendPos = (modelPosA*rateA) + (modelPosB*rateB);//m_pBlendAnim->m_pBone[bi].m_pPosは１フレ分しか確保していなく、その都度AとBから求める
			const DsVec3d localPos = parentBlendRot.ToTransposition() * (blendPos - parentBlendPos);
			m_pBlendAnim->m_pBone[idx].m_pPos[0].val = localPos;//ローカル座標へ変換
		}
		{//回転のブレンド
			const DsQuaterniond qA(modelRotA);
			const DsQuaterniond qB(modelRotB);
			const DsQuaterniond q = qA.LinearInterpolation(qB, rateB);
			//const DsQuaterniond q = DsQuaterniond::LinearInterpolation(qB, qA, rateA);
			blendRot = DsMathUtil::ToMat33d<double>(q);
			const DsQuaterniond localQ = DsQuaterniond(parentBlendRot.ToTransposition()*blendRot);//ローカル座標へ変換
			m_pBlendAnim->m_pBone[idx].m_pRot[0].val = DsVec4d::ToVec4(localQ.x, localQ.y, localQ.z, localQ.w);
		}
		{//スケールのブレンド
			const DsVec3d& sA = animA.RefCurrentScale(idx);
			const DsVec3d& sB = animB.RefCurrentScale(idx);
			m_pBlendAnim->m_pBone[idx].m_pScale[0].val = (sA*rateA) + (sB*rateB);
		}

		//ブレンド後のグローバル座標を子供へ
		_BlendPoseModel(pChild, animA, animB, modelRotA, modelPosA, modelRotB, modelPosB, blendRot, blendPos, blendRate);
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

		{//位置のブレンド
			const DsVec3d& posA = poseA.RefCurrentPos();
			const DsVec3d& posB = poseB.RefCurrentPos();
			//m_pBlendAnim->m_pBone[bi].m_pPos[0].localTime = toLocalTime; //使ってない
			m_pBlendAnim->m_pBone[bi].m_pPos[0].val = (posA*rateA) + (posB*rateB);//m_pBlendAnim->m_pBone[bi].m_pPosは１フレ分しか確保していなく、その都度AとBから求める
		}
		{//回転のブレンド
			const DsVec4d& rotA = poseA.RefCurrentRot();
			const DsVec4d& rotB = poseB.RefCurrentRot();
			const DsQuaterniond qA(rotA.x, rotA.y, rotA.z, rotA.w);
			const DsQuaterniond qB(rotB.x, rotB.y, rotB.z, rotB.w);
			const DsQuaterniond q = qA.LinearInterpolation(qB, rateB);//ローカル同士の補間だとポーズがかけ離れてしまうかも。
			//const DsQuaterniond q = DsQuaterniond::LinearInterpolation(qB, qA, rateA);//ローカル同士の補間だとポーズがかけ離れてしまうかも。
			m_pBlendAnim->m_pBone[bi].m_pRot[0].val = DsVec4d::ToVec4(q.x, q.y, q.z, q.w);
		}
		{//スケールのブレンド
			const DsVec3d& sA = poseA.RefCurrentScale();
			const DsVec3d& sB = poseB.RefCurrentScale();
			m_pBlendAnim->m_pBone[bi].m_pScale[0].val = (sA*rateA) + (sB*rateB);
		}
	}
}


//blendRate = 1 で A。 blendRate = 0 で B
const DsKeyframeAnim& DsAnimBlend::Blend(const DsAnimSkeleton& skeleton, const DsAnimClip* pClipA, const DsAnimClip* pClipB, double blendRate)
{
	if (pClipA && pClipB)
	{
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

		//ローカル座標でブレンド
		_BlendPoseLocal(animA, animB, blendRate);
		
		//モデル座標でブレンド
		//for (const DsAnimBone* pBone : skeleton.RefRootBone()) {
		//	_BlendPoseModel(pBone, animA, animB, blendRate);
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
		printf("ブレンドアニメA、B共に存在しません\n");
		return *m_pBlendAnim;
	}
}