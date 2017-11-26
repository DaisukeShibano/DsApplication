#include "DsPhysicsPch.h"
#ifndef __DS_BOUNDING_TREE_AABB__
#include "Collision/BoundingTree/DsBoundingTreeAabb.h"
#endif
#ifndef __DS_ACTOR__
#include "Actor/DsActor.h"
#endif
#ifndef _DS_AABB_H_
#include "Math/DsAabb.h"
#endif

using namespace DsLib;
using namespace DsPhysics;

static const int CHILD_NUM = 8;

DsBoundingTreeAabb::DsBoundingTreeAabb(const DsActor& actor, const DsVec3d& pos, const DsVec3d& boxSize, const DsQuad* pFaces, int faceNum, const DsVec3d* pVertex, int vertexNum, const std::vector<int>& containFaceIdxs, int maxDepth)
	: DsBoundingTreeBase()
	, m_pos(pos)
	, m_boxSize(boxSize)
	, m_owner(actor)
	, m_pFace(pFaces)
	, m_faceNum(faceNum)
	, m_pVertex(pVertex)
	, m_vertexNum(vertexNum)
	, m_containFaceIdxs(containFaceIdxs)
	, m_child()
	, m_maxDepth(maxDepth)
{
	m_child.reserve(CHILD_NUM);
}

DsBoundingTreeAabb::~DsBoundingTreeAabb()
{
	for each(DsBoundingTreeAabb* pNode in m_child){
		delete pNode;
	}
}

//virtual 
void DsBoundingTreeAabb::ConstructTree()
{
	for each(DsBoundingTreeAabb* pNode in m_child){
		delete pNode;
	}
	m_child.clear();
	_ConstructTree(0);
}

void DsBoundingTreeAabb::_ConstructTree(int depth)
{
	const bool isTerminal = (m_containFaceIdxs.size() <= CHILD_NUM) //分割数よりも面の数の方が少ないので、面のAABBを末端として生成。ここは別に分割数じゃなくてもいいかも
						|| (depth >= m_maxDepth);
	if (isTerminal){
		//三角形のaabbを得る
		for each(int faceIdx in m_containFaceIdxs){
			const DsQuad& face = m_pFace[faceIdx];
			const int vn = face.vn;
			DsVec3d maxP = DsVec3d(-DBL_MAX, -DBL_MAX, -DBL_MAX);
			DsVec3d minP = DsVec3d(DBL_MAX, DBL_MAX, DBL_MAX);
			for (int vi = 0; vi < vn; ++vi){
				const DsVec3d v = m_pVertex[face.index[vi]];
				maxP.x = max(maxP.x, v.x);
				maxP.y = max(maxP.y, v.y);
				maxP.z = max(maxP.z, v.z);
				minP.x = min(minP.x, v.x);
				minP.y = min(minP.y, v.y);
				minP.z = min(minP.z, v.z);
			}
			DsAabb tri;
			tri.Setup(maxP, minP);
			const DsVec3d maxLen = tri.GetMax() - tri.GetCenter();

			//三角形１つ分のaabbを末端ノードにする
			std::vector<int> tmp;
			tmp.push_back(faceIdx);
			DsBoundingTreeAabb* pChild = new DsBoundingTreeAabb(m_owner, tri.GetCenter(), maxLen, m_pFace, m_faceNum, m_pVertex, m_vertexNum, tmp, m_maxDepth);
			DS_ASSERT(pChild, "メモリ確保失敗");
			m_child.push_back(pChild);
		}
	}
	else{
		//分割空間
		const DsVec3d halfSize = m_boxSize*0.5;
		const DsVec3d offsetPos[] =	//子ノードの基準位置
		{
			m_pos + DsVec3d(halfSize.x, halfSize.y, halfSize.z),
			m_pos + DsVec3d(halfSize.x, halfSize.y, -halfSize.z),
			m_pos + DsVec3d(-halfSize.x, halfSize.y, -halfSize.z),
			m_pos + DsVec3d(-halfSize.x, halfSize.y, halfSize.z),

			m_pos + DsVec3d(halfSize.x, -halfSize.y, halfSize.z),
			m_pos + DsVec3d(halfSize.x, -halfSize.y, -halfSize.z),
			m_pos + DsVec3d(-halfSize.x, -halfSize.y, -halfSize.z),
			m_pos + DsVec3d(-halfSize.x, -halfSize.y, halfSize.z),
		};
		static_assert(sizeof(offsetPos) / sizeof(offsetPos[0]) == CHILD_NUM, "サイズ不一致");

		DsAabb box;
		for (int ci = 0; ci < CHILD_NUM; ++ci){
			box.Setup( offsetPos[ci] + halfSize,  offsetPos[ci] - halfSize);

			std::vector<int> containFaceIdxs;

			//三角形のaabbを得る。どこ所属の三角形かを算出
			for each(int faceIdx in m_containFaceIdxs){
				const DsQuad& face = m_pFace[faceIdx];
				const int vn = face.vn;
				DsVec3d maxP = DsVec3d(-DBL_MAX, -DBL_MAX, -DBL_MAX);
				DsVec3d minP = DsVec3d(DBL_MAX, DBL_MAX, DBL_MAX);
				for (int vi = 0; vi < vn; ++vi){
					const DsVec3d v = m_pVertex[face.index[vi]];
					maxP.x = max(maxP.x, v.x);
					maxP.y = max(maxP.y, v.y);
					maxP.z = max(maxP.z, v.z);
					minP.x = min(minP.x, v.x);
					minP.y = min(minP.y, v.y);
					minP.z = min(minP.z, v.z);
				}

				DsAabb tri;
				tri.Setup(maxP, minP);

				if (DsAabb::IsContain(tri, box)){
					//この境界の中に含まれてた
					containFaceIdxs.push_back(faceIdx);
				}
			}

			if (!containFaceIdxs.empty())
			{
				DsBoundingTreeAabb* pChild = new DsBoundingTreeAabb(m_owner, offsetPos[ci], halfSize, m_pFace, m_faceNum, m_pVertex, m_vertexNum, containFaceIdxs, m_maxDepth);
				DS_ASSERT(pChild, "メモリ確保失敗");
				pChild->_ConstructTree(depth + 1);
				m_child.push_back(pChild);
			}
		}
	}
}

