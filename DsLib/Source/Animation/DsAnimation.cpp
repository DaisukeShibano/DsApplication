#include "DsPch.h"
#ifndef _DS_ANIMATION_H_
#include "Animation/DsAnimation.h"
#endif

#ifndef _DS_AMIM_SKELTON_
#include "Animation/DsAnimSkelton.h"
#endif
#ifndef _DS_AMIM_RES_
#include "Res/DsAnimRes.h"
#endif
#ifndef _DS_KEYFRAME_ANIM_CTRL_
#include "Animation/DsKeyFrameAnimCtrl.h"
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


using namespace DsLib;

DsAnimation::DsAnimation(const DsAnimController& animController, const DsAnimRes& anim, DsDrawCommand& com)
	: m_animSM(animController)
	, m_pSkelton(NULL)
	, m_pKeyFrameAnim(NULL)
	, m_pAnimModel(NULL)
	, m_pSkinMesh(NULL)
	, m_blend()
	, m_pos(DsVec3d::Zero())
	, m_rot(DsMat33d::Identity())
	, m_com(com)
{
	m_pSkelton = anim.CreateSkelton();
	m_pKeyFrameAnim = anim.CreateKeyFrameAnim();
	m_pAnimModel = anim.CreateAnimModel();

	//�A�j���Đ��N���X���A�j���J�ڐ�����Ăяo���񂶂�Ȃ��A�A�j���J�ڐ��䂪�A�j���Đ��N���X���Ăяo���`�ɂ������B
	if (m_pKeyFrameAnim && m_pSkelton && m_pAnimModel)
	{
		m_animSM.Initialize(m_pKeyFrameAnim->GetKeyFrameAnims(), m_pKeyFrameAnim->GetKeyFrameAnimNum());

		//�L�[�t���[���A�j���Ȃ��Ȃ�X�L�����b�V�����f���͖��ʁB
		m_pSkinMesh = new DsSkinMesh(*m_pAnimModel);
		m_pSkinMesh->Initialize();
	}

	if (const DsAnimClip* pClip = m_animSM.GetDefaultClip())
	{
		m_blend.Initialize(pClip->RefAnim());
	}
}

DsAnimation::~DsAnimation()
{
	m_com.RefAnimRender().UnRegister(_GetAnimModel());
	delete m_pSkelton; m_pSkelton = NULL;
	delete m_pKeyFrameAnim; m_pKeyFrameAnim = NULL;
	delete m_pAnimModel; m_pAnimModel = NULL;
	delete m_pSkinMesh; m_pSkinMesh = NULL;
	
}

void DsAnimation::RegisterDraw()
{
	//�`��ɓo�^
	DsAnimModel* pModel = _GetAnimModel();
	if (pModel) {
		if (!pModel->IsCreateVertexNormal())
		{
			pModel->CreateVertexNormal();//���_�@���v�Z�̂��߂̔z��m��
			pModel->UpdateNormal();
		}
		m_com.RefAnimRender().Register(pModel);
	}
}

void DsAnimation::Update(double dt)
{
	//�Đ��A�j��������
	m_animSM.Update(dt);
	
	//�u�����h���ʂ��A�j���{�[���ɓK�p�B
	if (m_pKeyFrameAnim && m_pSkelton)
	{
		double blendRate = 1.0;
		const DsAnimClip* pPre = m_animSM.GetPreActiveClip();
		if (pPre)
		{
			//�u�����h���́A�O�̃A�j�����獡�̃A�j���ɏ��X�ɃE�F�C�g���s���悤�ɂ���
			blendRate = 1.0 - pPre->GetBlendRate();
		}
		const DsKeyFrameAnim& blend = m_blend.Blend(m_animSM.GetActiveClip(), pPre, blendRate);
		m_pKeyFrameAnim->ApplyAnim(dt, *m_pSkelton, blend);
	}

	//�{�[���̌��ʂ𒸓_�ɓK�p�B
	if (m_pSkinMesh && m_pSkelton)
	{
		m_pSkinMesh->ApplySkelton(*m_pSkelton);
	}
}

void DsAnimation::SetRootMatrix(const DsVec3d& p, const DsMat33d& r)
{
	if (m_pSkelton) {
		m_pSkelton->SetRootPos(p);
		m_pSkelton->SetRootRot(r);
	}
	else {
		//�X�P���g���������ĂȂ������ꍇ
		m_pos = p;
		m_rot = r;
	}
}

const DsVec3d& DsAnimation::GetPosition() const
{
	return (m_pSkelton) ? (m_pSkelton->GetRootPos()) : (m_pos);
}

const DsMat33d& DsAnimation::GetRotation() const
{
	return (m_pSkelton) ? (m_pSkelton->GetRootRot()) : (m_rot);
}

DsAnimModel* DsAnimation::_GetAnimModel()
{
	return (m_pSkinMesh && m_pSkinMesh->GetSkinMeshModel()) ? (m_pSkinMesh->GetSkinMeshModel()) : (m_pAnimModel);
}
