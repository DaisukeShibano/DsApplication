#include "DsPch.h"
#ifndef _DS_WINDOW_MANAGER_H_
#include "System/DsWindowManager.h"
#endif

#ifndef _DS_WINDOW_H_
#include "System/DsWindow.h"
#endif
#ifndef _DS_WINDOW_EZ_H_
#include "System/DsWindowEz.h"
#endif

#ifndef _DS_WINDOW_GL_H_
#include "System/DsWindowGL.h"
#endif

using namespace DsLib;

DsWindowManager::DsWindowManager()
{
}

DsWindowManager::~DsWindowManager()
{
}

//static 
DsWindow* DsWindowManager::Create(DS_WINDOW_SYSTEM_TYPE type)
{
	DsWindow* pWindow=0;

	switch (type)
	{
	case DS_WINDOW_SYSTEM_TYPE_GL:
		{
			pWindow = new DsWindowGL();
		}
		break;
	case DS_WINDOW_SYSTEM_TYPE_GLUT:
		{
			pWindow = new DsWindowEz();
		}
		break;

	default:
		pWindow = new DsWindow();
		break;
	}

	return pWindow;
}