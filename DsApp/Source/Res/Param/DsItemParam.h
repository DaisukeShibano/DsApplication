#ifndef _DS_ITEM_PARAM_
#define _DS_ITEM_PARAM_

namespace DsApp
{
	struct DS_ITEM_PARAM_FORMAT
	{
		const char* dispName;
		const char* resName;
	};

	class DsItemParam
	{
	public:
		DsItemParam(int id);

	public:
		bool IsValid() const { return m_pParam != NULL; }
		const char* GetDispName() const { return (m_pParam) ? (m_pParam->dispName) : (""); }
		const char* GetResName() const { return (m_pParam) ? (m_pParam->resName) : (""); }

	private:
		DS_ITEM_PARAM_FORMAT* m_pParam;

	};


}

#endif