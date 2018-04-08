#include "DsPch.h"
#ifndef _DS_ANIM_MODEL_
#include "Animation/DsAnimModel.h"
#endif

using namespace DsLib;

DsAnimModel::DsAnimModel() 
	: m_pVertex(0)
	, m_vn(0)
	, m_pFace(0)
	, m_fn(0)
	, m_pMaterial(0)
	, m_mn(0)
	, m_pos(DsVec3d::Zero())
	, m_rot(DsMat33d::Identity())
	, m_pVertexNormalIdxs(NULL)
	, m_pVertexNormals(NULL)
{

}

DsAnimModel::~DsAnimModel()
{
	delete[] m_pVertex; m_pVertex = NULL;
	delete[] m_pFace; m_pFace = NULL;
	delete[] m_pMaterial; m_pMaterial = NULL;

	delete[] m_pVertexNormalIdxs; m_pVertexNormalIdxs = NULL;
	delete[] m_pVertexNormals; m_pVertexNormals = NULL;
}

//自分と同じ内容の別インスタンスを作成
DsAnimModel* DsAnimModel::CreateClone() const 
{
	DsAnimModel* ret = new DsAnimModel();
	DS_ASSERT(ret, "メモリ確保失敗");

	ret->m_vn = m_vn;
	ret->m_pVertex = new DsVec4d[m_vn];
	DS_ASSERT(ret->m_pVertex, "メモリ確保失敗");
	for (int vIdx = 0; vIdx < m_vn; ++vIdx)
	{
		ret->m_pVertex[vIdx] = m_pVertex[vIdx];
	}

	ret->m_fn = m_fn;
	ret->m_pFace = new Face[m_fn];
	DS_ASSERT(ret->m_pFace, "メモリ確保失敗");
	for (int fIdx = 0; fIdx < m_fn; ++fIdx)
	{
		ret->m_pFace[fIdx].normal = m_pFace[fIdx].normal;
		ret->m_pFace[fIdx].vn = m_pFace[fIdx].vn;
		ret->m_pFace[fIdx].pIndex = new int[m_pFace[fIdx].vn];
		DS_ASSERT(ret->m_pFace[fIdx].pIndex, "メモリ確保失敗");
		for (int vIdx = 0; vIdx < m_pFace[fIdx].vn; ++vIdx)
		{
			ret->m_pFace[fIdx].pIndex[vIdx] = m_pFace[fIdx].pIndex[vIdx];
		}
	}

	//マテリアル
	ret->m_mn = m_mn;
	ret->m_pMaterial = new Material[m_mn];
	DS_ASSERT(ret->m_pMaterial, "メモリ確保失敗");
	for (int mi = 0; mi < m_mn; ++mi)
	{
		const int tn = m_pMaterial[mi].textureNum;
		ret->m_pMaterial[mi].textureNum = tn;
		ret->m_pMaterial[mi].pTexture = new Material::Texture[tn];
		DS_ASSERT(ret->m_pMaterial[mi].pTexture, "メモリ確保失敗");
		for (int ti = 0; ti < tn; ++ti)
		{
			ret->m_pMaterial[mi].pTexture[ti].path = m_pMaterial[mi].pTexture[ti].path;
			const int uvn = m_pMaterial[mi].pTexture[ti].uvNum;
			ret->m_pMaterial[mi].pTexture[ti].uvNum = uvn;
			ret->m_pMaterial[mi].pTexture[ti].pUV = new Material::Texture::UV[uvn];
			DS_ASSERT(ret->m_pMaterial[mi].pTexture[ti].pUV, "メモリ確保失敗");
			for (int uvi = 0; uvi < uvn; ++uvi)
			{
				const Material::Texture::UV& uv = m_pMaterial[mi].pTexture[ti].pUV[uvi];
				ret->m_pMaterial[mi].pTexture[ti].pUV[uvi].x = uv.x;
				ret->m_pMaterial[mi].pTexture[ti].pUV[uvi].y = uv.y;
				ret->m_pMaterial[mi].pTexture[ti].pUV[uvi].vertexIdx = uv.vertexIdx;
			}

			ret->m_pMaterial[mi].pTexture[ti].refGeomFacesIndex.assign(m_pMaterial[mi].pTexture[ti].refGeomFacesIndex.begin(), m_pMaterial[mi].pTexture[ti].refGeomFacesIndex.end());
			for each(const int fIdx in ret->m_pMaterial[mi].pTexture[ti].refGeomFacesIndex)
			{
				Face* pRefFace = &ret->m_pFace[fIdx];
				ret->m_pMaterial[mi].pTexture[ti].refGeomFaces.push_back(pRefFace);
			}
		}
	}

	return ret;
}

void DsAnimModel::CreateVertexNormal()
{
	//１つの頂点に隣接している複数の面インデックスを全頂点分用意する

	const int vn = GetVertexNum();
	m_pVertexNormalIdxs = new ShareNlmIdxs[vn];
	DS_ASSERT(m_pVertexNormalIdxs, "メモリ確保失敗");
	m_pVertexNormals = new DsVec3d[vn];
	DS_ASSERT(m_pVertexNormals, "メモリ確保失敗");

	const Face* pFace = GetFace();
	const int fn = GetFaceNum();
	for (int vIdx = 0; vIdx < vn; ++vIdx)
	{
		//vIdxを使っている面のインデクスを検索
		ShareNlmIdxs fIndexs;//１つの頂点に隣接している面のインデックス
		for (int fIdx = 0; fIdx < fn; ++fIdx)
		{
			const int fvn = pFace[fIdx].vn;
			for (int fvIdx = 0; fvIdx < fvn; ++fvIdx)
			{
				if (pFace[fIdx].pIndex[fvIdx] == vIdx)
				{
					//この面はvIdxを含む面。保存。
					fIndexs.push_back(fIdx);
					break;
				}
			}
		}
		m_pVertexNormalIdxs[vIdx] = fIndexs;
	}
}

void DsAnimModel::UpdateNormal()
{
	Face* pFace = GetFace();
	const DsVec4d* pVertex = GetVertex();
	const int fn = GetFaceNum();
	for (int fi = 0; fi < fn; ++fi)
	{
		const int fvn = pFace[fi].vn;
		DS_ASSERT((3 <= fvn), "DsAnimModelの面の頂点数が３未満");
		pFace[fi].normal = DsVec4d::CalcNormal(pVertex[pFace[fi].pIndex[0]], pVertex[pFace[fi].pIndex[1]], pVertex[pFace[fi].pIndex[2]]);
	}

	//頂点法線更新
	if (m_pVertexNormalIdxs && m_pVertexNormals)
	{
		const int vn = GetVertexNum();
		for (int vi = 0; vi < vn; ++vi)
		{
			DsVec3d normal = DsVec3d::Zero();
			for each(int fIdx in m_pVertexNormalIdxs[vi])
			{
				//頂点法線を、隣接している面の法線の合計から求める
				normal = normal + DsVec3d::ToVec3(static_cast<float>(pFace[fIdx].normal.x), static_cast<float>(pFace[fIdx].normal.y), static_cast<float>(pFace[fIdx].normal.z));
			}
			m_pVertexNormals[vi] = DsVec3d::Normalize(normal);
		}
	}
}