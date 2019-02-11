#ifndef __DS_FACE__
#define __DS_FACE__

namespace DsPhysics
{
	struct DsQuad
	{
		DsQuad()
		:vn(4)
		{
		}

		int index[4];
		int vn;
		DsVec3d normal;
		DsVec3d normalOriginal;

		DsVec3d CalcCenterPos(const DsVec3d* pV ) const
		{
			DsVec3d center = DsVec3d::Zero();
			for(int idx=0; vn>idx; ++idx)
			{
				center += pV[ index[idx] ];
			}
			center /= (double)(vn);

			return center;
		}
	};
}

#endif