#include "DsPch.h"

#ifndef _DS_ANIM_CLIP_H_
#include "Animation/DsAnimClip.h"
#endif
#ifndef _DS_KEYFRAME_ANIM_
#include "Animation/DsKeyframeAnim.h"
#endif

using namespace DsLib;

static const double BLEND_TIME = 0.2;


DsAnimClip::DsAnimClip(DsKeyframeAnim& anim)
	: m_anim(anim)
	, m_isLoop(false)
	, m_isActive(false)
	, m_blendRate(0.0)
	, m_isRequestEnd(false)
	, m_localTime(0)
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
		}
	}
	if (m_isActive)
	{
		m_localTime += dt;
		m_blendRate = min(1.0, m_blendRate + dt / (BLEND_TIME));
	}
	else
	{
		m_localTime = 0.0f;
		m_blendRate = max(0.0, m_blendRate - dt / (BLEND_TIME));
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

bool DsAnimClip::IsEnd() const
{
	return ( m_isRequestEnd && (m_blendRate < 0.00001) );
}

const std::string& DsAnimClip::RefAnimName() const
{
	return RefAnim().RefName();
}