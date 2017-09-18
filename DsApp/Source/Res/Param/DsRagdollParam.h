#ifndef _DS_RES_PARAM_DS_RAGDOLL_PARAM_
#define _DS_RES_PARAM_DS_RAGDOLL_PARAM_


namespace DsApp
{
	enum class INSIDE_COLLISION_GROUP : unsigned int;
}

namespace DsApp
{
	enum DS_RAGDOLL_PARAM_ANIM_TYPE : int
	{
		KEYFRAME,		//キーフレームアニメ
		PHYSICS,		//物理
	};


	struct DsRagdollParamST
	{
		double mass;
		int animType; //DS_RAGDOLL_PARAM_ANIM_TYPE
		double damperV;
		double damperA;
		unsigned int collisionGroup; //INSIDE_COLLISION_GROUP
	};

	class DsRagdollParam
	{
	public:
		DsRagdollParam(int id);

	public:
		double GetMass() const { return (m_pParam) ? (m_pParam->mass) : (0.0); }
		
		DS_RAGDOLL_PARAM_ANIM_TYPE GetAnimType() const { return static_cast<DS_RAGDOLL_PARAM_ANIM_TYPE>( (m_pParam) ? (m_pParam->animType) : (0) );  }

		double GetDamperV() const { return (m_pParam) ? (m_pParam->damperV) : (0.0); }
		double GetDamperA() const { return (m_pParam) ? (m_pParam->damperA) : (0.0); }
		INSIDE_COLLISION_GROUP GetCollisionGroup() const { return  static_cast<INSIDE_COLLISION_GROUP>( (m_pParam) ? (m_pParam->collisionGroup) : (0) ); }

	private:
		DsRagdollParamST* m_pParam;

	};
}

#endif

