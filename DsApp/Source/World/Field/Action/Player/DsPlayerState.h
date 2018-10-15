#pragma once

#ifndef _DS_CHR_STATE_
#include "World/Field/Action/DsChrState.h"
#endif

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
	};

}