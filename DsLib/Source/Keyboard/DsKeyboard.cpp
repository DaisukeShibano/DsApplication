#include "DsPch.h"
#ifndef _DS_KEYBOARD_H_
#include "Keyboard/DsKeyboard.h"
#endif

using namespace DsLib;

DsKeyboard::DsKeyboard()
{
	for each(bool& push in m_keyPush)
	{
		push = false;
	}
}

DsKeyboard::~DsKeyboard()
{

}

void DsKeyboard::OnKey(int key, bool push)
{
	switch (key)
	{
	case 'w':
	case 'W':
		m_keyPush[FORWARD] = push;
		break;

	case 's':
	case 'S':
		m_keyPush[BACK] = push;
		break;

	case 'a':
	case 'A':
		m_keyPush[LEFT] = push;
		break;

	case 'd':
	case 'D':
		m_keyPush[RIGHT] = push;
		break;
	}
}