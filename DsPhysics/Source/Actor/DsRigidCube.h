#ifndef __DS_RIGID_CUBE__
#define __DS_RIGID_CUBE__

#ifndef __DS_RIGID_BODY__
#include "Actor/DsRigidBody.h"
#endif


namespace DsPhysics
{
	class DsRigidCube : public DsRigidBody
	{
	private:
		enum {
			VERTEX_NUM = 8,
			FACE_NUM = 6,
			LINE_NUM = 12,
		};
	
	public:
	/////////////Factory////////////
	class DsRigidCubeFactory : public DsActorFactory
	{
	public:
		DsRigidCubeFactory( const DsVec3d* pv, const double mass, const char* name )
			: m_pv()
			, m_sizeVertex(NULL)
			, m_mass(mass)
			, m_initPos(DsVec3d::Zero())
			, m_initRot(DsMat33d::Identity())
			, m_initOption(Option::Default())
			, m_name(name)
		{
			for (int i = 0; i < VERTEX_NUM; ++i) {
				m_pv[i] = pv[i];
			}
		}

		virtual ~DsRigidCubeFactory()
		{
		}

		virtual DsActor* CreateIns( const DsActorId& id ) const override;

	public:
		void InitPos(const DsVec3d& pos){m_initPos = pos;}
		void InitRot(const DsMat33d& rot){m_initRot = rot;}
		void SetOption(const Option& option){ m_initOption = option; }

	private:
		DsVec3d m_pv[VERTEX_NUM];
		DsVec3d* m_sizeVertex;
		const double m_mass;
		DsVec3d m_initPos;
		DsMat33d m_initRot;
		Option m_initOption;
		std::string m_name;
	};
	/////////////Factoryここまで////////////


	public:
		DsRigidCube(const DsActorId& id, const char* name);

		virtual ~DsRigidCube();

	public:
		virtual DsActor::ACTOR_TYPE GetType() const override{ return DsActor::RIGID_CUBE; }

	public:	
		//vertexは引数を内部で持ってるメンバにコピー
		void Create( const DsVec3d* pv, const double mass );

	public:
		//寸法からvertexを得る
		static void GetVertex( DsVec3d* pv, double xl, double yl, double zl);


	private:
		DsVec3d	m_vertex[VERTEX_NUM];
		DsVec3d	m_vertexOriginal[VERTEX_NUM];
		DsQuad	m_face[FACE_NUM];
		DsLine	m_line[LINE_NUM];



	public://デバッグ
		virtual void Draw(DsDrawCommand& com) override;
		
	};
}

#endif