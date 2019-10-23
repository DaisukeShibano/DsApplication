#pragma once
#include "Collision/DsColliderUtil.h"

namespace DsLib
{
	class DsAabbNode;
}

namespace DsLib
{

	class DsAabbTree
	{
	public:
		DsAabbTree();
		~DsAabbTree();

	public:
		void Init(const DsVec3d* pVetex, int vertexNum, const int* pIndex, int indexNum);
		void TreeUpdate();
		bool Test(const DsShapeSegment& seg) const;

	private:
		void _CreateChild(DsAabbNode& parent, const int* pIndex, int indexNum, const double* len, int depth);
		DsAabbNode* _Create();

	private:
		DsAabbNode* m_pNodeBuf;
		const DsVec3d* m_pVertex;
		const int* m_pIndex;
		int m_vertexNum;
		int m_indexNum;
		int m_treeSize;
		int m_useIndex;
		int m_maxDepth;
	};

	

}