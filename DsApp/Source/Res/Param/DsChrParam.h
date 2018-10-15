#pragma once


namespace DsApp
{
	struct DS_CHR_PARAN_FORMAT
	{
		const int id;
		const char* pChrName;
		const int stateId;
	};


	class DsChrParam
	{
	public:
		DsChrParam(int id);
		DsChrParam(std::string chrName);
		~DsChrParam() {}

	private:
		void _Init();

	public:
		int GetStateId() const { m_pParam ? m_pParam->stateId : -1; }

	private:
		const DS_CHR_PARAN_FORMAT* m_pParam;
	};

}