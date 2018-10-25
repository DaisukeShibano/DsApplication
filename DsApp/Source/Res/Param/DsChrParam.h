#pragma once


namespace DsApp
{
	struct DS_CHR_PARAN_FORMAT
	{
		const int id;
		const char* pChrName;
		const int stateGraphId;
	};


	class DsChrParam
	{
	public:
		DsChrParam(int id);
		DsChrParam(std::string chrName);
		~DsChrParam() {}

	public:
		bool IsValid() const { return NULL != m_pParam; }

	private:
		void _Init();

	public:
		int GetStateGraphId() const { return m_pParam ? m_pParam->stateGraphId : -1; }

	private:
		const DS_CHR_PARAN_FORMAT* m_pParam;
	};

}