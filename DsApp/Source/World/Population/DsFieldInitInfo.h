#ifndef _DS_FIELD_INIT_INFO_H_
#define _DS_FIELD_INIT_INFO_H_

namespace DsLib
{
	class DsHitRes;
	class DsMapRes;
	enum class DS_MAP_OBJ_TYPE : char;
	enum class DS_MAP_FIELD_OBJ_TYPE : char;
}

namespace DsApp
{
	struct DsFieldInitInfo
	{
		DsFieldInitInfo()
			: pos()
			, rot()
			, name(NULL)
			, pHitRes(NULL)
			, pAnimRes(NULL)
			, physicsType()
			, boundCoef(0.0)
		{}
		DsVec3d pos;
		DsMat33d rot;
		const char* name;
		const DsLib::DsHitRes* pHitRes;
		const DsLib::DsAnimRes* pAnimRes;
		DS_MAP_OBJ_TYPE physicsType;
		double boundCoef;
	};
}


#endif
