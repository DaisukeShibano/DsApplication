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
	���̃N���X���p�����ăR���|�[�l���g���쐬����
	Update�̖߂�l��false�Ȃ玩���I�ɔj�������
	*/
	class DsComponent
	{
	public:
		virtual ~DsComponent() {};
		//!<@retval true:�p�� false:�j��
		virtual bool Update(const COMPONENT_UPDATE_ARG& arg) = 0;

	};
}

#endif