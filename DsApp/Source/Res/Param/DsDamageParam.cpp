#include "DsAppPch.h"
#include "Res/Param/DsDamageParam.h"

using namespace DsApp;

namespace
{
	static DS_DAMAGE_PARAN_FORMAT s_param[] =
	{
		//id	radius	hitEffectId	KBLen	KBTime
		{0,		0.3,	0,			0.8,	0.1}
	};


	//static‚ÌŒÅ’è’·”z—ñ‚©‚çid‚â–¼‘O‚Å‚·‚®‚ÉŽæ‚èo‚¹‚é‚æ‚¤‚É‚µ‚Ä‚¨‚­
	class DsDamageParamBank
	{
	public:
		DsDamageParamBank()
		{
			for (const DS_DAMAGE_PARAN_FORMAT& param : s_param) {
				m_idMap[param.id] = &param;
			}
		}

		const DS_DAMAGE_PARAN_FORMAT* GetById(int id) const
		{
			const DS_DAMAGE_PARAN_FORMAT* ret = NULL;
			auto it = m_idMap.find(id);
			if (it != m_idMap.end()) {
				ret = it->second;
			}
			return ret;
		}

	private:
		std::map<int, const DS_DAMAGE_PARAN_FORMAT*> m_idMap;
	};

	static DsDamageParamBank* s_pBank = NULL;
}




DsDamageParam::DsDamageParam(int id)
	:m_pParam(NULL)
{
	_Init();
	m_pParam = s_pBank->GetById(id);
}

void DsDamageParam::_Init()
{
	if (!s_pBank) {
		static DsDamageParamBank bank;
		s_pBank = &bank;
	}
}