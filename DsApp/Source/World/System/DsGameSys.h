#pragma once

namespace DsApp
{
	class DsLockOn;
	class DsHitStop;
}

namespace DsApp
{
	class DsGameSys
	{
	public:
		DsGameSys();
		virtual ~DsGameSys();

	public:
		void Initialize();
		DsLockOn* GetLockOn() const { return m_pLockOn; }
		DsHitStop* GetHitStop() const{ return m_pHitStop; }

	private:
		DsLockOn* m_pLockOn;
		DsHitStop* m_pHitStop;
	};


}