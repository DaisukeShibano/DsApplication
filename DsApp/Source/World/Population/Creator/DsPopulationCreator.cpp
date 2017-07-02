#include "DsAppPch.h"
#ifndef _DS_POPULATION_CREATOR_H_
#include "World/Population/Creator/DsPopulationCreator.h"
#endif

#ifndef _DS_RESOURCE_
#include "Res/DsResource.h"
#endif
#ifndef _DS_MAP_RES_H_
#include "Res/DsMapRes.h"
#endif
#ifndef _DS_HITRES_H_
#include "Res/DsHitRes.h"
#endif
#ifndef _DS_APP_H_
#include "DsApp.h"
#endif
#ifndef _DS_ANIM_CONTROLLER_H_
#include "Animation/ActionRequest/DsAnimController.h"
#endif

using namespace DsLib;
using namespace DsPhysics;
using namespace DsApp;


DsPopulationCreator::DsPopulationCreator(DsSys& sys)
	: m_sys(sys)
	, m_hits()
	, m_objs()
	, m_chrs()
	, m_requestHits()
	, m_requestObjs()
	, m_requestChrs()
{

}

DsPopulationCreator::~DsPopulationCreator()
{
	Destoroy();
}

void DsPopulationCreator::Create(const char* resPath, DsResource& res, DsPhysicsWorld& world)
{
	const DsMapRes* pMapRes = res.RegisterMapRes(resPath);
	if (NULL == pMapRes) return;

	//配置情報で生成
	for each(const DsMapRes::Data& mapInfo in pMapRes->GetData())
	{
		const DsAnimRes* pAnimRes = res.RegisterAnimRes(mapInfo.pAnimPath);
		const DsHitRes* pHitRes = res.RegisterHitRes(mapInfo.pHitPath);

		if (pAnimRes && pHitRes)
		{
			DsFieldInitInfo reqInitInfo;
			reqInitInfo.name = mapInfo.pName;
			reqInitInfo.pos = mapInfo.pos;
			reqInitInfo.rot = mapInfo.rot;
			reqInitInfo.pHitRes = pHitRes;
			reqInitInfo.pAnimRes = pAnimRes;
			reqInitInfo.physicsType = mapInfo.objType;

			_InitInfo info;
			info.initInfo = reqInitInfo;

			switch (mapInfo.fieldObjType)
			{
			case DS_MAP_FIELD_OBJ_TYPE::CHR:
				{
					DsFieldChr* pChr = new DsFieldChr(m_sys, world);
					info.pObj = pChr;
					m_requestChrs.push_back(info);
					
				}
				break;

			case DS_MAP_FIELD_OBJ_TYPE::OBJ:
				{
					DsFieldObj* pObj = new DsFieldObj(m_sys, world);
					m_objs.push_back(pObj);
					info.pObj = pObj;
					m_requestObjs.push_back(info);
				}
				break;

			case DS_MAP_FIELD_OBJ_TYPE::HIT:
				{
					DsFieldHit* pHit = new DsFieldHit(m_sys, world);
					info.pObj = pHit;
					m_requestHits.push_back(info);
				}
				break;

			case DS_MAP_FIELD_OBJ_TYPE::PLAYER:
			{
				DsFieldPlayer* pChr = new DsFieldPlayer(m_sys, world);
				info.pObj = pChr;
				m_requestChrs.push_back(info);
			}
			break;

			default:
				DS_LOG("見知らぬFieldObjタイプ");
				break;
			}
		}
		else
		{
			DS_LOG("DsPopulationCreatorでAnimResorHitResがなかった %s %s", mapInfo.pAnimPath, mapInfo.pHitPath);
		}
	}
}

void DsPopulationCreator::Update(double dt)
{
	bool isHitOk = true;
	for (_InitInfo& iniInfo : m_requestHits) {
		if ( !iniInfo.pObj->IsRequestInit()) {
			iniInfo.pObj->Initialize(iniInfo.initInfo);
			m_hits.push_back( static_cast<DsFieldHit*>(iniInfo.pObj));
			
		}
	}
	for (DsFieldObj* pObj : m_hits) {
		if ( !pObj->IsCompleteInit() ) {
			isHitOk = false;
		}
	}

	//ヒットが準備終わってから配置物
	bool isObjOk = true;
	if (isHitOk) {
		for (_InitInfo& iniInfo : m_requestObjs) {
			if (!iniInfo.pObj->IsRequestInit()) {
				iniInfo.pObj->Initialize(iniInfo.initInfo);
				m_objs.push_back(static_cast<DsFieldObj*>(iniInfo.pObj));
			}
		}
	}
	for (DsFieldObj* pObj : m_objs) {
		if (!pObj->IsCompleteInit() ) {
			isObjOk = false;
		}
	}

	//オブジェが準備終わってから配置物
	if (isObjOk) {
		for (_InitInfo& iniInfo : m_requestChrs) {
			if (!iniInfo.pObj->IsRequestInit()) {
				iniInfo.pObj->Initialize(iniInfo.initInfo);
				m_chrs.push_back(static_cast<DsFieldChr*>(iniInfo.pObj));
			}
		}
	}


}

void DsPopulationCreator::Destoroy()
{
	for each(DsFieldHit* pHit in m_hits)
	{
		delete pHit;
	}
	m_hits.clear();

	for each(DsFieldObj* pObj in m_objs)
	{
		delete pObj;
	}
	m_objs.clear();
	
	for each(DsFieldChr* pChr in m_chrs)
	{
		delete pChr;
	}
	m_chrs.clear();
}