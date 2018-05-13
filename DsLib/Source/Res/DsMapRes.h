#ifndef _DS_MAP_RES_H_
#define _DS_MAP_RES_H_

#ifndef _DS_RES_ITEM_
#include "Res/DsResItem.h"
#endif

namespace DsLib
{
	class DsAnimRes;
}

namespace DsLib
{
	enum class DS_MAP_OBJ_TYPE : char
	{
		STATIC = 0,
		DYNAMIC,
		AUTONOMOUS,
		CONTROL,
	};

	enum class DS_MAP_FIELD_OBJ_TYPE : char
	{
		CHR = 0,
		OBJ,
		HIT,
		PLAYER,
	};

	class DsMapRes : public DsResItem
	{
	public:
		struct DATA
		{
			const char* pName;
			const char* pAnimPath;
			const char* pHitPath;
			DsVec3d pos;
			DsVec3d ang;
			DS_MAP_OBJ_TYPE objType;
			DS_MAP_FIELD_OBJ_TYPE fieldObjType;
		};

	public:
		DsMapRes();
		virtual ~DsMapRes();

	public:
		virtual bool Initialize(const char* path, DsResource& resource) override;
		const std::vector<DATA>& GetData() const { return m_data; }

	private:
		void* m_resTop;
		std::vector<DATA> m_data;
	};

}

#endif