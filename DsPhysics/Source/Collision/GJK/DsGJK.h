#ifndef __DS_GJK_H__
#define __DS_GJK_H__

namespace DsPhysics
{
	struct DsQuad;
	struct DsLine;
	class DsCollisionResult;
	class DsActorId;
	class DsCollisionContext;
}

namespace DsPhysics
{
	class DsGJK
	{
		public:
			bool CalcColide(const DsCollisionContext& convex1, const DsCollisionContext& convex2);
	};
}

#endif