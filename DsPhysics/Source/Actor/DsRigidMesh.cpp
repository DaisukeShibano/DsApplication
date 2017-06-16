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



namespace
{
	DsVec3d _GetMaxVector(const DsVec3d* pVec, const int vn)
	{
		double maxX = 0.0;
		double maxY = 0.0;
		double maxZ = 0.0;

		for (int i = 0; vn > i; ++i)
		{
			maxX = (maxX > fabs(pVec[i].x)) ? maxX : fabs(pVec[i].x);
			maxY = (maxY > fabs(pVec[i].y)) ? maxY : fabs(pVec[i].y);
			maxZ = (maxZ > fabs(pVec[i].z)) ? maxZ : fabs(pVec[i].z);
		}

		const DsVec3d ret = { maxX*2.0, maxY*2.0, maxZ*2.0 };
		return ret;
	}

	DsVec3d _GetCenterOfGravity(const DsVec3d* v, const int vertexNum)
	{
		DsVec3d sum = { 0.0, 0.0, 0.0 };
		for (int vn = 0; vertexNum > vn; ++vn)
		{
			sum += v[vn];
		}
		const DsVec3d ave = { sum.x / vertexNum, sum.y / vertexNum, sum.z / vertexNum };
		return ave;
	}
}



DsRigidMesh::DsRigidMesh(const DsActorId& id, const char* name)
	: DsRigidBody(id, name)
	, m_pAabbTree(NULL)
{
}

DsRigidMesh::~DsRigidMesh()
{
	delete m_pAabbTree; m_pAabbTree = NULL;
	delete m_pCollisionContext; m_pCollisionContext = NULL;
}

void DsRigidMesh::Create(const DsAnimModel& animModel)
{
	m_geomInfo.vn = animModel.GetVertexNum();
	m_geomInfo.pVertex = new DsVec3d[m_geomInfo.vn];
	m_geomInfo.pVertexOriginal = new DsVec3d[m_geomInfo.vn];
	for (int i = 0; i < m_geomInfo.vn; ++i)
	{
		m_geomInfo.pVertex[i] = animModel.GetVertex()[i];
		m_geomInfo.pVertexOriginal[i] = animModel.GetVertex()[i];
	}

	m_geomInfo.fn = animModel.GetFaceNum();
	m_geomInfo.pFace = new DsQuad[m_geomInfo.fn];
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
		DsRigidPhysicsInfo& pi = m_physicsInfo;

		//mass
		{//慣性テンソルは立方体
			const double density = 2700.0;//アルミが2.7Mg/m~3 らしい
			const DsVec3d maxLength = _GetMaxVector(m_geomInfo.pVertex, m_geomInfo.vn);
			const double mass = density*maxLength.x*maxLength.y*maxLength.z;
			const double bias = 1.0;
			const double M = m_option.isStatic ? DsMathUtil::DS_INFINITY_D : mass;
			const double Ixx = M / 3.0 * (maxLength.y*maxLength.y + maxLength.z*maxLength.z) * bias;
			const double Iyy = M / 3.0 * (maxLength.x*maxLength.x + maxLength.z*maxLength.z) * bias;
			const double Izz = M / 3.0 * (maxLength.x*maxLength.x + maxLength.y*maxLength.y) * bias;
			
			const double maxMass = 1000;
			pi.mass.inertia = DsMat33d::Identity();
			pi.mass.inertia[0] = min(Ixx, maxMass);
			pi.mass.inertia[4] = min(Iyy, maxMass);
			pi.mass.inertia[8] = min(Izz, maxMass);
			pi.mass.mass = min(M, maxMass);
		}

		//初期姿勢
		pi.rotation = DsMat33d::Identity();

		//初期位置
		pi.pos.Set(0.0, 0.0, 0.0);

		//重心位置
		DsRigidGeometryInfo& gi = m_geomInfo;
		//pi.centerOfGravity = _GetCenterOfGravity(m_geomInfo.pVertex, m_geomInfo.vn);
		pi.centerOfGravity = GetPosition();

		//AABB
		DsVec3d maxLen = DsVec3d::Zero();
		for (int i = 0; i < m_geomInfo.vn; ++i)
		{
			const DsVec3d len = gi.pVertex[i] - pi.centerOfGravity;
			maxLen.x = max(maxLen.x, fabs(len.x));
			maxLen.y = max(maxLen.y, fabs(len.y));
			maxLen.z = max(maxLen.z, fabs(len.z));
		}
		m_aabb.Setup(maxLen.x, maxLen.y, maxLen.z, pi.centerOfGravity);
		m_sideSize.x = maxLen.x;
		m_sideSize.y = maxLen.y;
		m_sideSize.z = maxLen.z;
	}

	{//AABBツリー生成。
		std::vector<int> faceIdx;
		for (int i = 0; i < m_geomInfo.fn; ++i){
			faceIdx.push_back(i);
		}
		const int depth = (m_option.isStatic) ? (DsBoundingTreeAabb::DEFAULT_DEPTH) : (0);//分割が動くものに対応してない
		m_pAabbTree = new DsBoundingTreeAabb(*this, m_physicsInfo.pos, m_sideSize, m_geomInfo.pFace, m_geomInfo.fn, m_geomInfo.pVertex, m_geomInfo.vn, faceIdx, depth);
		m_pAabbTree->ConstructTree();
	}

	//静的オブジェでも最初の一回だけUpdateしないと初期姿勢が反映されない
	m_isForceUpdate = true;
	m_isForceRotation = true;

	_Update(m_initPos, m_initRot);

	m_pCollisionContext = new DsCollisionContext(m_geomInfo.pVertex, m_geomInfo.vn, m_geomInfo.pFace, m_geomInfo.fn,
		m_geomInfo.pLine, m_geomInfo.ln, GetId(), m_physicsInfo.centerOfGravity, NULL, //1フレ前頂点はパフォーマンスのためやらない。
		m_sideSize, m_pAabbTree, &m_aabb, GetRotation());
}

