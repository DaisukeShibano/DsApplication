#ifndef _DS_FIELD_OBJ_H_
#define _DS_FIELD_OBJ_H_

namespace DsLib
{
	class DsHitRes;
	class DsMapRes;
	enum class DS_MAP_OBJ_TYPE : char;
	enum class DS_MAP_FIELD_OBJ_TYPE : char;
}

namespace DsApp
{
	class DsFieldObj
	{
	public:
		struct InitInfo
		{
			InitInfo()
				: pos()
				, rot()
				, name(NULL)
				, pHitRes(NULL)
				, pAnimRes(NULL)
				, physicsType()
				, boundCoef(0.0)
			{}
			DsVec3d pos;
			DsMat33d rot;
			const char* name;
			const DsLib::DsHitRes* pHitRes;
			const DsLib::DsAnimRes* pAnimRes;
			DS_MAP_OBJ_TYPE physicsType;
			double boundCoef;
		};

	public:
		DsFieldObj(DsLib::DsSys& sys, DsPhysics::DsPhysicsWorld& world);
		virtual ~DsFieldObj();

	public:
		virtual void Update(double dt);
		virtual void Initialize(const InitInfo& initInfo);
		virtual void SetPosition(const DsVec3d& pos);
		virtual void SetRotation(const DsMat33d& rot);
		virtual DsVec3d GetPosition() const;
		virtual DsMat33d GetRotation() const;

	public:
		DsPhysics::DsActor* GetActor();
		const DsPhysics::DsActor* GetActor() const;

	protected:
		virtual DsLib::DsAnimController& RefAnimController();

	protected:
		DsLib::DsSys& m_sys;
		std::string m_name;
		DsLib::DsAnimation* m_pAnimation;
		DsPhysics::DsActorId m_actorId;
		DsPhysics::DsPhysicsWorld& m_world;


	public:
		virtual void DbgDraw(DsLib::DsDrawCommand& com);

	};
}


#endif
