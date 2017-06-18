#include "DsPch.h"
#ifndef _DS_SKINMESH_
#include "Animation/SkinMesh/DsSkinMesh.h"
#endif

#ifndef _DS_ANIM_MODEL_
#include "Animation/DsAnimModel.h"
#endif
#ifndef _DS_AMIM_SKELETON_
#include "Animation/DsAnimSkeleton.h"
#endif
#ifndef _DS_DRAW_COMMAND_H_
#include "Graphics/Command/DsDrawCommand.h"
#endif

using namespace DsLib;

DsSkinMesh::DsSkinMesh(const DsAnimModel& model)
	:m_srcModel(model)
	,m_pModel(NULL)
{
}

DsSkinMesh::~DsSkinMesh()
{
	delete m_pModel; m_pModel = NULL;
}

void DsSkinMesh::Initialize()
{
	m_pModel = m_srcModel.CreateClone();
}

namespace
{
	static const int S_WEI_NUM = 1024 * 32;
	static double s_weigth[S_WEI_NUM];

	void _Mapping(const DsAnimBone* bone, const DsMat33d& bRot, const DsVec3d& bPos, DsVec4d* vertex, const DsVec4d* srcVertex)
	{
		for (int idx = 0; idx < bone->vIndexNum; ++idx)
		{	
			const DsVec3d srcV = srcVertex[bone->pIndex[idx]];
			const double w = bone->pWeight[idx];

			//s_weigth[index] += w;//�f�o�b�O�p

			//���_�͏����p���̃{�[���ƈ�v���Ă���

			//initWorldPose �̓X�P�[�����������Ă���
			//bRot�@�͂��łɐe�̃X�P�[�����������Ă�

			//�����p������ǂꂾ�������������߂�B
			DsVec3d diffP = bPos - bone->initWorldPose.GetPos();//initWorldPos��T�X�^���X�̂Ƃ��̃|�[�Y�łȂ��ƃ_���B
			DsMat33d diffR = bRot*bone->initWorldPose.ToMat33().ToTransposition();
			
			//�A�j���ɂ���ĕό`�����{�[���Œ��_���ړ�������
			const DsVec3d transV = (diffR*(srcV - bone->initWorldPose.GetPos())) + (bone->initWorldPose.GetPos() + diffP);
			
			//�d�ݕt���������_�̕ω���
			const DsVec3d transVW = (transV - srcVertex[bone->pIndex[idx]])*w;

			//���f
			const int dstIdx = bone->pIndex[idx];
			vertex[dstIdx] += DsVec4d(transVW.x, transVW.y, transVW.z, 0);
		}

		//�ċN��
		//for each(const DsAnimBone* child in bone->child)
		//{
		//	_Mapping(child, child->worldPose.ToMat33(), child->worldPose.GetPos(), vertex, srcVertex);
		//}
	}
}

//���_�f�[�^���X�P���g���̎p���ɍ��킹��
void DsSkinMesh::ApplySkeleton(const DsAnimSkeleton& skeleton)
{
	//for (int i = 0; i < S_WEI_NUM; ++i)
	//{
	//	s_weigth[i] = 0.0;//�f�o�b�O�p
	//}
	
	const std::vector<DsAnimBone*>& roots = skeleton.RefRootBone();
	if (roots.empty())
	{
		//�����Ȃ��ꍇ��Root���K�p����Ȃ��̂ŕʓr����
		const int vn = m_pModel->GetVertexNum();
		DsVec4d* pVertex = m_pModel->GetVertex();
		const DsVec4d* pSrcVertex = m_srcModel.GetVertex();
		for (int vIdx = 0; vIdx < vn; ++vIdx)
		{
			pVertex[vIdx] = (skeleton.GetRootRot()*pSrcVertex[vIdx]) + skeleton.GetRootPos();
		}
	}
	else
	{
		//�ό`����O�ɏ����p���Ń��Z�b�g
		const int vn = m_pModel->GetVertexNum();
		DsVec4d* pVertex = m_pModel->GetVertex();
		const DsVec4d* pSrcVertex = m_srcModel.GetVertex();
		for (int vIdx = 0; vIdx < vn; ++vIdx)
		{
			pVertex[vIdx] = pSrcVertex[vIdx];
		}
	}

	
	const std::vector<DsAnimBone*>& boneArray = skeleton.RefBoneArray();
	for each(const DsAnimBone* pBone in boneArray)
	{
		_Mapping(pBone, pBone->worldPose.ToMat33(), pBone->worldPose.GetPos(), m_pModel->GetVertex(), m_srcModel.GetVertex());
	}

	//�ċN��
	//for each(const DsAnimBone* root in roots)
	//{
	//	_Mapping(root, root->worldPose.ToMat33(), root->worldPose.GetPos(), m_pModel->GetVertex(), m_srcModel.GetVertex());
	//}

	//�A�j���ŕό`�����ʂ̖@���X�V
	m_pModel->UpdateNormal();
}

void DsSkinMesh::DbgDraw() const
{
	const int tmpSize = 32;
	DsVec3d tmpBuffer[tmpSize];
	const DsVec4d* pVertex = m_pModel->GetVertex();
	const int fn = m_pModel->GetFaceNum();
	for (int fIdx = 0; fIdx < fn; ++fIdx)
	{
		const DsAnimModel:: Face* pFace = m_pModel->GetFace();
		const int vn = pFace[fIdx].vn;
		for (int vIdx = 0; vIdx < vn; ++vIdx)
		{
			DS_ASSERT(vIdx <= tmpSize, "���f���`�掞�Ƀe���v�o�b�t�@�T�C�Y���I�[�o�[���܂����B");
			const int index = pFace[fIdx].pIndex[vIdx];
			tmpBuffer[vIdx] = pVertex[index];
		}
		DsDbgSys::GetIns().RefDrawCom().DrawPolygon(tmpBuffer, vn);
	}
}