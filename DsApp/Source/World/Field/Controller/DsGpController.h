#ifndef _DS_GP_CONTROLLER_H_
#define _DS_GP_CONTROLLER_H_

#ifndef _DS_CHR_CONTROLLER_H_
#include "World/Field/Controller/DsChrController.h"
#endif

namespace DsApp
{
	struct DsDecisionTreeData;
}

namespace DsApp
{
	class DsGpController : public DsChrController
	{
	public:
		DsGpController();
		virtual ~DsGpController();

	public:
		virtual void Update(double dt) override;

	private:
		

	private:
		DsDecisionTreeData* m_pGpInputData;
		DsDecisionTreeData* m_pGpOutputData;
	};




}

#endif