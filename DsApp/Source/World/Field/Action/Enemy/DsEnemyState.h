#pragma once
#include "World/Field/Action/DsChrState.h"

namespace DsApp
{
	
}

namespace DsApp
{
	
	class DsEnemyState : public DsChrState
	{
	public:
		DsEnemyState(const INIT_ARG& arg)
			:DsChrState(arg)
		{}

		virtual ~DsEnemyState()
		{}
	};
}