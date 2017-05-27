#ifndef __DS_ACTOR_MATERIAL__
#define __DS_ACTOR_MATERIAL__

namespace DsPhysics
{
}

namespace DsPhysics
{
	struct DsActorMaterial
	{
		DsActorMaterial()
			: m_bundCoef(0)
			, m_staticFrictionCoef(0)
			, m_kinematicFricCoef(0)
		{}
		static const DsActorMaterial& Null();
		static const DsActorMaterial Aluminum();

		double m_bundCoef; //’µ‚Ë•Ô‚èŒW”
		double m_staticFrictionCoef;//Ã~–€CŒW”
		double m_kinematicFricCoef;//Ã~–€CŒW”

	};
}
#endif