#include "DsPch.h"
#include "Collision/DsAabbTree.h"
#include "Collision/DsAabbNode.h"

using namespace DsLib;


DsAabbTree::DsAabbTree()
	: m_pNodeBuf(NULL)
	, m_pVertex(NULL)
	, m_pIndex(NULL)
	, m_vertexNum(0)
	, m_indexNum(0)
	, m_treeSize(0)
	, m_useIndex(0)
	, m_maxDepth(10)
{
}


DsAabbTree::~DsAabbTree()
{
	delete m_pNodeBuf;
	m_pNodeBuf = NULL;
}


void DsAabbTree::Init(const DsVec3d* pVetex, int vertexNum, const int* pIndex, int indexNum)
{
	m_pVertex = pVetex;
	m_pIndex = pIndex;
	m_vertexNum = vertexNum;
	m_indexNum = indexNum;
	m_treeSize = indexNum * 2;
	m_pNodeBuf = new DsAabbNode[m_treeSize];
	DS_ASSERT(m_pNodeBuf, "ÉÅÉÇÉäämï€é∏îs");
	TreeUpdate();
}

namespace
{
	/*
		óßï˚ëÃÇÃ8ï™äÑindex
		â∫ïî4Ç¬
		3 2
		0 1
		è„ïîÇ¬
		7 6
		4 5
	*/

	void _DivAabb0(DsAabb& aabb, const double maxV[3], const double minV[3], const double len[3])
	{
		aabb.Setup(DsVec3d(maxV[0] - len[0], maxV[1] - len[1], maxV[2] - len[2]), DsVec3d(minV[0]         , minV[1]         , minV[2]         ));
	}
	void _DivAabb1(DsAabb& aabb, const double maxV[3], const double minV[3], const double len[3])
	{
		aabb.Setup(DsVec3d(maxV[0]         , maxV[1] - len[1], maxV[2] - len[2]), DsVec3d(minV[0] + len[0], minV[1]         , minV[2]         ));
	}
	void _DivAabb2(DsAabb& aabb, const double maxV[3], const double minV[3], const double len[3])
	{
		aabb.Setup(DsVec3d(maxV[0]         , maxV[1] - len[1], maxV[2]         ), DsVec3d(minV[0] + len[0], minV[1]         , minV[2] + len[2]));
	}
	void _DivAabb3(DsAabb& aabb, const double maxV[3], const double minV[3], const double len[3])
	{
		aabb.Setup(DsVec3d(maxV[0] - len[0], maxV[1] - len[1], maxV[2]         ), DsVec3d(minV[0]         , minV[1]         , minV[2] + len[2]));
	}
	void _DivAabb4(DsAabb& aabb, const double maxV[3], const double minV[3], const double len[3])
	{
		aabb.Setup(DsVec3d(maxV[0] - len[0], maxV[1]         , maxV[2] - len[2]), DsVec3d(minV[0]         , minV[1] - len[1], minV[2]         ));
	}
	void _DivAabb5(DsAabb& aabb, const double maxV[3], const double minV[3], const double len[3])
	{
		aabb.Setup(DsVec3d(maxV[0]         , maxV[1]         , maxV[2] - len[2]), DsVec3d(minV[0] + len[0], minV[1] - len[1], minV[2]         ));
	}
	void _DivAabb6(DsAabb& aabb, const double maxV[3], const double minV[3], const double len[3])
	{
		aabb.Setup(DsVec3d(maxV[0]         , maxV[1]         , maxV[2]         ), DsVec3d(minV[0] + len[0], minV[1] - len[1], minV[2] + len[2]));
	}
	void _DivAabb7(DsAabb& aabb, const double maxV[3], const double minV[3], const double len[3])
	{
		aabb.Setup(DsVec3d(maxV[0] - len[0], maxV[1]         , maxV[2]         ), DsVec3d(minV[0]         , minV[1] - len[1], minV[2] + len[2]));
	}
	
	void (* const _DivAabb[8]) (DsAabb& aabb, const double maxV[3], const double minV[3], const double len[3]) =
	{
		&_DivAabb0,
		&_DivAabb1,
		&_DivAabb2,
		&_DivAabb3,
		&_DivAabb4,
		&_DivAabb5,
		&_DivAabb6,
		&_DivAabb7,
	};

}


