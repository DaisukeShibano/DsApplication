#include "DsAppPch.h"
#include "Res/Param/DsChrParam.h"

using namespace DsApp;

namespace
{
	static DS_CHR_PARAN_FORMAT s_param[] =
	{
		//id	ChrName		stateGraphId
		{0,		"alice",			0},
		{1,		"enemy1",			1},
	};


	//static‚ÌŒÅ’è’·”z—ñ‚©‚çid‚â–¼‘O‚Å‚·‚®‚ÉŽæ‚èo‚¹‚é‚æ‚¤‚É‚µ‚Ä‚¨‚­
	class DsChrParamBank
	{
	public:
		DsChrParamBank()
		{
			for (const DS_CHR_PARAN_FORMAT& param : s_param) {
				m_nameMap[param.pChrName] = &param;
				m_idMap[param.id] = &param;
			}
		}

		const DS_CHR_PARAN_FORMAT* GetById(int id) const
		{
			const DS_CHR_PARAN_FORMAT* ret = NULL;
			auto it = m_idMap.find(id);
			if (it != m_idMap.end()) {
				ret = it->second;
			}
			return ret;
		}

		const DS_CHR_PARAN_FORMAT* GetByChrName(std::string name) const
		{
			const DS_CHR_PARAN_FORMAT* ret = NULL;
			auto it = m_nameMap.find(name);
			if (it != m_nameMap.end()) {
				ret = it->second;
			}
			return ret;
		}

	private:
		std::map<std::string, const DS_CHR_PARAN_FORMAT*> m_nameMap;
		std::map<int, const DS_CHR_PARAN_FORMAT*> m_idMap;
	};

	static DsChrParamBank* s_pBank = NULL;
}




DsChrParam::DsChrParam(int id)
	:m_pParam(NULL)
{
	_Init();
	m_pParam = s_pBank->GetById(id);
}

DsChrParam::DsChrParam(std::string chrName)
	:m_pParam(NULL)
{
	_Init();
	m_pParam = s_pBank->GetByChrName(chrName);
}

void DsChrParam::_Init()
{
	if (!s_pBank) {
		static DsChrParamBank bank;
		s_pBank = &bank;
	}
}