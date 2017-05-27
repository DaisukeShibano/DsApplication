#ifndef __DS_COMMON_POSE__
#define __DS_COMMON_POSE__

namespace DsPhysics
{
}

namespace DsPhysics
{

	class DsPose
	{
	public:
		static DsPose Identity()
		{
			return DsPose(DsVec3d::Zero(), DsMat33d::Identity());
		}

	public:
		DsPose(const DsVec3d& pos, const DsMat33d& rot )
		:m_pos(pos)
		,m_rot(rot)
		{}

	public:
		DsVec3d& RefPos(){ return m_pos; }
		const DsVec3d& GetPos() const { return m_pos; }
		DsMat33d& RefRot(){ return m_rot; }
		const DsMat33d& GetRot()const{ return m_rot; }

	private:
		DsVec3d m_pos;
		DsMat33d m_rot;
	};
}
#endif