#pragma once

namespace DsApp
{
	class DsHitStop
	{
	public:
		DsHitStop();
		~DsHitStop();

	public:
		void RequestHitStop(double time);
		void Update(double dt);
		bool IsHitStop() const;

	private:
		double m_reqTime;

	};


}