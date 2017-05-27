#ifndef _DS_CLOUD_EMITTER_H_
#define _DS_CLOUD_EMITTER_H_

#ifndef _DS_CLOUD_INS_H_
#include "Graphics/Render/Tool/Cloud/DsCloudIns.h"
#endif

namespace DsLib
{
	class DsCamera;
}

namespace DsLib
{
	class DsCloudEmitter
	{
	public:
		DsCloudEmitter(const DsCamera& cam);
		virtual ~DsCloudEmitter();

	public:
		void Initialize(unsigned int texId);
		void Update(float dt);
		void Enable();
		void Disable();

	private:
		void _SortEffects();

	private:
		std::vector<DsCloudIns*> m_clouds;
		const DsCamera& m_cam;
		bool m_isEnable;
		float m_sortTime;//[s]
	};
}

#endif