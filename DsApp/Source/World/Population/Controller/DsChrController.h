#ifndef _DS_CHR_CONTROLLER_H_
#define _DS_CHR_CONTROLLER_H_

namespace DsApp
{
	class DsChrController
	{
	public:
		DsChrController();
		virtual ~DsChrController();

	public:
		virtual void Update(double dt) = 0;

	private:


	};


}

#endif