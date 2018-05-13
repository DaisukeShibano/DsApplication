#ifndef _DS_ANIM_EVENT_TRACK_RES_
#define _DS_ANIM_EVENT_TRACK_RES_

namespace DsApp
{
	struct DS_ANIM_ET_DATA;
}

namespace DsApp
{
	class DsAnimEventTrackRes : public DsLib::DsResItem
	{
	public:
		DsAnimEventTrackRes();
		virtual ~DsAnimEventTrackRes();

	public:
		virtual bool Initialize(const char* path, DsResource& resource) override;
		const DS_ANIM_ET_DATA* GetData() const { return m_pData; }

	private:
		DS_ANIM_ET_DATA* m_pData;
	};
}

#endif