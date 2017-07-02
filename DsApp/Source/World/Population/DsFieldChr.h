#ifndef _DS_FIELD_CHR_H_
#define _DS_FIELD_CHR_H_

#ifndef _DS_FIELD_OBJ_H_
#include "World/Population/DsFieldObj.h"
#endif

namespace DsLib
{
	class DsHitRes;
}

namespace DsApp
{
	class DsFieldChr : public DsFieldObj
	{
	public:
		DsFieldChr(DsLib::DsSys& sys, DsPhysics::DsPhysicsWorld& world);
		virtual ~DsFieldChr();

	public:
		virtual void Update(double dt);
		virtual void Initialize(const DsFieldInitInfo& initInfo);
		virtual DsVec3d GetPosition() const override;
		virtual DsMat33d GetRotation() const override;
		virtual void SetPosition(const DsVec3d& pos) override;
		virtual void SetRotation(const DsMat33d& rot) override;

	public:
		virtual void DbgDraw(DsLib::DsDrawCommand& com);

	protected:
		DsVec3d m_vel;
		DsVec3d m_ang;

	};
}


#endif
