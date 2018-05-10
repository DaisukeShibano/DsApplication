#include "DsPch.h"
#ifndef _DS_KEYBOARD_H_
#include "Keyboard/DsKeyboard.h"
#endif

using namespace DsLib;

DsKeyboard::DsKeyboard()
	:m_keyPush{}
{
}

DsKeyboard::~DsKeyboard()
{

}

void DsKeyboard::OnKey(ds_uint8 key, bool push)
{
	m_keyPush[key] = push;
}

void DsKeyboard::OnKeySp(int key, bool push)
{

}