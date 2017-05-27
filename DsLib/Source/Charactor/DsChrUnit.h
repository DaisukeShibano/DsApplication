#ifndef _DS_CHR_UNIT_H_
#define _DS_CHR_UNIT_H_

namespace DsLib
{
	class DsActReq;
	class DsAnimation;
	class DsSys;
	class DsDrawCommand;
	class DsAnimRes;
}

namespace DsLib
{
	class DsChrUnit
	{
	public:
		DsChrUnit(DsSys& sys, const DsAnimRes& anim);
		DsChrUnit(DsActReq* pAct, const DsAnimRes& anim);
		DsChrUnit(DsSys& sys);
		~DsChrUnit();

	public:
		void Update(double dt);
		void RegisterDraw(DsDrawCommand& com);
		void SetRootMatrix(const DsVec3d& p, const DsMat33d& r);

	public:
		const DsAnimation* GetAnimation() const { return m_pAnimation; }
		DsActReq* GetActReq() { return m_pActReq; }

	private:
		DsActReq* m_pActReq;
		DsAnimation* m_pAnimation;
	};

	
	
}

#endif