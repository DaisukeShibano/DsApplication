#include "DsPch.h"
#ifndef _DS_MAP_RES_H_
#include "Res/DsMapRes.h"
#endif

using namespace DsLib;

namespace
{
	struct ObjInsFormat
	{
		ObjInsFormat()
			: objInsName("")
			, drawModelPath("")
			, hitModelPath("")
			, posX(0.0)
			, posY(0.0)
			, posZ(0.0)
			, angX(0.0)
			, angY(0.0)
			, angZ(0.0)
			, objType(0)
			, fieldObjType(0)
		{
		}
		std::string objInsName;
		std::string drawModelPath;
		std::string hitModelPath;
		double posX;
		double posY;
		double posZ;
		double angX;
		double angY;
		double angZ;
		int objType;
		int fieldObjType;
	};

	struct OutputRes
	{
		~OutputRes()
		{
			for each(ObjInsFormat* obj in objs)
			{
				delete obj;
			}
			objs.clear();
		}

		int version;
		std::vector<ObjInsFormat*> objs;
	};

	OutputRes* _LoadRes(const char* path)
	{
		OutputRes* pRes = new OutputRes();

		DsFile fs(path, std::ios::out | std::ios::binary);
		if (fs.IsFail())
		{
			DS_ASSERT(false, "ファイルオープンに失敗");
			return pRes;
		}

		{//version
			long ver;
			fs.Read((char*)(&ver), sizeof(long));
			pRes->version = ver;
		}

		int objNum = 0;
		{//objNum
			fs.Read((char*)(&objNum), sizeof(int));
		}

		for (int oi = 0; oi < objNum; ++oi)
		{//obj
			ObjInsFormat* obj = new ObjInsFormat();

			int tmpCharNum = 0;
			char* tmpChar = 0;

			fs.Read((char*)(&tmpCharNum), sizeof(int));
			tmpChar = new char[tmpCharNum+1];
			fs.Read(tmpChar, tmpCharNum);
			tmpChar[tmpCharNum] = '\0';
			obj->objInsName = tmpChar;
			delete[] tmpChar;

			fs.Read((char*)(&tmpCharNum), sizeof(int));
			tmpChar = new char[tmpCharNum+1];
			fs.Read(tmpChar, tmpCharNum);
			tmpChar[tmpCharNum] = '\0';
			obj->drawModelPath = tmpChar;
			delete[] tmpChar;

			fs.Read((char*)(&tmpCharNum), sizeof(int));
			tmpChar = new char[tmpCharNum+1];
			fs.Read(tmpChar, tmpCharNum);
			tmpChar[tmpCharNum] = '\0';
			obj->hitModelPath = tmpChar;
			delete[] tmpChar;

			fs.Read((char*)(&obj->posX), sizeof(double));
			fs.Read((char*)(&obj->posY), sizeof(double));
			fs.Read((char*)(&obj->posZ), sizeof(double));
			fs.Read((char*)(&obj->angX), sizeof(double));
			fs.Read((char*)(&obj->angY), sizeof(double));
			fs.Read((char*)(&obj->angZ), sizeof(double));

			fs.Read((char*)(&obj->objType), sizeof(int));
			fs.Read((char*)(&obj->fieldObjType), sizeof(int));
			
			pRes->objs.push_back(obj);
		}
		return pRes;
	}
}

DsMapRes::DsMapRes()
	: m_resTop(NULL)
	, m_path()
	, m_data()
{
}

void DsMapRes::Initialize(const char* path)
{
	m_path = path;
	OutputRes* pRes = _LoadRes(path);
	m_resTop = pRes;

	for each(const ObjInsFormat* obj in pRes->objs)
	{
		Data data;
		data.pName = obj->objInsName.c_str();
		data.pAnimPath = obj->drawModelPath.c_str();
		data.pHitPath = obj->hitModelPath.c_str();
		data.objType = static_cast<DS_MAP_OBJ_TYPE>(obj->objType);
		data.fieldObjType = static_cast<DS_MAP_FIELD_OBJ_TYPE>(obj->fieldObjType);
		data.pos = DsVec3d(obj->posX, obj->posY, obj->posZ);
		data.rot = DsMat33d::RotateX(DegToRad(obj->angX))*DsMat33d::RotateY(DegToRad(obj->angY))*DsMat33d::RotateZ(DegToRad(obj->angZ));
		m_data.push_back(data);
	}
}

DsMapRes::~DsMapRes()
{
	OutputRes* pRes = static_cast<OutputRes*>(m_resTop);
	delete pRes; 
	m_resTop = 0;
}

