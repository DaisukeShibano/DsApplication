#pragma once

namespace DsLib
{
	
}

namespace DsApp
{
	class DsChrStateRes : public DsLib::DsResItem
	{
	public:
		DsChrStateRes();
		virtual ~DsChrStateRes();

	public:
		virtual bool Initialize(const char* path, DsLib::DsResource& resource) override;

	};
}
