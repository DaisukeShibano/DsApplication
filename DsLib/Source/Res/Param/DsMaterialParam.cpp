#include "DsPch.h"
#include "Res/Param/DsMaterialParam.h"

using namespace DsLib;

namespace
{
	static DS_MATERIAL_PARAN_FORMAT s_param[] =
	{
		//id	isWaveNormal
		{0,		false },
		{1,		true },
	};


	//static‚ÌŒÅ’è’·”z—ñ‚©‚çid‚â–¼‘O‚Å‚·‚®‚ÉŽæ‚èo‚¹‚é‚æ‚¤‚É‚µ‚Ä‚¨‚­
	class DsMaterialParamBank
	{
	public:
		DsMaterialParamBank()
		{
			for (const DS_MATERIAL_PARAN_FORMAT& param : s_param) {
				m_idMap[param.id] = &param;
			}
		}

		const DS_MATERIAL_PARAN_FORMAT* GetById(int id) const
		{
			const DS_MATERIAL_PARAN_FORMAT* ret = NULL;
			auto it = m_idMap.find(id);
			if (it != m_idMap.end()) {
				ret = it->second;
			}
			return ret;
		}

	private:
		std::map<int, const DS_MATERIAL_PARAN_FORMAT*> m_idMap;
	};

	static DsMaterialParamBank* s_pBank = NULL;
}




DsMaterialParam::DsMaterialParam(int id)
	:m_pParam(NULL)
{
	_Init();
	m_pParam = s_pBank->GetById(id);
}

void DsMaterialParam::_Init()
{
	if (!s_pBank) {
		static DsMaterialParamBank bank;
		s_pBank = &bank;
	}
}