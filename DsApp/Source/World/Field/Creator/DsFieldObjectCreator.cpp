#include "DsAppPch.h"
#ifndef _DS_FIELD_OBJECT_CREATOR_H_
#include "World/Field/Creator/DsFieldObjectCreator.h"
#endif
//���̃w�b�_
#ifndef _DS_MAP_RES_H_
#include "Res/DsMapRes.h"
#endif
#ifndef _DS_APP_H_
#include "DsApp.h"
#endif

using namespace DsLib;
using namespace DsPhysics;
using namespace DsApp;


DsFieldObjectCreator::DsFieldObjectCreator(DsSys& sys)
	: m_sys(sys)
	, m_hits()
	, m_objs()
	, m_chrs()
	, m_requestHits()
	, m_requestObjs()
	, m_requestChrs()
{

}

DsFieldObjectCreator::~DsFieldObjectCreator()
{
	Destoroy();
}

void DsFieldObjectCreator::Create(const char* resPath, DsPhysicsWorld& world)
{
	const DsMapRes* pMapRes = m_sys.RefResource().RegisterItem<DsMapRes>(resPath);
	if (NULL == pMapRes) {
		return;
	}

	//�z�u���Ő���
	for (const DsMapRes::DATA& mapInfo : pMapRes->GetData())
	{


		DsFieldInitInfo reqInitInfo;
		reqInitInfo.name = mapInfo.pName;
		reqInitInfo.pos = mapInfo.pos;
		reqInitInfo.ang = mapInfo.ang;
		reqInitInfo.hitName = mapInfo.pHitPath;
		reqInitInfo.animName = mapInfo.pAnimPath;
		reqInitInfo.physicsType = mapInfo.objType;

		INIT_INFO info;
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
			DS_LOG("���m���FieldObj�^�C�v");
			break;
		}
	}
}

void DsFieldObjectCreator::Update(double dt)
{
	bool isHitOk = true;
	for (INIT_INFO& iniInfo : m_requestHits) {
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

	//�q�b�g�������I����Ă���z�u��
	bool isObjOk = true;
	if (isHitOk) {
		for (INIT_INFO& iniInfo : m_requestObjs) {
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

	//�I�u�W�F�������I����Ă���z�u��
	if (isObjOk) {
		for (INIT_INFO& iniInfo : m_requestChrs) {
			if (!iniInfo.pObj->IsRequestInit()) {
				iniInfo.pObj->Initialize(iniInfo.initInfo);
				m_chrs.push_back(static_cast<DsFieldChr*>(iniInfo.pObj));
			}
		}
	}


}

void DsFieldObjectCreator::Destoroy()
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