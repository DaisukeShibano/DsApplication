#ifndef __DS_MASS__
#define __DS_MASS__

namespace DsPhysics
{
	struct DsMass
	{
		double mass;
		DsMat33d inertia;
		DsMat33d inertiaOriginal;

		static const DsMass& Unit()
		{
			static DsMass ret;
			ret.mass = 1.0;
			ret.inertia = DsMat33d::Identity();
			ret.inertiaOriginal = DsMat33d::Identity();
			return ret;
		}
	};
}


#endif