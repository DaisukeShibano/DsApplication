#pragma once
#include "World/Field/Action/DsChrState.h"

namespace DsApp
{
	
}

namespace DsApp
{
	
	class DsEnemyState1 : public DsChrState
	{
	public:
		DsEnemyState1(const INIT_ARG& arg)
			:DsChrState(arg)
		{}

		virtual ~DsEnemyState1()
		{}

		static void Initialize();
	};
}