#pragma once


namespace DsApp
{
	struct DS_DAMAGE_PARAN_FORMAT
	{
		const int id;
		const double radius;
		const int hitEffectId;
		const double knockBackLen;
		const double knockBackTime;
	};


	class DsDamageParam
	{
	public:
		DsDamageParam(int id);
		~DsDamageParam() {}

	public:
		bool IsValid() const { return NULL != m_pParam; }

	private:
		void _Init();

	public:
		double GetRadius() const { return m_pParam ? m_pParam->radius : 0.0; }
		int GetHitEffectId() const { return m_pParam ? m_pParam->hitEffectId : 0; }
		double GetKnockBackLen() const { return m_pParam ? m_pParam->knockBackLen : 0.0; }
		double GetKnockBackTime() const { return m_pParam ? m_pParam->knockBackTime : 0.0; }

	private:
		const DS_DAMAGE_PARAN_FORMAT* m_pParam;
	};

}