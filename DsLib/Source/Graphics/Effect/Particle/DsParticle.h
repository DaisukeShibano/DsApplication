#ifndef _DS_PARTICLE_
#define _DS_PARTICLE_

/*
#ifndef _DS_SQUARE_PARTICLE_
#include "Graphics/Effect/Particle/DsParticle.h"
#endif
*/

namespace DsLib
{
	static const double SQUARE_PARTICLE_UV[4][2] =
	{
		{0, 0},
		{0, 1},
		{1, 1},
		{1, 0},
	};

	struct DsSquareParticle
	{
		DsSquareParticle()
			: pos()
			, speed()
			, lifeTime(0)
			, pNext(NULL)
		{}
		DsVec3d pos[4];
		//DsVec2d uvPos[4];//uv座標はインデックス値から固定
		DsVec3d speed;
		double lifeTime;
		DsSquareParticle* pNext;
	};

}

#endif