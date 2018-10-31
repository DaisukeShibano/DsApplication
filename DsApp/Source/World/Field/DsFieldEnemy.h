#pragma once

#ifndef _DS_FIELD_CHR_H_
#include "World/Field/DsFieldChr.h"
#endif


namespace DsApp
{
	class DsFieldEnemy : public DsFieldChr
	{
	public:
		DsFieldEnemy(DsLib::DsSys& sys, DsPhysics::DsPhysicsWorld& world);
		virtual ~DsFieldEnemy();

	public:
		virtual void Initialize(const DsFieldInitInfo& initInfo) override;
		virtual void Update(double dt);
	};

}