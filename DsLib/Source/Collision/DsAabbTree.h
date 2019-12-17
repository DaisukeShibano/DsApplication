#pragma once
#include "Collision/DsColliderUtil.h"


namespace DsLib
{
	class DsAabbNode;
	class DsCollisionBuffer;
}

namespace DsLib
{

	class DsAabbTree
	{
	public:
		DsAabbTree();
		~DsAabbTree();

	public:
		static size_t CalcMemSize();
		void Init(const float* pVetex, int vertexNum, const int* pIndex, int indexNum, ds_uint8* pBuf);
		void TreeUpdate();
		bool Test(const DsShapeSegment& seg) const;


		void Build(const DsCollisionBuffer* pBuff);

	private:
		void _CreateChild(DsAabbNode& parent, const int* pIndex, int indexNum, const float* len, int depth);
		DsAabbNode* _Create();

	private:
		DsAabbNode* m_pNodeBuf;
		const float* m_pVertex;
		const int* m_pIndex;
		const int* m_pIndexBuf;
		int m_vertexNum;
		int m_indexNum;
		int m_treeSize;
		int m_useIndex;
		int m_maxDepth;
		float m_cellUnitLen[3];
	};

	

}