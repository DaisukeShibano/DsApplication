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
		static bool Test(const DsVec3d* pVertA, int vertNumA, const DsVec3d* pVertB, int vertNumB);
	};
}

#endif