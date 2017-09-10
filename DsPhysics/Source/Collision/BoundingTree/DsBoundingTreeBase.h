#ifndef __DS_BOUNDING_TREE_BASE__
#define __DS_BOUNDING_TREE_BASE__

///#ifndef __DS_BOUNDING_TREE_BASE__
///#include "Collision/BoundingTree/DsBoundingTreeBase.h"
///#endif

namespace DsLib
{
	class DsAabb;
}

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
		virtual bool IsContain(const DsBoundingTreeAabbColideGroup& cmp) const = 0;//Œ‹‹ÇŽg‚Á‚Ä‚È‚¢

	public:
		virtual bool IsContain(const DsLib::DsAabb& cmp) const = 0;


	public:
		virtual void Draw(DsDrawCommand& com)const{};
	};
}

#endif