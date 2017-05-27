#ifndef _DS_LIGHT_H_
#define _DS_LIGHT_H_

namespace DsLib
{
	class DsLight
	{
	public:
		DsLight(unsigned int lightNo);
		virtual ~DsLight();

	public:
		void Enable();
		void Disable();
		virtual void SetPos(const DsVec3f& pos);
		virtual void SetDir(const DsVec3f& dir);
		const DsVec3f& GetPos() const { return m_pos; }
		const DsVec3f& GetDir() const { return m_dir; }

	protected:
		void ApplyPos(float x, float y, float z, float w);
		void ApplyDir(float x, float y, float z, float w);

	protected:
		DsVec3f m_pos;
		DsVec3f m_dir;

	private:
		const int m_lightNo;
	};
}

#endif