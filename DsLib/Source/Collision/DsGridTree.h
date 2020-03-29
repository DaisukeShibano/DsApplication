#pragma once
#include "Collision/DsColliderUtil.h"


namespace DsLib
{
	class DsCollisionBuffer;
	struct DsGridEntity;
	struct DsGrid;
}

namespace DsLib
{

	class DsGridTree
	{
	public:
		DsGridTree();
		~DsGridTree();

	public:
		static size_t CalcMemSize(size_t entityNum);
		static DsGridTree* Create(ds_uint8* pBuf, const DsCollisionBuffer* pCollisionBuf, ds_uint8* pDbgTop, size_t dbgSize);

	public:
		void TreeUpdate();
		const float* GetMinCoord() const { return m_minP; }
		bool Test(const float start[3], const float end[3]) const;

	private:
		void _Build(const DsCollisionBuffer* pBuff);

	private:
		float m_cellUnitLen[3];
		float m_minP[3];
		const DsCollisionBuffer* m_pBuff;
		DsGrid* m_pGrids;
		DsGridEntity* m_pGridEntities;


	public:
		void DbgDraw(DsDrawCommand& draw);
	};

	

}