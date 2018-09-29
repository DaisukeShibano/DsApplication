#include "DsPch.h"

#ifndef _DS_ANIM_CLIP_H_
#include "Animation/DsAnimClip.h"
#endif
#ifndef _DS_KEYFRAME_ANIM_
#include "Animation/DsKeyframeAnim.h"
#endif

using namespace DsLib;

static const double INTERPOLATE_TIME = 0.2;


DsAnimClip::DsAnimClip(DsKeyframeAnim& anim)
	: m_anim(anim)
	, m_isLoop(false)
	, m_isActive(false)
	, m_blendRate(0.0)
	, m_isRequestEnd(false)
	, m_localTime(0)
	, m_interpolationTime(INTERPOLATE_TIME)
{

}

DsAnimClip::~DsAnimClip()
{

}

void DsAnimClip::Activate(double dt)
{
	m_isActive = true;
	m_isRequestEnd = false;
	m_localTime = 0.0;
	m_anim.Reset();
}

void DsAnimClip::Update(double dt)
{
	if (m_isLoop)
	{
		if (m_anim.IsEnd())
		{
			m_anim.Reset();
			m_localTime = 0.0;
		}
	}
	if (m_isActive)
	{
		m_localTime += dt;

		if (0.0 < m_interpolationTime) {
			m_blendRate = min(1.0, m_blendRate + dt / (m_interpolationTime));
		}
		else {
			m_blendRate = 1.0;
		}
	}
	else
	{
		m_localTime = 0.0;
		if (0.0 < m_interpolationTime) {
			m_blendRate = max(0.0, m_blendRate - dt / (m_interpolationTime));
		}
		else {
			m_blendRate = 0.0;
		}
	}
	
	m_anim.Update(dt);
	m_isRequestEnd |= m_anim.IsEnd();
}

void DsAnimClip::Deactivate(double dt)
{
	m_isActive = false;
}

void  DsAnimClip::ResetAnim()
{
	m_anim.Reset();
}

//time[s]
void DsAnimClip::SetLocalTime(double time)
{
	m_localTime = time;
	if (0.0 < m_interpolationTime) {
		m_blendRate = min(1.0, time / (m_interpolationTime));
	}
	else {
		m_blendRate = 1.0;
	}
	m_anim.SetLocalTime(time);
	m_isRequestEnd = m_anim.IsEnd();
}

double DsAnimClip::GetLocalTime()const
{
	return m_localTime;
}

bool DsAnimClip::IsEnd() const
{
	return m_isRequestEnd ;
}

bool DsAnimClip::IsEndBlend() const
{
	return m_blendRate < 0.00001;
}

void DsAnimClip::ClearInterpolationTime()
{
	m_interpolationTime = INTERPOLATE_TIME;
}

const std::string& DsAnimClip::RefAnimName() const
{
	return RefAnim().RefName();
}