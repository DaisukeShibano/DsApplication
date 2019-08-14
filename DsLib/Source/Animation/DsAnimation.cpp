#include "DsPch.h"
#ifndef _DS_ANIMATION_H_
#include "Animation/DsAnimation.h"
#endif
//���̃w�b�_
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
#ifndef _DS_MODEL_
#include "Model/DsModel.h"
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
#ifndef _DS_RESOURCE_
#include "Res/DsResource.h"
#endif


using namespace DsLib;

DsAnimation::DsAnimation(const char* pResName, DsDrawCommand& com, DsResource& resource)
	: m_resource(resource)
	, m_pAnimRes(NULL)
	, m_pSkeleton(NULL)
	, m_pKeyframeAnim(NULL)
	, m_pAnimModel(NULL)
	, m_pSkinMesh(NULL)
	, m_pCustomProperty(NULL)
	, m_animModifier(NULL)
	, m_blend()
	, m_pos(DsVec3d::Zero())
	, m_rot(DsMat33d::Identity())
	, m_deltaMotion(DsVec3d::Zero())
	, m_animClips()
	, m_pRequestAnim(NULL)
	, m_pPlayAnim(NULL)
	, m_pPlayAnimPre(NULL)
	, m_com(com)
{
	m_pAnimRes =m_resource.RegisterItem<DsAnimRes>(pResName);
	if (m_pAnimRes) {

		m_pSkeleton = m_pAnimRes->CreateSkeleton();
		m_pKeyframeAnim = m_pAnimRes->CreateKeyframeAnim();
		m_pAnimModel = m_pAnimRes->CreateAnimModel();
		m_pCustomProperty = m_pAnimRes->CustomProperty();

		if (m_pKeyframeAnim && m_pSkeleton && m_pAnimModel)
		{
			const int animNum = m_pKeyframeAnim->GetKeyframeAnimNum();
			DsKeyframeAnim* pAnims = m_pKeyframeAnim->GetKeyframeAnims();
			m_animClips.reserve(animNum);
			for (int idx = 0; idx < animNum; ++idx) {
				DsAnimClip* pClip = new DsAnimClip(pAnims[idx]);
				DS_ASSERT(pClip, "�������m�ێ��s");
				m_animClips.push_back(pClip);
			}


			//�L�[�t���[���A�j���Ȃ��Ȃ�X�L�����b�V�����f���͖��ʁB
			m_pSkinMesh = new DsSkinMesh(*m_pAnimModel);
			DS_ASSERT(m_pSkinMesh, "�������m�ێ��s");
			m_pSkinMesh->Initialize();
		}

		//�����̓A�j������Ȃ��ăX�P���g���ŏ�����������
		if (!m_animClips.empty()) {
			m_blend.Initialize(m_animClips[0]->RefAnim());
		}

		if (m_pSkeleton) {
			m_pSkeleton->UpdatePose();
		}

		//�{�[���̌��ʂ𒸓_�ɓK�p�B
		if (m_pSkinMesh && m_pSkeleton)
		{
			m_pSkinMesh->ApplySkeleton(*m_pSkeleton);
		}
		else {
			//�����Ȃ��̂Ŗ��ʂ�������������Ȃ�
			DsModel* pModel = GetModel();
			if (pModel) {
				pModel->UpdateNormal();
			}
		}

		//���f���̍��W���X�V
		DsModel* pModel = GetModel();
		if (pModel) {
			pModel->SetPosition(m_pos);
			pModel->SetRotation(m_rot);
		}
	}
}

DsAnimation::~DsAnimation()
{
	m_pAnimRes = m_resource.UnregisterItem<DsAnimRes>(m_pAnimRes);
	m_com.RefModelRender().UnRegister(GetModel());
	delete m_pSkeleton; m_pSkeleton = NULL;
	delete m_pKeyframeAnim; m_pKeyframeAnim = NULL;
	delete m_pAnimModel; m_pAnimModel = NULL;
	delete m_pSkinMesh; m_pSkinMesh = NULL;
	delete m_pCustomProperty; m_pCustomProperty = NULL;
	
	for (DsAnimClip* pClip : m_animClips) {
		delete pClip;
	}
	m_animClips.clear();

}

