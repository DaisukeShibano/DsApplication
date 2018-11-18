#include "DsAppPch.h"
#include "World/Field/HitStop/DsHitStop.h"
//‘¼‚Ìƒwƒbƒ_

using namespace DsApp;

DsHitStop::DsHitStop()
	: m_reqTime(0.0)
{
}

DsHitStop::~DsHitStop()
{
}


void DsHitStop::RequestHitStop(double time)
{
	m_reqTime = time;
}

void DsHitStop::Update(double dt)
{
	if (IsHitStop()) {
		m_reqTime -= dt;
	}
}

bool DsHitStop::IsHitStop() const
{
	return 0.0 < m_reqTime;
}