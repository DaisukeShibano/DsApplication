#ifndef _DS_ANIM_EVENT_CALLBACK_
#define _DS_ANIM_EVENT_CALLBACK_

namespace DsLib
{
	class DsResource;
	class DsAnimRes;
	class DsAnimation;
}

namespace DsApp
{
	class DsFieldObj;
	struct DS_ANIM_ET_PARAM;
	struct DS_ANIM_ET_TRACE_EFFECT;
	struct DS_ANIM_ET_DAMAGE;
}

namespace DsApp
{
	class DsAnimEventTrackRes;
}

namespace DsApp
{
	class DsAnimEventCallback
	{
	public:
		DsAnimEventCallback(DsFieldObj& owner, DsLib::DsResource& resource);
		virtual ~DsAnimEventCallback();

	public:
		void Initialize(const DsAnimRes& animRes);
		void Call();

	private:
		void _Call(const DS_ANIM_ET_PARAM& param);
		void _TraceEffect(const DS_ANIM_ET_TRACE_EFFECT* pParam);
		void _Damage(const DS_ANIM_ET_DAMAGE* pParam);

	private:
		double m_preLocalTime;
		DsFieldObj& m_owner;
		DsLib::DsResource& m_resource;
		DsAnimEventTrackRes* m_pRes;
	};



}


#endif