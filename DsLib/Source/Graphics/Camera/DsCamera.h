#ifndef _DS_CAMERA_H_
#define _DS_CAMERA_H_

namespace DsLib
{
	class DsMouse;
};

namespace DsLib
{
	class DsCamera
	{
	public:
		DsCamera();
		virtual ~DsCamera();

	public:
		void Update(const double dt);

	public:
		void SetAspect(float aspect){ m_aspect = aspect; }
		void SetZoom(float zoom){ m_zoom = zoom; }
		void SetPos(const DsVec3d& pos);
		void SetRot(const DsMat33d& rot);

	public:
		const double *GetCamMat16() const;
		const double* GetCamMatInv16() const;
		const DsVec3d GetPos() const { return DsVec3d::ToVec3_forMat16(m_mat); }
		const DsMat33d GetRot() const { return DsMat33d::ToMat33(m_mat); }
		const DsMat33d GetRotInv() const { return DsMat33d::ToMat33(m_matInv); }
		const DsVec3f GetUp() const { return GetRot().GetAxisY(); }
		double GetNear() const;
		double GetFar() const;
		double GetFovy() const;
		double GetAspect() const{ return m_aspect; }

	private:
		double m_mat[16];
		double m_matInv[16];
		double m_aspect;
		double m_zoom;
		DsVec3d m_pos;
	};
}

#endif