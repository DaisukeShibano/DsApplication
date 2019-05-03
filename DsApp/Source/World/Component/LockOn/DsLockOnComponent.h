#pragma once

#ifndef _DS_COMPONENT_
#include "World/Component/DsComponent.h"
#endif

namespace DsApp
{
	class DsLockOnPoint;
}

namespace DsApp
{
	class DsLockOnComponent : public DsComponent
	{
	public:
		DsLockOnComponent();
		virtual ~DsLockOnComponent();

	public:
		virtual bool Update(COMPONENT_UPDATE_RESULT& result, const COMPONENT_UPDATE_ARG& arg) override;
		void Request();

	private:
		DsLockOnPoint* m_pLockOnPoint;
		bool m_isRequest;
	};
}