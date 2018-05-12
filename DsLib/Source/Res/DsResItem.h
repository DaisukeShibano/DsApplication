#ifndef _DS_RES_ITEM_
#define _DS_RES_ITEM_

/*
#ifndef _DS_RES_ITEM_
#include "Res/DsResItem.h"
#endif
*/

namespace DsLib
{
	class DsResItem
	{
	public:
		DsResItem() :m_resPath(), m_count(0) {}
		virtual void Initialize(const char* path) = 0;

	public:
		void Ref() { ++m_count; }
		void Unref() { --m_count; }
		int GetCount()const { return m_count; }
		const std::string& RefPath()const { return m_resPath; }
		void SetPath(const char* path) { m_resPath = path; }

	private:
		std::string m_resPath;
		int m_count;
	};


	class DsResItemFactory
	{
	public:
		virtual DsResItem* CreateIns() const = 0;
	};
}

#endif