void DsRigidMesh::_Update(const DsVec3d& deltaPos, const DsMat33d& deltaRot)
{
	if (m_option.isStatic && (!m_isForceUpdate) && (!m_isForceRotation))
	{
		return;
	}

	DsRigidGeometryInfo& gi = m_geomInfo;
	DsRigidPhysicsInfo& pi = m_physicsInfo;

	//初期姿勢反映のため回転成分を有効化
	if (m_option.isRotation || m_isForceRotation)
	{
		//姿勢更新
		{
			pi.rotation = deltaRot * pi.rotation;
		}
	}
	
	const bool isChangeRot = m_isForceRotation || (!deltaRot.IsNearEqual(DsMat33d::Identity()));
	if (isChangeRot || m_isForceUpdate)
	{
		//頂点回転
		for (int vn = 0; gi.vn > vn; ++vn)
		{
			gi.pVertex[vn] = pi.rotation * gi.pVertexOriginal[vn];
		}

		//慣性テンソル更新
		{
			const DsMat33d invR = DsMat33d::Inverse(deltaRot);
			const DsMat33d rotI = (deltaRot * pi.mass.inertia) * invR;
			pi.mass.inertia = rotI;
		}
		//拘束用慣性テンソル逆行列
		{
			const DsMat33d&& invRI = DsMat33d::Inverse(pi.mass.inertia);
			pi.massInv[0] = 1.0 / pi.mass.mass;
			pi.massInv[1] = invRI[0]; pi.massInv[2] = invRI[1]; pi.massInv[3] = invRI[2];
			pi.massInv[4] = invRI[3]; pi.massInv[5] = invRI[4]; pi.massInv[6] = invRI[5];
			pi.massInv[7] = invRI[6]; pi.massInv[8] = invRI[7]; pi.massInv[9] = invRI[8];
		}

		//法線更新
		for (int fn = 0; gi.fn > fn; ++fn)
		{
			const DsVec3d rotN = deltaRot * gi.pFace[fn].normal;//なんフレかに一回求め直したほうがいいかも。
			gi.pFace[fn].normal = DsVec3d::Normalize(rotN);//正規化は毎フレやらなくても良い気がする
		}

		//位置更新
		{
			pi.pos += deltaPos;
		}

		//頂点移動
		for (int vn = 0; gi.vn > vn; ++vn)
		{
			gi.pVertex[vn] = pi.pos + gi.pVertex[vn];
		}
	}
	else if (!deltaPos.IsNearZero())
	{
		//位置更新
		{
			pi.pos += deltaPos;
		}

		//頂点移動
		for (int vn = 0; gi.vn > vn; ++vn)
		{
			gi.pVertex[vn] += deltaPos;
		}
	}

	//重心更新
	//pi.centerOfGravity = _GetCenterOfGravity(gi.pVertex, gi.vn);
	pi.centerOfGravity = GetPosition();
	
	//AABB
	{
		if (isChangeRot)
		{
			DsVec3d maxLen = DsVec3d::Zero();
			for (int i = 0; i < m_geomInfo.vn; ++i)
			{
				const DsVec3d len = gi.pVertex[i] - pi.centerOfGravity;
				maxLen.x = max(maxLen.x, fabs(len.x));
				maxLen.y = max(maxLen.y, fabs(len.y));
				maxLen.z = max(maxLen.z, fabs(len.z));
			}
			m_aabb.Setup(maxLen.x, maxLen.y, maxLen.z, pi.centerOfGravity);
		}
		else
		{
			//回転してないなら中心座標だけ更新
			m_aabb.SetPos(pi.centerOfGravity);
		}
	}

	m_isForceUpdate = false;
	m_isForceRotation = false;
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