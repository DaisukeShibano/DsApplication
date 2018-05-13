#ifndef _DS_WEP_PARAM_
#define _DS_WEP_PARAM_

namespace DsApp
{
	struct DS_WEP_PARAM_FORMAT
	{
		const char* dispName;
		const char* resName;
		int attachDmypolyId;
		int damageId;
	};

	class DsWepParam
	{
	public:
		DsWepParam(int id);

	public:
		bool IsValid() const { return m_pParam != NULL; }
		const char* GetDispName() const { return (m_pParam) ? (m_pParam->dispName) : (""); }
		const char* GetResName() const { return (m_pParam) ? (m_pParam->resName) : (""); }
		const int GetAttachDmypolyId() const { return (m_pParam) ? (m_pParam->attachDmypolyId) : (-1); }
		const int GetDamageId() const { return (m_pParam) ? (m_pParam->damageId) : (-1); }

	private:
		DS_WEP_PARAM_FORMAT * m_pParam;

	};


}

#endif