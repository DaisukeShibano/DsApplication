#ifndef _DS_FIELD_HIT_H_
#define _DS_FIELD_HIT_H_

#ifndef _DS_FIELD_OBJ_H_
#include "World/Field/DsFieldObj.h"
#endif

namespace DsApp
{
	class DsFieldHit : public DsFieldObj
	{
	public:
		DsFieldHit(DsLib::DsSys& sys, DsPhysics::DsPhysicsWorld& world);
		virtual ~DsFieldHit();

	public:
		virtual void Update(double dt) override;
		virtual void Initialize(const DsFieldInitInfo& initInfo) override;
		virtual void SetPosition(const DsVec3d& pos) override;
		virtual void SetRotation(const DsMat33d& rot) override;
		virtual DsVec3d GetPosition() const override;
		virtual DsMat33d GetRotation() const override;

	public:
		virtual void DbgDraw(DsLib::DsDrawCommand& com);

	private:
	};
}


#endif
