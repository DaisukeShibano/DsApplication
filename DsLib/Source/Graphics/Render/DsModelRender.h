#ifndef _DS_MODEL_RENDER_H_
#define _DS_MODEL_RENDER_H_

#ifndef _DS_TEXTURE_H_
#include "Graphics/Render/DsTexture.h"
#endif

namespace DsLib
{
	class DsModel;
	class DsShader;
}

namespace DsLib
{
	class DsModelRender
	{
	public:
		DsModelRender();
		virtual ~DsModelRender();

	public:
		void Initialize(DsShader& shader);
		void Register(const DsModel* pModel);
		void UnRegister(const DsModel* pModel);

	public:
		void Render() const;
		void RenderPolygon() const;
		void RenderNonMaterial() const;

	private:
		std::list<const DsModel*> m_drawList;
		DsTexture m_texture;
		DsShader* m_pShader;
	};
}

#endif