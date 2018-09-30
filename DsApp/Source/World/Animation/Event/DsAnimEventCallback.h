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
		void Initialize(const char* pAnimResName);
		void Call();

	private:
		void _Call(const struct DS_ANIM_ET_PARAM& param);
		void _TraceEffect(const struct DS_ANIM_ET_TRACE_EFFECT* pParam);
		void _SoundEffect(const struct DS_ANIM_ET_SOUND_EFFECT* pParam);
		void _Damage(const struct DS_ANIM_ET_DAMAGE* pParam);
		void _Cancel(const struct DS_ANIM_ET_CANCEL_ACTION_TIMING* pParam);
		void _Interpolation(const struct DS_ANIM_ET_ANIM_INTERPOLATION* pParam);
		void _ActionInput(const struct DS_ANIM_ET_ACTION_INPUT* pParam);
		void _NoHitDamage(const struct DS_ANIM_ET_NO_HIT_DAMAGE* pParam);
		void _DisableTurn(const struct DS_ANIM_ET_DISABLE_TURN* pParam);
		void _DisableGravity(const struct DS_ANIM_ET_DISABLE_GRAVITY* pParam);
		void _SuperArmor(const struct DS_ANIM_ET_SUPER_ARMOR* pParam);

	private:
		double m_preLocalTime;
		DsFieldObj& m_owner;
		DsLib::DsResource& m_resource;
		DsAnimEventTrackRes* m_pRes;
	};



}


#endif