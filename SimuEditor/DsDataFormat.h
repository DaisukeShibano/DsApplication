#ifndef _DS_DATA_FORMAT_H_
#define _DS_DATA_FORMAT_H_

/*
#ifndef _DS_DATA_FORMAT_H_
#include "DsDataFormat.h"
#endif
*/

namespace SimuEditor {
	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	public ref struct ObjInsFormat : public System::Object
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



			, pFieldObj(0)
		{
		}
		String^ objInsName;
		String^ drawModelPath;
		String^ hitModelPath;
		double posX;
		double posY;
		double posZ;
		double angX;
		double angY;
		double angZ;
		int objType;
		int fieldObjType;




		//ÉcÅ[Éãè„Ç≈ÇµÇΩégÇÌÇ»Ç¢Ç‡ÇÃ
		void* pFieldObj;
	};
}


#endif