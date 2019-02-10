#ifndef _DS_DRAW_COMMAND_H_
#define _DS_DRAW_COMMAND_H_

#ifndef _DS_MODEL_RENDER_H_
#include "Graphics/Render/DsModelRender.h"
#endif

namespace DsLib
{
	class DsRender;
}

namespace DsLib
{
	class DsDrawComBase
	{
	protected:
		typedef std::map<unsigned char*, unsigned int> TexIdMap;

	public:
		virtual void Exe(DsDrawCommand& owner) = NULL;
	};

	class DsDrawCommand
	{
	private:
		enum
		{
			BUFFER_SIZE = 32 * 1024 * 1024,
		};

	public:
		static DsDrawCommand& Create(DsModelRender& modelRender, DsRender& render);

	private:
		DsDrawCommand(DsModelRender& modelRender, DsRender& render);

	public:
		virtual ~DsDrawCommand();

	public:
		void Exe();
		void Clear();

	public:
		DsDrawCommand& DrawBox(const DsVec3f& pos, const float size);
		DsDrawCommand& DrawBox(const DsVec3d& pos, const double size){ return DrawBox(ToVec3f(pos), static_cast<float>(size)); }
		DsDrawCommand& DrawSphere(const DsVec3f& pos, const float radius);
		DsDrawCommand& DrawSphere(const DsVec3d& pos, const double radius){ return DrawSphere(ToVec3f(pos), static_cast<float>(radius)); }
		DsDrawCommand& DrawLine(const DsVec3f& p1, const DsVec3f& p2);
		DsDrawCommand& DrawLine(const DsVec3d& p1, const DsVec3d& p2){ return DrawLine(ToVec3f(p1), ToVec3f(p2)); };
		DsDrawCommand& DrawCapsule(const DsVec3f& p1, const DsVec3f& p2, const float radius);
		DsDrawCommand& DrawCapsule(const DsVec3d& p1, const DsVec3d& p2, const double radius){ return DrawCapsule(ToVec3f(p1), ToVec3f(p2), static_cast<float>(radius)); };
		DsDrawCommand& DrawPolygon(const DsVec3f& p1, const DsVec3f& p2, const DsVec3f& p3);
		DsDrawCommand& DrawPolygon(const DsVec3d& p1, const DsVec3d& p2, const DsVec3d& p3){ return DrawPolygon(ToVec3f(p1), ToVec3f(p2), ToVec3f(p3)); }
		DsDrawCommand& DrawPolygon(const DsVec3f* p, const int pn);
		DsDrawCommand& DrawPolygon(const DsVec3d* p, const int pn);
		DsDrawCommand& DrawQuad(const DsVec3f& p1, const DsVec3f& p2, const DsVec3f& p3, const DsVec3f& p4);
		DsDrawCommand& DrawQuad(const DsVec3d& p1, const DsVec3d& p2, const DsVec3d& p3, const DsVec3d& p4){ return DrawQuad(ToVec3f(p1), ToVec3f(p2), ToVec3f(p3), ToVec3f(p4)); }
		DsDrawCommand& DrawTexQuad(const DsVec3d& p1, const DsVec3d& p2, const DsVec3d& p3, const DsVec3d& p4,
			const DsVec2d& t1, const DsVec2d& t2, const DsVec2d& t3, const DsVec2d& t4,
			const unsigned char* pImage, int w, int h);
		DsDrawCommand& DrawAxis(const DsMat33f& m, const DsVec3f& pos, const float len = 1.0f, const float width = 1.0f);
		DsDrawCommand& DrawAxis(const DsMat33d& m, const DsVec3d& pos, const double len = 1.0f, const double width = 1.0f){ return DrawAxis(ToMat33f(m), ToVec3f(pos), static_cast<float>(len), static_cast<float>(width)); }
		DsDrawCommand& DrawAxis(const DsMat44f& m, const float len = 1.0f, const float width = 1.0f){ return DrawAxis(m.ToMat33(), m.GetPos(), len, width); }
		DsDrawCommand& DrawAxis(const DsMat44d& m, const double len = 1.0f, const double width = 1.0f){ return DrawAxis(ToMat33f(m.ToMat33()), ToVec3f(m.GetPos()), static_cast<float>(len), static_cast<float>(width)); }
		DsDrawCommand& SetColor(const DsVec3f& color);
		DsDrawCommand& SetColor(const DsVec3d& color){ return SetColor(ToVec3f(color)); }
		DsDrawCommand& SetColor(float r, float g, float b){ return SetColor(DsVec3d(r, g, b)); }
		DsDrawCommand& SetColor(double r, double g, double b){ return SetColor(ToVec3f(DsVec3d(r,g,b))); }
		DsDrawCommand& DrawText(const DsVec3d& pos, const wchar_t* pText, ...);
		DsDrawCommand& DrawTextScreen(const DsVec2d& pos, const wchar_t* pText, ...);//座標は上下で-0.5,0.5の範囲

	public:
		DsModelRender& RefModelRender() { return m_modelRender; }
		DsRender& RefRender() { return m_render; }

	private:
		std::vector<DsDrawComBase*> m_coms;
		char* m_pBuffer;
		size_t m_useMemory;
		DsModelRender& m_modelRender;
		DsRender& m_render;
		unsigned int m_dummyTexId;
		std::map< const unsigned char*, unsigned int> m_texIdMap;

	private://フォント
		ds_uint64 m_hFont;

	};


}

#endif