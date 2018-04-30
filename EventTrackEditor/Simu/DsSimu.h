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
	std::vector<std::string> RegisterObj(const char* drawModelPath, const char* hitModelPath, double px, double py, double pz, double rx, double ry, double rz, DS_MAP_OBJ_TYPE physicsType, DS_MAP_FIELD_OBJ_TYPE fieldObjType);
	void ClearObj();

public:
	void SetLocalTimeAnim(double time);
	void SetCurrentAnimName(const char* name);
	std::string GetCurrentAnimName()const;
	bool IsEndAnim()const;

private:
	DsSys* m_pSys;
	DsMainLoop* m_pLoop;
	std::list<DsApp::DsFieldObj*> m_fieldObjs;
	std::string m_selectAnimName;
	DsResource m_resource;
	DsApp::DsFieldObj* m_pChrIns;
	bool m_isInit;
};


#endif