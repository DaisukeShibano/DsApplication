#ifndef _DS_CHR_
#define _DS_CHR_

namespace DsLib
{
	class DsChrUnit;
	class DsSys;
	class DsCamera;
}

namespace DsPhysics
{
	class DsChrProxy;
}

namespace DsPhysics
{
	class DsChr
	{
	public:
		DsChr();
		virtual ~DsChr();

	public:
		void Initialize(DsSys& sys);
		void Update(double dt);

	private:
		void _UpdateCam(double dt);

	private:
		DsChrUnit* m_pChrUnit;
		bool m_isInit;
		DsChrProxy* m_pProxy;
		DsCamera* m_pCam;

	public:
		void DbgDrawSkelton();
		void DbgDrawSkinMesh();
	};
}

#endif
