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

			//s_weigth[index] += w;//デバッグ用

			//頂点は初期姿勢のボーンと一致している

			//initWorldPose はスケールがかかっている
			//bRotはすでに親のスケールがかかってる

			//初期姿勢からどれだけ動いたか求める。
			DsVec3d diffP = bPos - bone->initWorldPose.GetPos();//initWorldPosがTスタンスのときのポーズでないとダメ。
			DsMat33d diffR = bRot*bone->initWorldPose.ToMat33().ToTransposition();
			
			//アニメによって変形したボーンで頂点を移動させる
			const DsVec3d transV = (diffR*(srcV - bone->initWorldPose.GetPos())) + (bone->initWorldPose.GetPos() + diffP);
			
			//重み付けした頂点の変化量
			const DsVec3d transVW = (transV - srcVertex[vertexIdx])*w;

			//反映
			vertex[vertexIdx] += DsVec4d(transVW.x, transVW.y, transVW.z, 0);
		}

		//再帰版
		//for each(const DsAnimBone* child in bone->child)
		//{
		//	_Mapping(child, child->worldPose.ToMat33(), child->worldPose.GetPos(), vertex, srcVertex);
		//}
	}
}

//頂点データをスケルトンの姿勢に合わせる
void DsSkinMesh::ApplySkeleton(const DsAnimSkeleton& skeleton)
{
	
	//変形する前に初期姿勢でリセット
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

	//再帰版
	//for each(const DsAnimBone* root in roots)
	//{
	//	_Mapping(root, root->worldPose.ToMat33(), root->worldPose.GetPos(), m_pModel->GetVertex(), m_srcModel.GetVertex());
	//}

	//アニメで変形した面の法線更新
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
			DS_ASSERT(vIdx <= tmpSize, "モデル描画時にテンプバッファサイズをオーバーしました。");
			const int index = pFace[fIdx].pIndex[vIdx];
			tmpBuffer[vIdx] = pVertex[index];
		}
		DsDbgSys::GetIns().RefDrawCom().DrawPolygon(tmpBuffer, vn);
	}
}