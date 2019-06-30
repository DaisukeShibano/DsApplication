#include "DsPch.h"
#ifndef _DS_MODEL_
#include "Model/DsModel.h"
#endif

using namespace DsLib;

namespace
{
	const int UPDATE_NORMAL_F_NUM = 200;
	const int UPDATE_NORMAL_V_NUM = 200;
}

DsModel::DsModel() 
	: m_pVertex(0)
	, m_vn(0)
	, m_pFace(0)
	, m_fn(0)
	, m_pDmypoly(NULL)
	, m_dn(0)
	, m_pMaterial(0)
	, m_mn(0)
	, m_pos(DsVec3d::Zero())
	, m_rot(DsMat33d::Identity())
	, m_pVertexNormalIdxs(NULL)
	, m_pVertexNormals(NULL)
	, m_updateNormalFIdx(0)
	, m_updateNormalVIdx(0)
{

}

DsModel::~DsModel()
{
	delete[] m_pVertex; m_pVertex = NULL;
	delete[] m_pFace; m_pFace = NULL;
	delete[] m_pDmypoly; m_pDmypoly = NULL;
	delete[] m_pMaterial; m_pMaterial = NULL;

	delete[] m_pVertexNormalIdxs; m_pVertexNormalIdxs = NULL;
	delete[] m_pVertexNormals; m_pVertexNormals = NULL;
}

//自分と同じ内容の別インスタンスを作成
DsModel* DsModel::CreateClone() const 
{
	DsModel* ret = new DsModel();
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

	if (0 < m_dn) {
		ret->m_dn = m_dn;
		ret->m_pDmypoly = new Dmypoly[m_dn];
		DS_ASSERT(ret->m_pDmypoly, "メモリ確保失敗");
		for (int dIdx = 0; dIdx < m_dn; ++dIdx) {
			ret->m_pDmypoly[dIdx] = m_pDmypoly[dIdx];
		}
	}

	//マテリアル
	ret->m_mn = m_mn;
	ret->m_pMaterial = new Material[m_mn];
	DS_ASSERT(ret->m_pMaterial, "メモリ確保失敗");
	for (int mi = 0; mi < m_mn; ++mi)
	{
		ret->m_pMaterial[mi].ambient = m_pMaterial[mi].ambient;
		ret->m_pMaterial[mi].diffuse = m_pMaterial[mi].diffuse;
		ret->m_pMaterial[mi].emissive = m_pMaterial[mi].emissive;
		ret->m_pMaterial[mi].specular = m_pMaterial[mi].specular;
		ret->m_pMaterial[mi].shininess = m_pMaterial[mi].shininess;

		ret->m_pMaterial[mi].isWaveNormal = m_pMaterial[mi].isWaveNormal;

		const int tn = m_pMaterial[mi].textureNum;
		ret->m_pMaterial[mi].textureNum = tn;
		ret->m_pMaterial[mi].pTexture = new Material::Texture[tn];
		DS_ASSERT(ret->m_pMaterial[mi].pTexture, "メモリ確保失敗");
		for (int ti = 0; ti < tn; ++ti)
		{
			ret->m_pMaterial[mi].pTexture[ti].pathAlbedo = m_pMaterial[mi].pTexture[ti].pathAlbedo;
			ret->m_pMaterial[mi].pTexture[ti].pathNormal = m_pMaterial[mi].pTexture[ti].pathNormal;
			ret->m_pMaterial[mi].pTexture[ti].pathSpecular = m_pMaterial[mi].pTexture[ti].pathSpecular;

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
			for(const int fIdx : ret->m_pMaterial[mi].pTexture[ti].refGeomFacesIndex)
			{
				Face* pRefFace = &ret->m_pFace[fIdx];
				ret->m_pMaterial[mi].pTexture[ti].refGeomFaces.push_back(pRefFace);
			}
		}
	}

	return ret;
}

void DsModel::CreateVertexNormal()
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

void DsModel::UpdateNormal()
{
	Face* pFace = GetFace();
	const DsVec4d* pVertex = GetVertex();
	const int fn = GetFaceNum();
	const int endFn = min(fn, m_updateNormalFIdx + UPDATE_NORMAL_F_NUM);
	int fi = m_updateNormalFIdx;
	for (; fi < endFn; ++fi)
	{
		const int fvn = pFace[fi].vn;
		DS_ASSERT((3 <= fvn), "DsModelの面の頂点数が３未満");
		pFace[fi].normal = DsVec4d::CalcNormal(pVertex[pFace[fi].pIndex[0]], pVertex[pFace[fi].pIndex[1]], pVertex[pFace[fi].pIndex[2]]);
	}
	m_updateNormalFIdx += fi;
	if (fn <= m_updateNormalFIdx) {
		m_updateNormalFIdx = 0;
	}


	//頂点法線更新
	if (m_pVertexNormalIdxs && m_pVertexNormals)
	{
		const int vn = GetVertexNum();
		const int endVn = min(vn, m_updateNormalVIdx + UPDATE_NORMAL_V_NUM);
		int vi = m_updateNormalVIdx;
		for (; vi < endVn; ++vi)
		{
			DsVec3d normal = DsVec3d::Zero();
			for (int fIdx : m_pVertexNormalIdxs[vi])
			{
				//頂点法線を、隣接している面の法線の合計から求める
				normal = normal + DsVec3d::ToVec3(static_cast<float>(pFace[fIdx].normal.x), static_cast<float>(pFace[fIdx].normal.y), static_cast<float>(pFace[fIdx].normal.z));
			}
			m_pVertexNormals[vi] = DsVec3d::Normalize(normal);
		}
		m_updateNormalVIdx += vi;
		if (vn <= m_updateNormalVIdx) {
			m_updateNormalVIdx = 0;
		}
	}
}


bool DsModel::GetDmypoly(int id, std::vector<DsMat44d>& outMat)const
{
	bool ret = false;

	if (0 <= id) {
		//数が増えるようならmapかsetで持ったほうがいいかな？
		for (int i = 0; i < m_dn; ++i) {
			if (m_pDmypoly[i].id == id) {
				DsVec3d pos = m_pVertex[m_pDmypoly[i].index[0]];
				DsVec3d yDir = DsVec3d::Normalize(DsVec3d(m_pVertex[m_pDmypoly[i].index[1]]) - pos);
				const DsVec3d zDir = DsVec3d::Normalize(DsVec3d(m_pVertex[m_pDmypoly[i].index[2]]) - pos);
				const DsVec3d xDir = DsVec3d::Normalize(DsVec3d::Cross(yDir, zDir));
				yDir = DsVec3d::Cross(zDir, xDir);//直行じゃないかもしれないのでy計算しなおす
				
				pos = GetRotation()*pos + GetPosition();
				DsMat33d rot = GetRotation()*DsMat33d::SetAxis(xDir, yDir, zDir);
				const DsMat44d mat = DsMat44d::Get(rot, pos);
				outMat.push_back(mat);
				ret = true;
			}
		}
	}

	return ret;
}