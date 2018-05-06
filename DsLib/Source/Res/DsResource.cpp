#include "DsPch.h"
#ifndef _DS_RESOURCE_
#include "Res/DsResource.h"
#endif

#ifndef _DS_MAP_RES_H_
#include "Res/DsMapRes.h"
#endif
#ifndef _DS_ANIM_SET_H_
#include "Res/DsAnimRes.h"
#endif
#ifndef _DS_HITRES_H_
#include "Res/DsHitRes.h"
#endif

using namespace std;
using namespace DsLib;


//static 
string DsResource::GetExt(const std::string& path)
{
	string::size_type extPos = path.find_last_of(".");
	return path.substr(extPos, path.size() - extPos);
}

DsResource::DsResource()
	: m_animSet()
	, m_mapSet()
	, m_hitSet()
	, m_resItemSet()
{
}

DsResource::~DsResource()
{
	Finalize();
}

void DsResource::Finalize()
{
	for(auto pRes : m_animSet)
	{
		delete pRes.second;
	}
	m_animSet.clear();

	for(auto pRes : m_mapSet)
	{
		delete pRes.second;
	}
	m_mapSet.clear();

	for(auto pRes : m_hitSet)
	{
		delete pRes.second;
	}
	m_hitSet.clear();

	for (auto pRes : m_resItemSet) {
		delete pRes.second;
	}
	m_resItemSet.clear();
}

DsAnimRes* DsResource::RegisterAnimRes(const char* name)
{
	const auto find = m_animSet.find(name);
	if (find != m_animSet.end())
	{
		return find->second;
	}
	else
	{
		DsAnimRes* res = new DsAnimRes();
		DS_ASSERT(res, "ƒƒ‚ƒŠŠm•ÛŽ¸”s");
		res->Initialize(name);
		string tmpStr = name;
		m_animSet.insert(map<string, DsAnimRes*>::value_type(tmpStr, res));
		return res;
	}
}

const DsAnimRes* DsResource::GetAnimRes(const char* name) const
{
	const auto find = m_animSet.find(name);
	if (find != m_animSet.end())
	{
		return find->second;
	}
	else
	{
		return NULL;
	}
}

DsMapRes* DsResource::RegisterMapRes(const char* name)
{
	const auto find = m_mapSet.find(name);
	if (find != m_mapSet.end())
	{
		return find->second;
	}
	else
	{
		DsMapRes* res = new DsMapRes();
		DS_ASSERT(res, "ƒƒ‚ƒŠŠm•ÛŽ¸”s");
		res->Initialize(name);
		string tmpStr = name;
		m_mapSet.insert(map<string, DsMapRes*>::value_type(tmpStr, res) );
		return res;
	}
}

const DsMapRes* DsResource::GetMapRes(const char* name)const
{
	const auto find = m_mapSet.find(name);
	if (find != m_mapSet.end())
	{
		return find->second;
	}
	else
	{
		return NULL;
	}
}

DsHitRes* DsResource::RegisterHitRes(const char* name)
{
	const auto find = m_hitSet.find(name);
	if (find != m_hitSet.end())
	{
		return find->second;
	}
	else
	{
		DsHitRes* res = new DsHitRes();
		DS_ASSERT(res, "ƒƒ‚ƒŠŠm•ÛŽ¸”s");
		res->Initialize(name, *this);
		string tmpStr = name;
		m_hitSet.insert(map<string, DsHitRes*>::value_type(tmpStr, res));
		return res;
	}
}

const DsHitRes* DsResource::GetHitRes(const char* name)const
{
	const auto find = m_hitSet.find(name);
	if (find != m_hitSet.end())
	{
		return find->second;
	}
	else
	{
		return NULL;
	}
}

DsResItem* DsResource::RegisterItem(const char* name, const DsResItemFactory& factory)
{
	const auto find = m_resItemSet.find(name);
	if (find != m_resItemSet.end())
	{
		return find->second;
	}
	else
	{
		DsResItem* pItem = factory.CreateIns();
		DS_ASSERT(pItem, "ƒƒ‚ƒŠŠm•ÛŽ¸”s");
		pItem->Ref();
		pItem->SetPath(name);
		pItem->Initialize(name);
		m_resItemSet.insert(map<string, DsResItem*>::value_type(pItem->RefPath(), pItem));
		return NULL;
	}
}

void DsResource::UnregisterItem(DsResItem* pItem)
{
	if (pItem) {
		pItem->Unref();
		if (pItem->GetCount() <= 0) {
			m_resItemSet.erase(pItem->RefPath());
			delete pItem;
		}
	}
}


const DsResItem* DsResource::GetItem(const char* name)const
{
	const auto find = m_resItemSet.find(name);
	if (find != m_resItemSet.end())
	{
		return find->second;
	}
	else
	{
		return NULL;
	}
}
