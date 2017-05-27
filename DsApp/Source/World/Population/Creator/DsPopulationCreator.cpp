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
			DsFieldObj::InitInfo initInfo;
			initInfo.name = mapInfo.pName;
			initInfo.pos = mapInfo.pos;
			initInfo.rot = mapInfo.rot;
			initInfo.pHitRes = pHitRes;
			initInfo.pAnimRes = pAnimRes;
			initInfo.physicsType = mapInfo.objType;

			switch (mapInfo.fieldObjType)
			{
			case DS_MAP_FIELD_OBJ_TYPE::CHR:
				{
					DsFieldChr* pChr = new DsFieldChr(m_sys, world);
					pChr->Initialize(initInfo);
					m_chrs.push_back(pChr);
				}
				break;

			case DS_MAP_FIELD_OBJ_TYPE::OBJ:
				{
					DsFieldObj* pObj = new DsFieldObj(m_sys, world);
					pObj->Initialize(initInfo);
					m_objs.push_back(pObj);
				}
				break;

			case DS_MAP_FIELD_OBJ_TYPE::HIT:
				{
					DsFieldHit* pHit = new DsFieldHit(m_sys, world);
					pHit->Initialize(initInfo);
					m_hits.push_back(pHit);
				}
				break;

			case DS_MAP_FIELD_OBJ_TYPE::PLAYER:
			{
				DsFieldPlayer* pChr = new DsFieldPlayer(m_sys, world);
				pChr->Initialize(initInfo);
				m_chrs.push_back(pChr);
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