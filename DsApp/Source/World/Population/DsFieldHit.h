#ifndef _DS_FIELD_HIT_H_
#define _DS_FIELD_HIT_H_

#ifndef _DS_FIELD_OBJ_H_
#include "World/Population/DsFieldObj.h"
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
		virtual void SetPosition(const DsVec3d& pos);
		virtual void SetRotation(const DsMat33d& rot);
		virtual DsVec3d GetPosition() const;
		virtual DsMat33d GetRotation() const;

	public:
		virtual void DbgDraw(DsLib::DsDrawCommand& com);

	private:
	};
}


#endif
