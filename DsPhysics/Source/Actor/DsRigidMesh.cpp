#include "DsPhysicsPch.h"
#ifndef _DS_RIGID_MESH_H_
#include "Actor/DsRigidMesh.h"
#endif
#ifndef _DS_ANIM_MODEL_
#include "Animation/DsAnimModel.h"
#endif
#ifndef __DS_BOUNDING_TREE_BASE__
#include "Collision/BoundingTree/DsBoundingTreeAabb.h"
#endif

using namespace DsLib;
using namespace DsPhysics;



DsRigidMesh::DsRigidMesh(const DsActorId& id, const char* name)
	: DsRigidBody(id, name)
	, m_pAabbTree(NULL)
{
}

DsRigidMesh::~DsRigidMesh()
{
	delete m_pAabbTree; m_pAabbTree = NULL;
	delete m_pCollisionGeometry; m_pCollisionGeometry = NULL;
}

//三角形のみなことが保障されているならモデルと頂点とか共有できるかも。
void DsRigidMesh::Create(const DsAnimModel& animModel)
{
	m_geomInfo.vn = animModel.GetVertexNum();
	m_geomInfo.pVertex = new DsVec3d[m_geomInfo.vn];
	DS_ASSERT(m_geomInfo.pVertex, "メモリ確保失敗");
	m_geomInfo.pVertexOriginal = new DsVec3d[m_geomInfo.vn];
	DS_ASSERT(m_geomInfo.pVertexOriginal, "メモリ確保失敗");

	for (int i = 0; i < m_geomInfo.vn; ++i)
	{
		m_geomInfo.pVertex[i] = animModel.GetVertex()[i];
		m_geomInfo.pVertexOriginal[i] = animModel.GetVertex()[i];
	}

	m_geomInfo.fn = animModel.GetFaceNum();
	m_geomInfo.pFace = new DsQuad[m_geomInfo.fn];
	DS_ASSERT(m_geomInfo.pFace, "メモリ確保失敗");
	std::vector<DsQuad> addFaces;
	for (int i = 0; i < m_geomInfo.fn; ++i)
	{
		const int vn = animModel.GetFace()[i].vn;
		
		if(3 == vn)
		{
			m_geomInfo.pFace[i].vn = vn;
			for (int vi = 0; vi < vn; ++vi)
			{
				m_geomInfo.pFace[i].index[vi] = animModel.GetFace()[i].pIndex[vi];
			}
			//法線を求める。animModelのを使う場合は頂点法線なので注意
			const int* tmpIdx = animModel.GetFace()[i].pIndex;
			const DsVec4d* tmpV = animModel.GetVertex();
			m_geomInfo.pFace[i].normal = DsVec3d::CalcNormal(tmpV[tmpIdx[0]], tmpV[tmpIdx[1]], tmpV[tmpIdx[2]]);

		}
		else if ( 4 <= vn)
		{
			//4角形以上は面を分割する。4角形からは平面でなくなる可能性があるので。
			
			//3角形まではif(3 == vn)と同じ。
			m_geomInfo.pFace[i].vn = 3;
			for (int vi = 0; vi < 3; ++vi)
			{
				m_geomInfo.pFace[i].index[vi] = animModel.GetFace()[i].pIndex[vi];
			}
			//法線を求める。animModelのを使う場合は頂点法線なので注意
			const int* tmpIdx = animModel.GetFace()[i].pIndex;
			const DsVec4d* tmpV = animModel.GetVertex();
			m_geomInfo.pFace[i].normal = DsVec3d::CalcNormal(tmpV[tmpIdx[0]], tmpV[tmpIdx[1]], tmpV[tmpIdx[2]]);
			

			//3つめの頂点以降から面を分割。隙間なく分割するには０番目を最後にする。3番目、4番目、０番目、のように分割させる
			int overVn = vn - m_geomInfo.pFace[i].vn;
			int curIndex = m_geomInfo.pFace[i].vn-1;
			while (overVn != 0)
			{
				//前回の頂点、前回+1、０番目の頂点、となるように3角形を作る

				//残り頂点数によって４角形か３角形か決める
				const int addFaceVn = 1;//３角形固定

				DsQuad addFace;
				addFace.vn = addFaceVn + 2;
				for (int vi = 0; vi < addFace.vn-1; ++vi)//追加分-1までの頂点で新しい面を作る。最後の頂点は０番固定
				{
					addFace.index[vi] = animModel.GetFace()[i].pIndex[curIndex+vi];
				}
				addFace.index[addFace.vn - 1] = animModel.GetFace()[i].pIndex[0];
				addFace.normal = DsVec3d::CalcNormal(m_geomInfo.pVertex[addFace.index[0]], m_geomInfo.pVertex[addFace.index[1]], m_geomInfo.pVertex[addFace.index[2]]);
				addFaces.push_back(addFace);

				curIndex += addFaceVn;
				overVn -= addFaceVn;
				DS_ASSERT((0 <= overVn), "面分割に失敗");//残り頂点はちょうど０になるはず
			}
		}
	}

	//分割面があったので、面リサイズ
	if(!addFaces.empty())
	{
		const size_t newSize = m_geomInfo.fn + addFaces.size();
		DsQuad* newFace = new DsQuad[newSize];
		DS_ASSERT(newFace, "メモリ確保失敗");
		for (int fi = 0; fi < m_geomInfo.fn; ++fi)
		{
			newFace[fi] = m_geomInfo.pFace[fi];
		}
		for (unsigned int fi = 0; fi < addFaces.size(); ++fi)
		{
			newFace[m_geomInfo.fn + fi] = addFaces[fi];
		}
		delete m_geomInfo.pFace;
		m_geomInfo.pFace = newFace;
		m_geomInfo.fn = static_cast<int>(newSize);
	}

	//面と面の判定があれば必要ないはず。
	m_geomInfo.ln = 0;
	m_geomInfo.pLine = 0;

	{
		DsVec3d maxP = DsVec3d(-DBL_MAX, -DBL_MAX, -DBL_MAX);
		DsVec3d minP = DsVec3d(DBL_MAX, DBL_MAX, DBL_MAX);
		for (int i = 0; i < m_geomInfo.vn; ++i)
		{
			const DsVec3d v = m_geomInfo.pVertex[i];
			maxP.x = max(maxP.x, v.x);
			maxP.y = max(maxP.y, v.y);
			maxP.z = max(maxP.z, v.z);
			minP.x = min(minP.x, v.x);
			minP.y = min(minP.y, v.y);
			minP.z = min(minP.z, v.z);
		}
		m_sideSize = (maxP - minP)*0.5;
	}

	{
		DsRigidPhysicsInfo& pi = m_physicsInfo;

		//mass
		{//慣性テンソルは立方体
			const double density = 2700.0;//アルミが2.7Mg/m~3 らしい
			const DsVec3d maxLength = _GetMaxVector(m_geomInfo.pVertex, m_geomInfo.vn);
			const double mass = density*maxLength.x*maxLength.y*maxLength.z;
			const DsVec3d bias = DsVec3d(1.0, 1.0, 1.0) + m_biasInertia;
			const double M = m_option.isStatic ? DS_MAX_MASS : mass;
			const double Ixx = min(M / 3.0 * (maxLength.y*maxLength.y + maxLength.z*maxLength.z) * bias.x, DS_MAX_MASS);
			const double Iyy = min(M / 3.0 * (maxLength.x*maxLength.x + maxLength.z*maxLength.z) * bias.y, DS_MAX_MASS);
			const double Izz = min(M / 3.0 * (maxLength.x*maxLength.x + maxLength.y*maxLength.y) * bias.z, DS_MAX_MASS);
			
			pi.mass.inertia = DsMat33d::Identity();
			pi.mass.inertia[0] = min(Ixx, DS_MAX_MASS);
			pi.mass.inertia[4] = min(Iyy, DS_MAX_MASS);
			pi.mass.inertia[8] = min(Izz, DS_MAX_MASS);
			pi.mass.mass = min(M, DS_MAX_MASS);
			pi.mass.inertiaOriginal = pi.mass.inertia;
		}

		//初期姿勢
		pi.rotation = DsMat33d::Identity();

		//初期位置
		pi.pos.Set(0.0, 0.0, 0.0);

		//重心位置
		DsRigidGeometryInfo& gi = m_geomInfo;
		//pi.centerOfGravity = _GetCenterOfGravity(m_geomInfo.pVertex, m_geomInfo.vn);
		pi.centerOfGravity = GetPosition();
	}

	//静的オブジェでも最初の一回だけUpdateしないと初期姿勢が反映されない
	m_isForceUpdate = true;
	m_isForceRotation = true;

	m_prePos = GetPosition() + m_initPos;
	m_preRot = GetRotation() * m_initRot;
	SetPosition(GetPosition() + m_initPos);
	SetRotation(GetRotation() * m_initRot);
	_Update(DsVec3d::Zero(), DsMat33d::Identity());

	m_pCollisionGeometry = new DsCollisionGeometry(m_geomInfo.pVertex, m_geomInfo.vn, m_geomInfo.pFace, m_geomInfo.fn,
		m_geomInfo.pLine, m_geomInfo.ln, GetId(), NULL, //1フレ前頂点はパフォーマンスのためやらない。
		m_sideSize, m_pAabbTree, &m_aabb, GetRotation());
	DS_ASSERT(m_pCollisionGeometry, "メモリ確保失敗");
}