void DsAabbTree::TreeUpdate()
{
	double maxV[3];
	double minV[3];
	for (int vi = 0; vi < m_vertexNum; ++vi) {
		const DsVec3d& v = m_pVertex[vi];
		maxV[0] = max(maxV[0], v.x);
		maxV[1] = max(maxV[1], v.y);
		maxV[2] = max(maxV[2], v.z);
		minV[0] = min(minV[0], v.x);
		minV[1] = min(minV[1], v.y);
		minV[2] = min(minV[2], v.z);
	}

	m_useIndex = 0;
	DsAabbNode* pRoot = _Create();
	pRoot->m_aabb.Setup(DsVec3d(maxV[0], maxV[1], maxV[2]), DsVec3d(minV[0], minV[1], minV[2]));
	pRoot->m_parentIdx = -1;

	double len[3];
	len[0] = pRoot->m_aabb.LenX();
	len[1] = pRoot->m_aabb.LenY();
	len[2] = pRoot->m_aabb.LenZ();
	
	_CreateChild(*pRoot, m_pIndex, m_indexNum, len, 0);
}

void DsAabbTree::_CreateChild(DsAabbNode& parent, const int* pIndex, int indexNum, const double* len, int depth)
{
	//const double* maxV = parent.m_aabb.m_maxV;
	//const double* minV = parent.m_aabb.m_minV;
	//DsAabb aabb[8];
	//aabb[0].Setup(DsVec3d(maxV[0] - len[0], maxV[1] - len[1], maxV[2] - len[2]), DsVec3d(minV[0]         , minV[1]         , minV[2]         ));
	//aabb[1].Setup(DsVec3d(maxV[0]         , maxV[1] - len[1], maxV[2] - len[2]), DsVec3d(minV[0] + len[0], minV[1]         , minV[2]         ));
	//aabb[2].Setup(DsVec3d(maxV[0]         , maxV[1] - len[1], maxV[2]         ), DsVec3d(minV[0] + len[0], minV[1]         , minV[2] + len[2]));
	//aabb[3].Setup(DsVec3d(maxV[0] - len[0], maxV[1] - len[1], maxV[2]         ), DsVec3d(minV[0]         , minV[1]         , minV[2] + len[2]));
	//aabb[4].Setup(DsVec3d(maxV[0] - len[0], maxV[1]         , maxV[2] - len[2]), DsVec3d(minV[0]         , minV[1] - len[1], minV[2]         ));
	//aabb[5].Setup(DsVec3d(maxV[0]         , maxV[1]         , maxV[2] - len[2]), DsVec3d(minV[0] + len[0], minV[1] - len[1], minV[2]         ));
	//aabb[6].Setup(DsVec3d(maxV[0]         , maxV[1]         , maxV[2]         ), DsVec3d(minV[0] + len[0], minV[1] - len[1], minV[2] + len[2]));
	//aabb[7].Setup(DsVec3d(maxV[0] - len[0], maxV[1]         , maxV[2]         ), DsVec3d(minV[0]         , minV[1] - len[1], minV[2] + len[2]));

	if (m_maxDepth < depth) {
		return;
	}

	int indexBuf[20000];
	for (int child = 0; child < 8; ++child) {
		int bufNum = 0;

		DsAabb childAabb;
		_DivAabb[child](childAabb, parent.m_aabb.m_maxV, parent.m_aabb.m_minV, len);
		
		for (int idx = 0; idx < indexNum; idx += 3) {
			const int val0 = pIndex[idx];
			const int val1 = pIndex[idx+1];
			const int val2 = pIndex[idx+2];

			const DsVec3d& a = m_pVertex[val0];
			const DsVec3d& b = m_pVertex[val1];
			const DsVec3d& c = m_pVertex[val2];
			DsAabb tmp;
			tmp.Setup(DsVec3d::Max(DsVec3d::Max(a, b), c), DsVec3d::Min(DsVec3d::Min(a, b), c));

			if (DsAabb::IsContain(tmp, childAabb)) {
				indexBuf[bufNum]     = val0;
				indexBuf[bufNum + 1] = val1;
				indexBuf[bufNum + 2] = val2;
				bufNum += 3;
			}
		}

		if (0 < bufNum) {
			DsAabbNode* pChild = _Create();
			if (pChild) {
				pChild->m_aabb = childAabb;
				pChild->m_parentIdx = parent.m_myIdx;
				parent.m_childIdx[child] = pChild->m_myIdx;
				const double tmpLen[3] = 
				{
					childAabb.LenX(),
					childAabb.LenY(),
					childAabb.LenZ(),
				};
				_CreateChild(*pChild, indexBuf, bufNum, tmpLen, depth+1);
			}
		}
		else {
			parent.m_childIdx[child] = -1;
		}
	}
}

DsAabbNode* DsAabbTree::_Create()
{
	DsAabbNode* ret = NULL;
	if (m_useIndex < m_treeSize) {
		ret = &m_pNodeBuf[m_useIndex];
		ret->m_myIdx = m_useIndex;
		++m_useIndex;
	}
	return ret;
}


bool DsAabbTree::Test(const DsShapeSegment& seg) const
{

	return false;
}