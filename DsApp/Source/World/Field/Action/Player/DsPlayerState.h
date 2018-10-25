#pragma once
#include "World/Field/Action/DsChrState.h"

namespace DsApp
{
	class DsPlayerState : public DsChrState
	{
	public:
		DsPlayerState(const INIT_ARG& arg)
			:DsChrState(arg)
		{}

		virtual ~DsPlayerState()
		{}

		static void Initialize();
	};

}