//virtual 
void DsRigidMesh::_Update(const DsVec3d& deltaPos, const DsMat33d& deltaRot)
{
	bool isCreate = false;
	if (m_option.isRotation || m_isForceRotation){
		isCreate = true;
	}

	DsRigidBody::_Update(deltaPos, deltaRot);

	//座標の更新があったらツリー作り直し。動くものは頻繁に変わるので非対応
	if (isCreate && m_option.isStatic)
	{
		{
			//AABBツリー生成。
			delete m_pAabbTree;
			std::vector<int> faceIdx;
			for (int i = 0; i < m_geomInfo.fn; ++i) {
				faceIdx.push_back(i);
			}
			const int depth = DsBoundingTreeAabb::DEFAULT_DEPTH;
			m_pAabbTree = new DsBoundingTreeAabb(*this, m_aabb.GetCenter(), m_sideSize, m_geomInfo.pFace, m_geomInfo.fn, m_geomInfo.pVertex, m_geomInfo.vn, faceIdx, depth);
			DS_ASSERT(m_pAabbTree, "メモリ確保失敗");
			m_pAabbTree->ConstructTree();
		}
	}
}


//virtual 
void DsRigidMesh::Draw(DsDrawCommand& com)
{
	const DsVec3d* pVertex = m_geomInfo.pVertex;
	const int faceNum = m_geomInfo.fn;

	com.SetColor(m_dbgColor);
	for (int fIdx = 0; faceNum > fIdx; ++fIdx)
	{
		const DsQuad& face = m_geomInfo.pFace[fIdx];
		if (face.vn == 3)
		{
			com.DrawPolygon(pVertex[face.index[0]], pVertex[face.index[1]], pVertex[face.index[2]]);
		}
		else if (face.vn == 4)
		{
			com.DrawQuad(pVertex[face.index[0]], pVertex[face.index[1]], pVertex[face.index[2]], pVertex[face.index[3]]);
		}
	}
}


//////////////////////factory//////////////////////////
DsActor* DsRigidMesh::DsRigidMeshFactory::CreateIns(const DsActorId& id) const
{
	DsRigidMesh* pRet = new DsRigidMesh(id, m_name.c_str());
	if (pRet)
	{
		pRet->m_option = m_initOption;
		pRet->m_initPos = m_initPos;
		pRet->m_initRot = m_initRot;
		pRet->Create(m_anim);
		return pRet;
	}
	else
	{
		return NULL;
	}
}