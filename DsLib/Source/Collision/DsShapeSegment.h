#pragma once

namespace DsLib
{
	struct DsShapeSegment
	{
		static DsShapeSegment Setup2Point(const DsVec3f& start, const DsVec3f& end)
		{
			DsShapeSegment ret;
			const DsVec3f vec = end - start;
			ret.origin = start;
			ret.dir = DsVec3f::Normalize(vec);
			ret.len = vec.Length();
			ret.halfVec = start * (ret.len*0.5f);
			ret.halfVecFabs = DsVec3f::Abs(ret.halfVec);
			return ret;
		}

		DsVec3f origin;
		DsVec3f dir;
		DsVec3f halfVec;
		DsVec3f halfVecFabs;
		float len;
	};

}