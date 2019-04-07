#ifndef _DS_COMPONENT_
#define _DS_COMPONENT_

#ifndef _DS_COMPONENT_SYSTEM_DEFINE_
#include "World\/Component/DsComponentSystemDefine.h"
#endif

namespace DsApp
{
	class DsFieldObj;
}

namespace DsApp
{
	/*
	このクラスを継承してコンポーネントを作成する
	Updateの戻り値がfalseなら自動的に破棄される
	*/
	class DsComponent
	{
	public:
		virtual ~DsComponent() {};
		//!<@retval true:継続 false:破棄
		virtual bool Update(const COMPONENT_UPDATE_ARG& arg) = 0;

	};
}

#endif