void DsAnimation::RegisterDraw()
{
	//�`��ɓo�^
	DsModel* pModel = GetModel();
	if (pModel) {
		if (!pModel->IsCreateVertexNormal())
		{
			pModel->CreateVertexNormal();//���_�@���v�Z�̂��߂̔z��m��
			pModel->UpdateNormal();
		}
		m_com.RefModelRender().Register(pModel);
	}
}

void DsAnimation::Update(double dt)
{
	//�A�j���X�V
	bool isChangeAnim = false;
	if (m_pRequestAnim != m_pPlayAnim) {
		m_pPlayAnimPre = m_pPlayAnim;
		m_pPlayAnim = m_pRequestAnim;
		isChangeAnim = true;
	}

	for (DsAnimClip* pAnim : m_animClips) {
		if ((pAnim != m_pPlayAnim) && (pAnim != m_pPlayAnimPre)) {
			pAnim->Deactivate(dt);
			pAnim->ResetAnim();
		}
	}
	if (isChangeAnim) {
		if (m_pPlayAnim) {
			m_pPlayAnim->Activate(dt);
		}
		if (m_pPlayAnimPre) {
			m_pPlayAnimPre->Deactivate(dt);
		}
	}
	if (m_pPlayAnim) {
		m_pPlayAnim->Update(dt);
	}
	//��O�̃A�j���͕�Ԃ̂��߂��΂炭�X�V��������
	if (m_pPlayAnimPre) {
		m_pPlayAnimPre->Update(dt);
		if (m_pPlayAnimPre->IsEndBlend()) {
			m_pPlayAnimPre->ResetAnim();
			m_pPlayAnimPre = NULL;
		}
	}

	//�A�j���u�����h
	double blendRate = 1.0;
	if (m_pPlayAnimPre) {
		//�u�����h���́A�O�̃A�j�����獡�̃A�j���ɏ��X�ɃE�F�C�g���s���悤�ɂ���
		blendRate = 1.0 - m_pPlayAnimPre->GetBlendRate();
	}
	
	//�u�����h���ʂ��A�j���{�[���ɓK�p�B
	if (m_pSkeleton) {
		m_blend.m_dbgPos = m_pos;
		m_blend.m_dbgRot = m_rot;
		const DsKeyframeAnim& blend = m_blend.Blend(*m_pSkeleton, m_pPlayAnim, m_pPlayAnimPre, blendRate);

		if (m_animModifier) {
			m_animModifier->ModifyAnim(dt, blend, m_pos, m_rot, *m_pSkeleton);
		}
		else {
			DsAnimSkeletonModifier::UtilKeyframeAnim(dt, blend, *m_pSkeleton);
		}

		//�ړ��ʍX�V
		//Mater�ʒu�܂ŕ�Ԃ����Ƒ啪�ւ�ȓ�������B����blend��Master�ړ��ʊm�ۂ��ĂȂ�
		if (m_pPlayAnim) {
			DsAnimSkeletonModifier::UtilGetMasterMove(dt, m_pPlayAnim->RefAnim(), *m_pSkeleton, &m_deltaMotion, NULL);
		}
	}

	//�{�[���̌��ʂ𒸓_�ɓK�p�B
	if (m_pSkinMesh && m_pSkeleton)
	{
		m_pSkinMesh->ApplySkeleton(*m_pSkeleton);
	}
	else {
		//�����Ȃ��̂Ŗ��ʂ�������������Ȃ�
		DsModel* pModel = GetModel();
		if (pModel) {
			pModel->UpdateNormal();
		}
	}

	//���f���̍��W���X�V
	DsModel* pModel = GetModel();
	if (pModel) {
		pModel->SetPosition(m_pos);
		pModel->SetRotation(m_rot);
	}
}

void DsAnimation::SetRootMatrix(const DsVec3d& p, const DsMat33d& r)
{
	if (m_pSkeleton) {//�폜�\��
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

const DsModel* DsAnimation::GetModel() const
{
	return (m_pSkinMesh && m_pSkinMesh->GetSkinMeshModel()) ? (m_pSkinMesh->GetSkinMeshModel()) : (m_pAnimModel);
}
DsModel* DsAnimation::GetModel()
{
	return (m_pSkinMesh && m_pSkinMesh->GetSkinMeshModel()) ? (m_pSkinMesh->GetSkinMeshModel()) : (m_pAnimModel);
}
