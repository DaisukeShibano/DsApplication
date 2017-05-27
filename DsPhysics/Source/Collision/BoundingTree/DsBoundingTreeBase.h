#ifndef __DS_BOUNDING_TREE_BASE__
#define __DS_BOUNDING_TREE_BASE__

///#ifndef __DS_BOUNDING_TREE_BASE__
///#include "Collision/BoundingTree/DsBoundingTreeBase.h"
///#endif

namespace DsPhysics
{
	class DsBoundingTreeAabb;
	class DsBoundingTreeAabbColideGroup;
}

namespace DsPhysics
{
	class DsBoundingTreeBase
	{
	public:
		virtual void ConstructTree()=0;
		virtual bool IsContain(const DsBoundingTreeBase& cmp) const = 0;
		virtual bool IsContain(const DsBoundingTreeAabb& cmp) const = 0;
		virtual bool IsContain(const DsBoundingTreeAabbColideGroup& cmp) const = 0;
		//virtual const MY_CLASS& GetMyself() const = 0;

	public:
		virtual void Draw(DsDrawCommand& com){};
	};
}

#endif