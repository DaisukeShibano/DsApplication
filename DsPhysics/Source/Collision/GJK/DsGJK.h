#ifndef __DS_GJK_H__
#define __DS_GJK_H__

namespace DsPhysics
{
	struct DsQuad;
	struct DsLine;
	class DsCollisionResult;
	class DsActorId;
	class DsCollisionGeometry;
}

namespace DsPhysics
{
	class DsGJK
	{
		public:
			bool CalcColide(const DsCollisionGeometry& convex1, const DsCollisionGeometry& convex2);
	};
}

#endif