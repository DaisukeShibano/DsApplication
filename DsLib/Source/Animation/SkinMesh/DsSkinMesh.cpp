#include "DsPch.h"
#ifndef _DS_SKINMESH_
#include "Animation/SkinMesh/DsSkinMesh.h"
#endif

#ifndef _DS_MODEL_
#include "Model/DsModel.h"
#endif
#ifndef _DS_AMIM_SKELETON_
#include "Animation/DsAnimSkeleton.h"
#endif
#ifndef _DS_DRAW_COMMAND_H_
#include "Graphics/Command/DsDrawCommand.h"
#endif

using namespace DsLib;

DsSkinMesh::DsSkinMesh(const DsModel& model)
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
			const int vertexIdx = bone->pIndex[idx];
			const DsVec3d srcV = srcVertex[vertexIdx];
			const double w = bone->pWeight[idx];

			//s_weigth[index] += w;//�f�o�b�O�p

			//���_�͏����p���̃{�[���ƈ�v���Ă���

			//initWorldPose �̓X�P�[�����������Ă���
			//bRot�͂��łɐe�̃X�P�[�����������Ă�

			//�����p������ǂꂾ�������������߂�B
			DsVec3d diffP = bPos - bone->initWorldPose.GetPos();//initWorldPos��T�X�^���X�̂Ƃ��̃|�[�Y�łȂ��ƃ_���B
			DsMat33d diffR = bRot*bone->initWorldPose.ToMat33().ToTransposition();
			
			//�A�j���ɂ���ĕό`�����{�[���Œ��_���ړ�������
			const DsVec3d transV = (diffR*(srcV - bone->initWorldPose.GetPos())) + (bone->initWorldPose.GetPos() + diffP);
			
			//�d�ݕt���������_�̕ω���
			const DsVec3d transVW = (transV - srcVertex[vertexIdx])*w;

			//���f
			vertex[vertexIdx] += DsVec4d(transVW.x, transVW.y, transVW.z, 0);
		}

		//�ċA��
		//for each(const DsAnimBone* child in bone->child)
		//{
		//	_Mapping(child, child->worldPose.ToMat33(), child->worldPose.GetPos(), vertex, srcVertex);
		//}
	}
}

//���_�f�[�^���X�P���g���̎p���ɍ��킹��
void DsSkinMesh::ApplySkeleton(const DsAnimSkeleton& skeleton)
{
	
	//�ό`����O�ɏ����p���Ń��Z�b�g
	const int vn = m_pModel->GetVertexNum();
	DsVec4d* pVertex = m_pModel->GetVertex();
	const DsVec4d* pSrcVertex = m_srcModel.GetVertex();
	for (int vIdx = 0; vIdx < vn; ++vIdx)
	{
		pVertex[vIdx] = pSrcVertex[vIdx];
	}
	
	const std::vector<DsAnimBone*>& boneArray = skeleton.RefBoneArray();
	for(const DsAnimBone* pBone : boneArray)
	{
		_Mapping(pBone, pBone->modelPose.ToMat33(), pBone->modelPose.GetPos(), m_pModel->GetVertex(), m_srcModel.GetVertex());
	}

	//�ċA��
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
		const DsModel:: Face* pFace = m_pModel->GetFace();
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