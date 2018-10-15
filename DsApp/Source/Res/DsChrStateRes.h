#pragma once

namespace DsLib
{
	class DsASNode;
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

	private:
		DsLib::DsASNode* m_pData;
	};
}
