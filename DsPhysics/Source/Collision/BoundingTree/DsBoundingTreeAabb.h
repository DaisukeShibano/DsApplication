#ifndef __DS_BOUNDING_TREE_AABB__
#define __DS_BOUNDING_TREE_AABB__

#ifndef __DS_BOUNDING_TREE_BASE__
#include "Collision/BoundingTree/DsBoundingTreeBase.h"
#endif


namespace DsPhysics
{
	class DsActor;
	class DsBoundingTreeAabbColideGroup;
}

namespace DsPhysics
{
	class DsBoundingTreeAabb : public DsBoundingTreeBase
	{
	public:
		enum {DEFAULT_DEPTH=4};

	public:
		DsBoundingTreeAabb(const DsActor& actor, const DsVec3d& pos, const DsVec3d& boxSize, const DsQuad* pFaces, int faceNum, const DsVec3d* pVertex, int vertexNum, const std::vector<int>& containFaceIdxs, int maxDepth);	
		virtual ~DsBoundingTreeAabb();

	public:
		virtual void ConstructTree() override;
	private:
		void _ConstructTree(int depth);
	public:
		virtual bool IsContain(const DsBoundingTreeBase& cmp) const override
		{
			return cmp.IsContain(*this);
		}
		virtual bool IsContain(const DsBoundingTreeAabb& cmp) const override;
		virtual bool IsContain(const DsBoundingTreeAabbColideGroup& cmp) const override{ return true; }

	public:
		virtual bool IsContain(const DsLib::DsAabb& cmp) const;

	public:
		virtual void Draw(DsDrawCommand& com) const override;

	private:
		DsVec3d m_pos;
		DsVec3d m_boxSize;
		const DsActor& m_owner;
		const DsQuad* m_pFace;
		const int m_faceNum;
		const DsVec3d* m_pVertex;
		const int m_vertexNum;
		std::vector<int> m_containFaceIdxs;
		const int m_maxDepth;

	private:
		std::vector<DsBoundingTreeAabb*> m_child;

	};
}

#endif