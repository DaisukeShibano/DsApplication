#include "DsPch.h"
#ifndef _DS_ANIMATION_H_
#include "Animation/DsAnimation.h"
#endif

#ifndef _DS_AMIM_SKELETON_
#include "Animation/DsAnimSkeleton.h"
#endif
#ifndef _DS_AMIM_RES_
#include "Res/DsAnimRes.h"
#endif
#ifndef _DS_KEYFRAME_ANIM_SET_
#include "Animation/DsKeyframeAnimSet.h"
#endif
#ifndef _DS_SKINMESH_
#include "Animation/SkinMesh/DsSkinMesh.h"
#endif
#ifndef _DS_ANIM_MODEL_
#include "Animation/DsAnimModel.h"
#endif
#ifndef _DS_DRAW_COMMAND_H_
#include "Graphics/Command/DsDrawCommand.h"
#endif
#ifndef _DS_ANIM_CLIP_H_
#include "Animation/DsAnimClip.h"
#endif
#ifndef _DS_AMIM_RAGDOLL_INFO_
#include "Animation/DsAnimCustomProperty.h"
#endif
#ifndef _DS_ANIM_SKELETON_MODIFIER_
#include "Animation/DsAnimSkeletonModifier.h"
#endif



using namespace DsLib;

DsAnimation::DsAnimation(const DsAnimController& animController, const DsAnimRes& anim, DsDrawCommand& com)
	: m_animSM(animController)
	, m_pSkeleton(NULL)
	, m_pKeyframeAnim(NULL)
	, m_pAnimModel(NULL)
	, m_pSkinMesh(NULL)
	, m_pCustomProperty(NULL)
	, m_animModifier(NULL)
	, m_blend()
	, m_pos(DsVec3d::Zero())
	, m_rot(DsMat33d::Identity())
	, m_com(com)
{
	m_pSkeleton = anim.CreateSkeleton();
	m_pKeyframeAnim = anim.CreateKeyframeAnim();
	m_pAnimModel = anim.CreateAnimModel();
	m_pCustomProperty = anim.CustomProperty();

	//m_animSMは仮のアニメ遷移制御
	if (m_pKeyframeAnim && m_pSkeleton && m_pAnimModel)
	{
		m_animSM.Initialize(m_pKeyframeAnim->GetKeyframeAnims(), m_pKeyframeAnim->GetKeyframeAnimNum());

		//キーフレームアニメないならスキンメッシュモデルは無駄。
		m_pSkinMesh = new DsSkinMesh(*m_pAnimModel);
		DS_ASSERT(m_pSkinMesh, "メモリ確保失敗");
		m_pSkinMesh->Initialize();
	}

	if (const DsAnimClip* pClip = m_animSM.GetDefaultClip())
	{
		m_blend.Initialize(pClip->RefAnim());
	}

	if (m_pSkeleton) {
		m_pSkeleton->UpdatePose();
	}

	//ボーンの結果を頂点に適用。
	if (m_pSkinMesh && m_pSkeleton)
	{
		m_pSkinMesh->ApplySkeleton(*m_pSkeleton);
	}

	//モデルの座標を更新
	DsAnimModel* pModel = _GetAnimModel();
	if (pModel) {
		pModel->SetPosition(m_pos);
		pModel->SetRotation(m_rot);
	}
}

DsAnimation::~DsAnimation()
{
	m_com.RefAnimRender().UnRegister(_GetAnimModel());
	delete m_pSkeleton; m_pSkeleton = NULL;
	delete m_pKeyframeAnim; m_pKeyframeAnim = NULL;
	delete m_pAnimModel; m_pAnimModel = NULL;
	delete m_pSkinMesh; m_pSkinMesh = NULL;
	delete m_pCustomProperty; m_pCustomProperty = NULL;
	
}

void DsAnimation::RegisterDraw()
{
	//描画に登録
	DsAnimModel* pModel = _GetAnimModel();
	if (pModel) {
		if (!pModel->IsCreateVertexNormal())
		{
			pModel->CreateVertexNormal();//頂点法線計算のための配列確保
			pModel->UpdateNormal();
		}
		m_com.RefAnimRender().Register(pModel);
	}
}

void DsAnimation::Update(double dt)
{
	//再生アニメを決定
	m_animSM.Update(dt);
	
	//ブレンド結果をアニメボーンに適用。
	if (m_pSkeleton)
	{
		if (m_pKeyframeAnim) {
			double blendRate = 1.0;
			const DsAnimClip* pPre = m_animSM.GetPreActiveClip();
			if (pPre)
			{
				//ブレンド率は、前のアニメから今のアニメに徐々にウェイトが行くようにする
				blendRate = 1.0 - pPre->GetBlendRate();
			}
			const DsKeyframeAnim& blend = m_blend.Blend(m_animSM.GetActiveClip(), pPre, blendRate);

			if (m_animModifier) {
				m_animModifier->ModifyAnim(dt, *m_pSkeleton, blend, m_pos, m_rot);
			}
			else {
				DsAnimSkeletonModifier::UtilKeyframeAnim(dt, *m_pSkeleton, blend);
				
			}
		}
		else {
			//キーフレームアニメがないのは初期姿勢のまま
			m_pSkeleton->UpdatePose();
		}
	}

	//ボーンの結果を頂点に適用。
	if (m_pSkinMesh && m_pSkeleton)
	{
		m_pSkinMesh->ApplySkeleton(*m_pSkeleton);
	}

	//モデルの座標を更新
	DsAnimModel* pModel = _GetAnimModel();
	if (pModel) {
		pModel->SetPosition(m_pos);
		pModel->SetRotation(m_rot);
	}
}

void DsAnimation::SetRootMatrix(const DsVec3d& p, const DsMat33d& r)
{
	if (m_pSkeleton) {//削除予定
		m_pSkeleton->SetRootPos(p);
		m_pSkeleton->SetRootRot(r);
	}
	m_pos = p;
	m_rot = r;
}

const DsVec3d& DsAnimation::GetPosition() const
{
	return m_pos;
}

const DsMat33d& DsAnimation::GetRotation() const
{
	return m_rot;
}

DsAnimModel* DsAnimation::_GetAnimModel()
{
	return (m_pSkinMesh && m_pSkinMesh->GetSkinMeshModel()) ? (m_pSkinMesh->GetSkinMeshModel()) : (m_pAnimModel);
}
