#pragma once

#ifndef _DS_FIELD_CHR_H_
#include "World/Field/DsFieldChr.h"
#endif


namespace DsApp
{
	class DsFieldEnemy : public DsFieldChr
	{
	public:
		virtual void Initialize(const DsFieldInitInfo& initInfo) override;
	};

}