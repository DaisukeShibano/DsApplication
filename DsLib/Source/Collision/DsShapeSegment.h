#pragma once

namespace DsLib
{
	struct DsShapeSegment
	{
		static DsShapeSegment Setup2Point(const DsVec3d& start, const DsVec3d& end)
		{
			DsShapeSegment ret;
			const DsVec3d vec = end - start;
			ret.origin = start;
			ret.dir = DsVec3d::Normalize(vec);
			ret.len = vec.Length();
			ret.halfVec = start * (ret.len*0.5);
			ret.halfVecFabs = DsVec3d::Abs(ret.halfVec);
			return ret;
		}

		DsVec3d origin;
		DsVec3d dir;
		DsVec3d halfVec;
		DsVec3d halfVecFabs;
		double len;
	};

}