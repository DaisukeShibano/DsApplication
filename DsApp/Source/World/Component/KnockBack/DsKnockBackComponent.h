#pragma once

#ifndef _DS_COMPONENT_
#include "World/Component/DsComponent.h"
#endif

namespace DsApp
{
	class DsKnockBackComponent : public DsComponent
	{
	public:
		DsKnockBackComponent();
		virtual ~DsKnockBackComponent();

	public:
		virtual bool Update(const COMPONENT_UPDATE_ARG& arg) override;

	public:
		void Request(const DsVec3d& move, double time);

	private:
		bool m_isRequest;
		DsVec3d m_targetMove;
		double m_targetTime;
		double m_localTime;
	};
}
