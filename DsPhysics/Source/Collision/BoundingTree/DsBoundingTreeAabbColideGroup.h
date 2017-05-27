#ifndef __DS_BOUNDING_TREE_AABB_COLIDE_GROUP__
#define __DS_BOUNDING_TREE_AABB_COLIDE_GROUP__

#ifndef __DS_BOUNDING_TREE_BASE__
#include "Collision/BoundingTree/DsBoundingTreeBase.h"
#endif


namespace DsPhysics
{
	class DsActor;
}

namespace DsPhysics
{
	class DsBoundingTreeAabbColideGroup : public DsBoundingTreeBase
	{
	private:
		typedef unsigned char GroupMask;
		
	public:
		typedef std::vector<const DsActor*> ActorVec;
		typedef std::map<GroupMask, ActorVec> GroupMap;

	public:
		DsBoundingTreeAabbColideGroup(const DsAabb& aabb);
		virtual ~DsBoundingTreeAabbColideGroup();

	public:
		void Initialize();
		
	private:
		virtual void ConstructTree() override;

	private:
		virtual bool IsContain(const DsBoundingTreeBase& cmp) const override{ return cmp.IsContain(*this); }
		virtual bool IsContain(const DsBoundingTreeAabb& cmp) const override{ return true; }
		virtual bool IsContain(const DsBoundingTreeAabbColideGroup& cmp) const override;

	public:
		void Update(const DsActor*const* pActors, int actorNum);
		int GetTotalBoxNum() const;
		const GroupMap& RefGroupMap() const { return m_groupMap; }

	public:
		virtual void Draw(DsDrawCommand& com)override;

	private:
		DsAabb m_aabb;
		GroupMap m_groupMap;
		
	private:
		std::vector<DsBoundingTreeAabbColideGroup*> m_child;
		char* m_pBuf;
		size_t m_useBufSize;

	};
}

#endif