#ifndef _DS_SIMU_H_
#define _DS_SIMU_H_

#ifndef _DS_APP_H_
#include "DsApp.h"
#endif

#include "System/DsWindowGL.h"
#include "System/DsMainLoop.h"
#include "Graphics/Render/DsRender.h"
#include "Graphics/Camera/DsCamera.h"
#ifndef _DS_MOUSE_H_
#include "Mouse/DsMouse.h"
#endif
#ifndef _DS_MAP_RES_H_
#include "Res/DsMapRes.h"
#endif


class DsSimu
{
public:
	static void Initialize();
	static void Finalize();
	static DsSimu* GetIns();

public:
	DsSimu();
	~DsSimu();

public:
	void Setup(ds_uint64 handle);
	DsWindowGL& RefWindow();
	bool IsInit() const { return m_isInit; }
	void Update(double dt);
	DsApp::DsFieldObj* RegisterObj(const char* drawModelPath, const char* hitModelPath, double px, double py, double pz, double rx, double ry, double rz, DS_MAP_OBJ_TYPE physicsType, DS_MAP_FIELD_OBJ_TYPE fieldObjType);
	void Unregister(DsApp::DsFieldObj* pObj);
	void ClearObj();

private:
	DsSys* m_pSys;
	DsMainLoop* m_pLoop;
	bool m_isInit;
	std::list<DsApp::DsFieldObj*> m_fieldObjs;
	DsResource m_resource;
};


#endif