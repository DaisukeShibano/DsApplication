#pragma once

namespace DsApp
{
	class DsLockOn;
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

	private:
		DsLockOn* m_pLockOn;

	};


}