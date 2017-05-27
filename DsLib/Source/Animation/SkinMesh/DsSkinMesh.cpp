#include "DsPch.h"
#ifndef _DS_SKINMESH_
#include "Animation/SkinMesh/DsSkinMesh.h"
#endif

#ifndef _DS_ANIM_MODEL_
#include "Animation/DsAnimModel.h"
#endif
#ifndef _DS_AMIM_SKELTON_
#include "Animation/DsAnimSkelton.h"
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

			//s_weigth[index] += w;//デバッグ用

			//頂点は初期姿勢のボーンと一致している

			//initWorldPose はスケールがかかっている
			//bRot　はすでに親のスケールがかかってる

			//初期姿勢からどれだけ動いたか求める。
			DsVec3d diffP = bPos - bone->initWorldPose.GetPos();//initWorldPosがTスタンスのときのポーズでないとダメ。
			DsMat33d diffR = bRot*bone->initWorldPose.ToMat33().ToTransposition();
			
			//アニメによって変形したボーンで頂点を移動させる
			const DsVec3d transV = (diffR*(srcV - bone->initWorldPose.GetPos())) + (bone->initWorldPose.GetPos() + diffP);
			
			//重み付けした頂点の変化量
			const DsVec3d transVW = (transV - srcVertex[bone->pIndex[idx]])*w;

			//反映
			const int dstIdx = bone->pIndex[idx];
			vertex[dstIdx] += DsVec4d(transVW.x, transVW.y, transVW.z, 0);
		}

		for each(const DsAnimBone* child in bone->child)
		{
			//位置は親の座標から見た位置っぽい
			DsVec3d bonePos = bRot*(child->localPose.GetPos() );
			bonePos = bonePos + bPos;

			//座標は親からの累積
			const DsMat33d&& boneRot = bRot*child->localPose.ToMat33();
			_Mapping(child, boneRot, bonePos, vertex, srcVertex);
		}
	}
}

//頂点データをスケルトンの姿勢に合わせる
void DsSkinMesh::ApplySkelton(const DsAnimSkelton& skelton)
{
	//for (int i = 0; i < S_WEI_NUM; ++i)
	//{
	//	s_weigth[i] = 0.0;//デバッグ用
	//}
	
	const DsAnimSkelton::Bones& roots = skelton.RefRootBone();
	if (roots.empty())
	{
		//骨がない場合はRootが適用されないので別途処理
		const int vn = m_pModel->GetVertexNum();
		DsVec4d* pVertex = m_pModel->GetVertex();
		const DsVec4d* pSrcVertex = m_srcModel.GetVertex();
		for (int vIdx = 0; vIdx < vn; ++vIdx)
		{
			pVertex[vIdx] = (skelton.GetRootRot()*pSrcVertex[vIdx]) + skelton.GetRootPos();
		}
	}
	else
	{
		//変形する前に初期姿勢でリセット
		const int vn = m_pModel->GetVertexNum();
		DsVec4d* pVertex = m_pModel->GetVertex();
		const DsVec4d* pSrcVertex = m_srcModel.GetVertex();
		for (int vIdx = 0; vIdx < vn; ++vIdx)
		{
			pVertex[vIdx] = pSrcVertex[vIdx];
		}
	}

	
	for each(const DsAnimBone* root in roots)
	{
		//FBX変換かかってなさげなのでここで掛ける(そのうち出力ツール側でやりたい)
		//const DsMat33d fbxR = DsMat33d::RotateX(-M_PI_2);
		//intWorldはFbx変換かかってる。localPoseはかかってない。Fbx変換成分だけを抜き出す。ただし、localPoseの姿勢がキーフレームで変わってるとダメ
		const DsMat33d fbxR = root->initWorldPose.ToMat33()*root->localPose.ToMat33().ToTransposition();

		const DsMat33d m = skelton.GetRootRot()*fbxR*root->localPose.ToMat33();	//マスターの姿勢は変えられなかったから考慮しなくてOK
		//localPoseはエクスポート時のFbx座標変換がかかってない
		//localPoseを打ち消したら考慮した座標と一致した。とりあえずこれで行く
		const DsVec3d offset = (fbxR*root->localPose.GetPos());
		const DsVec3d p = offset + skelton.GetRootPos() + root->initWorldPose.GetPos();

		_Mapping(root, m, p, m_pModel->GetVertex(), m_srcModel.GetVertex());
	}

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
		const DsAnimModel:: Face* pFace = m_pModel->GetFace();
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