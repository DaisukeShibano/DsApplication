#include "DsPch.h"
#include "Graphics/Render/DsSceneBloom.h"
//‘¼‚Ìƒwƒbƒ_
#include "Graphics/Shader/DsShader.h"
#include "Graphics/GL/DsGLFunc.h"
#include "Graphics/Render/DsRender.h"

#include <gl/GL.h>
#include <gl/GLU.h>

using namespace DsLib;




namespace
{
	class DsSceneBloomImp : public DsSceneBloom
	{
	public:
		DsSceneBloomImp(const DsRender& ren, DsShader& shader)
			: m_render(ren)
			, m_shader(shader)
		{
			
		}

		virtual void Bloom() override
		{

		}

	public:
		virtual void DbgDraw() override
		{

		}

	private:
		const DsRender& m_render;
		DsShader& m_shader;
	};

}


//static 
DsSceneBloom* DsSceneBloom::Create(const DsRender& ren, DsShader& shader)
{
	return new DsSceneBloomImp(ren, shader);
}



