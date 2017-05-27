#ifndef _DS_ANIM_MODEL_RENDER_H_
#define _DS_ANIM_MODEL_RENDER_H_

#ifndef _DS_TEXTURE_H_
#include "Graphics/Render/DsTexture.h"
#endif

namespace DsLib
{
	class DsAnimModel;
}

namespace DsLib
{
	class DsAnimModelRender
	{
	public:
		DsAnimModelRender();
		virtual ~DsAnimModelRender();

	public:
		void Register(const DsAnimModel* pModel);
		void UnRegister(const DsAnimModel* pModel);

	public:
		void Render() const;
		void RenderPolygon() const;
		void RenderNonMaterial() const;

	private:
		std::list<const DsAnimModel*> m_drawList;
		DsTexture m_texture;
	};
}

#endif