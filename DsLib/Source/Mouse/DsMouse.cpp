#include "DsPch.h"
#ifndef _DS_MOUSE_H_
#include "Mouse/DsMouse.h"
#endif

using namespace DsLib;

DsMouse::DsMouse()
: m_clickState(DS_NON_CLICK)
, m_pos(DsVec2i(0, 0))
, m_dragPos(DsVec2i(0, 0))
, m_dragPrePos(DsVec2i(0, 0))
, m_dragMove(DsVec2i(0, 0))
, m_wheel(0)
, m_wheelMove(0)
{
}

DsMouse::~DsMouse()
{
}

void DsMouse::Update(double dt)
{
	m_dragMove = DsVec2i::Zero();
	m_wheelMove = 0;
}

void DsMouse::OnDrag(const DsVec2i& pos)
{
	m_dragPrePos = m_dragPos;
	m_dragPos = pos;
	m_dragMove = m_dragPos - m_dragPrePos;
}

void DsMouse::OnClick(DsMouseClickState button, DsMouseUpDown state, int x, int y)
{
	if (DS_DOWN_MOUSE == state)
	{
		m_pos =DsVec2i(x, y);
		m_clickState = button;
		//次にドラッグしたときに差分が出ないように。
		m_dragPos = m_pos;
		m_dragPrePos = m_pos;
	}
	else
	{
		m_clickState = DS_NON_CLICK;
	}
}

void DsMouse::OnWheel(int wheel, int x, int y)
{
	m_wheel += wheel;
	m_wheelMove = wheel;
}