//virtual 
bool DsBoundingTreeAabb::IsContain(const DsBoundingTreeAabb& cmp) const
{
	DsAabb a;
	a.Setup( m_pos + m_boxSize, m_pos - m_boxSize);

	DsAabb b;
	b.Setup( cmp.m_pos + cmp.m_boxSize, cmp.m_pos - cmp.m_boxSize);

	//まずは自分にcmpが含まれているか検査
	const bool isContain = DsAabb::IsContain(a, b);
	if (isContain){//含まれてたのでさらに内部の子を検索
		for each(const DsBoundingTreeAabb* pChild in m_child){
			if (pChild->IsContain(cmp)){
				return true;
			}
		}
	}
	
	return (isContain && m_child.empty());//末端(=三角形のAABB)に当たっていたかを知りたい
}


//virtual
bool DsBoundingTreeAabb::IsContain(const DsAabb& cmp) const
{
	DsAabb a;
	a.Setup(m_pos + m_boxSize, m_pos - m_boxSize);

	DsAabb b = cmp;
	

	//まずは自分にcmpが含まれているか検査
	const bool isContain = DsAabb::IsContain(a, b);
	if (isContain) {//含まれてたのでさらに内部の子を検索
		for each(const DsBoundingTreeAabb* pChild in m_child) {
			if (pChild->IsContain(cmp)) {
				return true;
			}
		}
	}

	return (isContain && m_child.empty());//末端(=三角形のAABB)に当たっていたかを知りたい
}

//virtual 
void DsBoundingTreeAabb::Draw(DsDrawCommand& com) const
{
	DsAabb a;
	a.Setup(m_pos + m_boxSize, m_pos - m_boxSize);
	a.Draw(com);
	
	for each(DsBoundingTreeAabb* pChild in m_child){
		pChild->Draw(com);
	}
}
