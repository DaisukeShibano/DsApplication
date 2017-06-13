#ifndef _DS_RES_PARAM_DS_RAGDOLL_PARAM_
#define _DS_RES_PARAM_DS_RAGDOLL_PARAM_

namespace DsPhysics
{
}

namespace DsPhysics
{
	struct DsRagdollParamST
	{
		double mass;
	};

	class DsRagdollParam
	{
	public:
		DsRagdollParam(int id);

	public:
		double GetMass() const { return (m_pParam) ? (m_pParam->mass) : (0.0); }

	private:
		DsRagdollParamST* m_pParam;

	};
}

#endif

