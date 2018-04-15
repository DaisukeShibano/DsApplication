#ifndef _DS_PARTICLE_
#define _DS_PARTICLE_

/*
#ifndef _DS_SQUARE_PARTICLE_
#include "Graphics/Effect/Particle/DsParticle.h"
#endif
*/

namespace DsLib
{
	static const float SQUARE_PARTICLE_UV[4][2] =
	{
		{0, 0},
		{1, 0},
		{1, 1},
		{0, 1},
	};

	struct DsSquareParticle
	{
		DsSquareParticle()
			: pos()
			, uvPos()
			, speed()
			, lifeTime(0)
			, pNext(NULL)
		{
			for (int i = 0; i < 4; ++i) {
				uvPos[i].Set(SQUARE_PARTICLE_UV[i][0], SQUARE_PARTICLE_UV[i][1]);
			}
		}
		DsVec3d pos[4];
		DsVec2f uvPos[4];
		DsVec3d speed;
		double lifeTime;
		DsSquareParticle* pNext;
	};

}

#endif