#ifndef _DS_MAP_RES_H_
#define _DS_MAP_RES_H_

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

	class DsMapRes
	{
	public:
		struct Data
		{
			const char* pName;
			const char* pAnimPath;
			const char* pHitPath;
			DsVec3d pos;
			DsMat33d rot;
			DS_MAP_OBJ_TYPE objType;
			DS_MAP_FIELD_OBJ_TYPE fieldObjType;
		};

	public:
		DsMapRes();
		virtual ~DsMapRes();

	public:
		void Initialize(const char* path);
		const std::vector<Data>& GetData() const { return m_data; }

	private:
		void* m_resTop;
		std::string m_path;
		std::vector<Data> m_data;
	};

}

#endif