#pragma once


namespace DsLib
{
	struct DS_MATERIAL_PARAN_FORMAT
	{
		const int id;
		const bool isWaveNormal;
	};


	class DsMaterialParam
	{
	public:
		DsMaterialParam(int id);
		~DsMaterialParam() {}

	public:
		bool IsValid() const { return NULL != m_pParam; }

	private:
		void _Init();

	public:
		bool IsWaveNormal() const { return m_pParam ? m_pParam->isWaveNormal : false; }

	private:
		const DS_MATERIAL_PARAN_FORMAT* m_pParam;
	};

}