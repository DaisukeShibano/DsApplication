#ifndef __DS_RIGID_BOX__
#define __DS_RIGID_BOX__

#ifndef __DS_RIGID_BODY__
#include "Actor/DsRigidBody.h"
#endif


namespace DsPhysics
{
	class DsRigidBox : public DsRigidBody
	{
	private:
		enum {
			VERTEX_NUM = 8,
			FACE_NUM = 6,
			LINE_NUM = 12,
		};
	
	public:
	/////////////Factory////////////
	class DsRigidBoxFactory : public DsActorCoordFactory
	{
	public:
		DsRigidBoxFactory( const DsVec3d* pv, const double mass, const char* name )
			: DsActorCoordFactory()
			, m_pv()
			, m_sizeVertex(NULL)
			, m_mass(mass)
			, m_initOption(Option::Default())
			, m_name(name)
			, m_biasI(DsVec3d::Zero())
		{
			for (int i = 0; i < VERTEX_NUM; ++i) {
				m_pv[i] = pv[i];
			}
		}

		virtual ~DsRigidBoxFactory()
		{
		}

		virtual DsActor* CreateIns( const DsActorId& id ) const override;

	public:
		void SetOption(const Option& option){ m_initOption = option; }
		void SetBiasI(const DsVec3d& bias) { m_biasI = bias; }

	private:
		DsVec3d m_pv[VERTEX_NUM];
		DsVec3d* m_sizeVertex;
		const double m_mass;
		Option m_initOption;
		std::string m_name;
		DsVec3d m_biasI;
	};
	/////////////Factoryここまで////////////


	public:
		DsRigidBox(const DsActorId& id, const char* name);

		virtual ~DsRigidBox();

	public:
		virtual DsActor::ACTOR_TYPE GetType() const override{ return DsActor::RIGID_BOX; }

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