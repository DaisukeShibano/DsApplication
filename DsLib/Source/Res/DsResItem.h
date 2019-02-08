#ifndef _DS_RES_ITEM_
#define _DS_RES_ITEM_

/*
#ifndef _DS_RES_ITEM_
#include "Res/DsResItem.h"
#endif
*/

namespace DsLib
{
	class DsResource;
}

namespace DsLib
{
	class DsResItem
	{
	public:
		DsResItem() :m_resName(), m_count(0){}
		virtual ~DsResItem(){}
		virtual bool Initialize(const char* path, DsResource& resource) = 0;

	public:
		void Ref() { ++m_count; }
		void Unref() { --m_count; }
		int GetCount()const { return m_count; }

	public:
		const std::string& RefName()const { return m_resName; }
		void SetResName(const char* name) { m_resName = name; }

	private:
		std::string m_resName;
		int m_count;
		
	};
}

#endif