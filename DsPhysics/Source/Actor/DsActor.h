#ifndef __DS_ACTOR__
#define __DS_ACTOR__

#ifndef __DS_ACTOR_MATERIAL__
#include "Actor/DsActorMaterial.h"
#endif //__DS_ACTOR_MATERIAL__


namespace DsPhysics
{
	class DsCollisionGeometry;
	class DsCollisionResult;
	class DsActor;
	class IConstraint;
	struct DsMass;
}

namespace DsPhysics
{

	class DsActorId
	{
		friend DsActor;
	private:
		enum
		{
			INVALID = -1,
		};

	public:
		DsActorId()
			:m_pActor(NULL)
		{}
		DsActorId(DsActor* pActor) :m_pActor(pActor){}
		bool IsValid()const{ return (NULL != m_pActor); }
		bool operator ==(const DsActorId& id)const{ return (m_pActor == id.m_pActor); }
		bool operator !=(const DsActorId& id)const{ return (m_pActor != id.m_pActor); }
		const DsActor* GetActor() const { return m_pActor; }
		DsActor* GetActor() { return m_pActor; }

	private:
		void _SetActor(DsActor* actor){ m_pActor = actor; }

	private:
		DsActor* m_pActor;
	};



	class DsActor
	{
	public:
		enum ACTOR_TYPE
		{
			RIGID_BOX = 0,
			RIGID_MESH,
			RIGID_SPHERE,
			RIGID_CAPSULE,
			RAY,
		};

	public:
		struct Option
		{
			static Option Default()
			{
				Option ret;
				ret.isGravity = true;
				ret.isStatic = false;
				ret.isRotation = true;
				ret.isDrawWireFrame = false;
				return ret;
			}
			static Option Static()
			{
				Option ret;
				ret.isGravity = false;
				ret.isStatic = true;
				ret.isRotation = false;
				ret.isDrawWireFrame = false;
				return ret;
			}
			//キャラプロキシ用。キーボードなどからの入力に反応しやすいように物理挙動を制限した物体
			static Option ChrProxy()
			{
				Option ret;
				ret.isGravity = true;
				ret.isStatic = false;
				ret.isRotation = false;
				ret.isDrawWireFrame = false;
				return ret;
			}
			bool isGravity;//重力有効
			bool isStatic;//静的オブジェ
			bool isRotation;//回転するか
			//デバッグ
			bool isDrawWireFrame;
		};

	public:
		DsActor(const DsActorId& id)
			:m_option(Option::Default())
			,m_isRest(false)
			,m_isRestContagion(false)
			,m_testColMask(0)
			,m_id(id)
			,m_lifeTime(1)
			,m_dt(0.0)
			,m_pUserData(NULL)
			,m_pOctreeNodeActorNext(NULL)

			,m_dbgColor(DsVec4d::Zero())
			,m_dbgLineColor(DsVec4d::Zero())
		{
			m_id._SetActor(this);
			//m_constraints.clear();
		}

	public:
		virtual ACTOR_TYPE GetType() const = 0;

		virtual const DsCollisionGeometry* GetCollisionGeometry() const = 0;
		
		virtual void Update() = 0;

	public:
		virtual const DsVec3d& GetPosition()const;
		virtual const DsVec3d& GetVelocity()const;
		virtual const DsVec3d& GetForce()const;
		virtual const DsMat33d& GetRotation()const;
		virtual const DsVec3d& GetAngularVel()const;
		virtual const DsVec3d& GetTorque()const;
		virtual const DsMass& GetMass()const;
		virtual const double* GetMassInv10()const;//平行移動1と回転9
		virtual void AddForce(const DsVec3d& force ){};
		virtual void AddTorque(const DsVec3d& force ){};
		virtual void AddConstraintForce(const DsVec3d& f, const DsVec3d& t){ AddForce(f); AddTorque(t); }
		virtual void IntegrateConstraintForce(){};
		virtual const DsActorMaterial& RefMaterial() const { return DsActorMaterial::Null(); }

	public:
		virtual const DsVec3d& GetPositionForConstraint()const { return GetPosition(); }

		//ローカル座標変換
		//virtual DsVec3d ToLocalSpace(const DsVec3d& p) const{ return GetRotation()*p; }

		virtual const DsAabb& RefAabb() const;

	public:
		virtual DsVec3d CalcVelocity(const DsVec3d& f) const { return DsVec3d::Zero(); }

		virtual DsVec3d CalcAngularVel(const DsVec3d& t) const { return DsVec3d::Zero(); }

	public:
		virtual const char* GetName() const { static char dummy[] = ""; return dummy; }
		std::string GetNameString() const { std::string ret = GetName(); return ret; }

	public:
		virtual void SetPosition(const DsVec3d& pos){};//配置などで使う
		virtual void SetRotation(const DsMat33d& rot){};//配置などで使う
		virtual void SetVelocity(const DsVec3d& v){};//あんまり使わない想定。AddForceで。
		virtual void SetAngularVelocity(const DsVec3d& v){};//あんまり使わない想定。AddTorqueで
		virtual void AddVelocity(const DsVec3d& v){};//あんまり使わない想定。AddForceで。
		virtual void AddAngularVelocity(const DsVec3d& v){};//あんまり使わない想定。AddTorqueで
		virtual void SetForce(const DsVec3d& f){};//あんまり使わない想定。AddForceで。
		virtual void SetMaterial(const DsActorMaterial& material){}
		virtual void SetDamper(double vel, double angVel) {};
		virtual void SetInertiaBias(const DsVec3d& bias) {};

	public:
		const DsActorId& GetId() const{ return m_id;}

		long int GetLifeTime() const { return m_lifeTime; }
		void AddLifeTime(long int time){ m_lifeTime += time; }
		//実行された次のフレームで消される。
		void Kill(){ m_lifeTime = -1; }
		void SetDT( const double dt ){ m_dt = dt; }
		const Option& RefOption() const { return m_option; }
		Option& RefOption() { return m_option; }
		bool IsRest()const { return m_isRest; }
		bool IsRestContagion() const{ return m_isRestContagion; }//IsRestな物体orIsRestContagionな物体と接触したか
		void SetRestContagion(bool isRest){ m_isRestContagion = isRest; }

	public:
		void* GetUserData() { return m_pUserData; }
		const void* GetUserData() const { return m_pUserData; }
		void SetUserData(void *pData) { m_pUserData = pData; }

	public://空間分割用
		void SetOctreeNodeNext(const DsActor* pActor){ m_pOctreeNodeActorNext = pActor; }
		const DsActor* GetOctreeNodeNext() const { return m_pOctreeNodeActorNext; }

	protected:
		void SetLifeTime(long int time){ m_lifeTime = time; }
		double GetDT() const { return m_dt; }
		void SetOption(const Option& op){ m_option = op; }

	protected:
		Option m_option;
		bool m_isRest;
		bool m_isRestContagion;

	public:
		unsigned char m_testColMask;

	private:
		DsActorId m_id;
		long int m_lifeTime;
		double m_dt;
		void* m_pUserData;

	private://空間分割用。actorに持つのは微妙だけど
		const DsActor* m_pOctreeNodeActorNext;

	public:
		//デバッグ用
		virtual void Draw(DsDrawCommand& com ){};
		virtual void SetColor(const DsVec4d& color){ m_dbgColor = color; }
		virtual void SetLineColor(const DsVec4d& lineColor){ m_dbgLineColor = lineColor; }


	protected:
		DsVec4d m_dbgColor;
		DsVec4d m_dbgLineColor;
};
}
#endif