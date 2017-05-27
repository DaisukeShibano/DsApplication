#ifndef _DS_CLOUD_INS_H_
#define _DS_CLOUD_INS_H_

namespace DsLib
{
	class  DsCloudIns
	{
	public:
		DsCloudIns( unsigned int texId );
		virtual ~DsCloudIns();

	public:
		void Initialize( const DsVec3d& pos );
		void Update(const float dt, const DsMat33d& camRot);
		bool IsDead() const;
		const DsVec3d& GetPos() const { return m_pos; }

	private:
		DsVec3d m_pos;
		DsMat33d m_rot;
		float m_size;
		const unsigned int m_texId;
		bool m_isCreate;
		float m_lifeTimer;//寿命タイマー[s]
		float m_alpha;
		float m_speefOffset;
	};

}

#endif