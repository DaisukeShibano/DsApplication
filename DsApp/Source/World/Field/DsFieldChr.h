#ifndef _DS_FIELD_CHR_H_
#define _DS_FIELD_CHR_H_

#ifndef _DS_FIELD_OBJ_H_
#include "World/Field/DsFieldObj.h"
#endif

namespace DsLib
{
	class DsHitRes;
}

namespace DsPhysics
{
	class DsRagdoll;
}

namespace DsApp
{
	class DsAnimRagdollModifier;
	class DsActionRequest;
	class DsActionCtrl;
}

namespace DsApp
{
	class DsFieldChr : public DsFieldObj
	{
	public:
		DsFieldChr(DsLib::DsSys& sys, DsPhysics::DsPhysicsWorld& world, DsLib::DsResource& resource);
		virtual ~DsFieldChr();

	public:
		virtual void Update(double dt);

	public:
		virtual void Initialize(const DsFieldInitInfo& initInfo);
	private:
		virtual void _SetActorCoord(DsPhysics::DsActorCoordFactory& factory, const DsFieldInitInfo& initInfo) override;

	protected:
		virtual DsActionRequest* _CreareActionRequest();

	public:
		virtual DsVec3d GetPosition() const override;
		virtual DsMat33d GetRotation() const override;
		virtual void SetPosition(const DsVec3d& pos) override;
		virtual void SetRotation(const DsMat33d& rot) override;

	protected:
		DsVec3d m_vel;
		DsVec3d m_ang;
		DsPhysics::DsRagdoll* m_pRagdoll;
		DsAnimRagdollModifier* m_pAnimRagdollModifier;
		DsActionRequest* m_pActReq;
		DsActionCtrl* m_pActCtrl;

	public://デバッグ
		virtual void DbgDraw(DsLib::DsDrawCommand& com);

	};
}


#endif
