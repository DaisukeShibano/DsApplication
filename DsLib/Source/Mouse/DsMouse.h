#ifndef _DS_MOUSE_H_
#define _DS_MOUSE_H_

namespace DsLib
{
	enum DsMouseClickState : int
	{
		DS_NON_CLICK,
		DS_RIGHT_CLICK,
		DS_CENTER_CLICK,
		DS_LEFT_CLICK,
	};

	enum DsMouseUpDown
	{
		DS_DOWN_MOUSE,
		DS_UP_MOUSE,
	};

	class DsMouse
	{
	public:
		DsMouse();
		virtual ~DsMouse();

	public:
		DsMouseClickState GetClickState() const{ return m_clickState; }
		const DsVec2i& GetPos2i() const { return m_pos; }
		const DsVec3f GetPos3f() const { return DsVec3f(static_cast<float>(m_pos.x), static_cast<float>(m_pos.y), 0.0f); }
		const DsVec3d GetPos3d() const { return DsVec3d(m_pos.x, m_pos.y, 0.0); }
		const int GetWheel() const { return m_wheel; }
		const int GetWheelMove() const { return m_wheelMove; }
		const DsVec2i GetDragMove() const { return m_dragMove; }

	public:
		void Update(double dt);
		void OnDrag(const DsVec2i& pos);
		void OnClick(DsMouseClickState button, DsMouseUpDown state, int x, int y);
		void OnWheel(int wheel, int x, int y);

	private:
		DsMouseClickState m_clickState;
		DsVec2i m_pos;
		DsVec2i m_dragPos;
		DsVec2i m_dragPrePos;
		DsVec2i m_dragMove;
		int m_wheel;
		int m_wheelMove;
	};
}


#endif