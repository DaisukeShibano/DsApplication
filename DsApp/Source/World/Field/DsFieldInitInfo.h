#ifndef _DS_FIELD_INIT_INFO_H_
#define _DS_FIELD_INIT_INFO_H_

namespace DsLib
{
	enum class DS_MAP_OBJ_TYPE : char;
	enum class DS_MAP_FIELD_OBJ_TYPE : char;
}

namespace DsApp
{
	struct DsFieldInitInfo
	{
		DsFieldInitInfo()
			: pos()
			, ang()
			, name(NULL)
			, hitName()
			, animName()
			, physicsType()
			, boundCoef(0.0)
		{}
		DsVec3d pos;
		DsVec3d ang;
		const char* name;
		std::string hitName;
		std::string animName;
		DS_MAP_OBJ_TYPE physicsType;
		double boundCoef;
	};
}


#endif
