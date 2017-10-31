#ifndef _DS_MATRIX44_H_
#define _DS_MATRIX44_H_

#ifndef _DS_MATRIX44D_H_
#include "Math/DsMatrix44d.h"
#endif

#ifndef _DS_MATRIX44F_H_
#include "Math/DsMatrix44f.h"
#endif


namespace DsLib
{
	template<typename TYPE>
	struct DsMat44
	{
		union
		{
			TYPE mat[16];
			struct
			{
				TYPE m00, m01, m02, m03;
				TYPE m10, m11, m12, m13;
				TYPE m20, m21, m22, m23;
				TYPE m30, m31, m32, m33;
			};
			struct
			{
				DsVec4<TYPE> v[4];
			};
		};

		//void operator =(const DsMat44<TYPE>& src)
		//{
		//	for (int i = 0; 16>i; ++i)
		//	{
		//		mat[i] = src.mat[i];
		//	}
		//}

		DsVec4<TYPE>& operator[](const int index)
		{
			assert((index < 4) && "indexの値がサイズを超えています\n");
			return v[index];
		}

		const DsVec4<TYPE>& operator[](const int index) const
		{
			assert((index < 4) && "indexの値がサイズを超えています\n");
			return v[index];
		}

		DsMat44<TYPE> operator *(const DsMat44<TYPE>& src) const
		{
			DsMat44<TYPE> dst =
			{
				m00*src.m00 + m01*src.m10 + m02*src.m20 + m03*src.m30, m00*src.m01 + m01*src.m11 + m02*src.m21 + m03*src.m31, m00*src.m02 + m01*src.m12 + m02*src.m22 + m03*src.m32, m00*src.m03 + m01*src.m13 + m02*src.m23 + m03*src.m33,
				m10*src.m00 + m11*src.m10 + m12*src.m20 + m13*src.m30, m10*src.m01 + m11*src.m11 + m12*src.m21 + m13*src.m31, m10*src.m02 + m11*src.m12 + m12*src.m22 + m13*src.m32, m10*src.m03 + m11*src.m13 + m12*src.m23 + m13*src.m33,
				m20*src.m00 + m21*src.m10 + m22*src.m20 + m23*src.m30, m20*src.m01 + m21*src.m11 + m22*src.m21 + m23*src.m31, m20*src.m02 + m21*src.m12 + m22*src.m22 + m23*src.m32, m20*src.m03 + m21*src.m13 + m22*src.m23 + m23*src.m33,
				m30*src.m00 + m31*src.m10 + m32*src.m20 + m33*src.m30, m30*src.m01 + m31*src.m11 + m32*src.m21 + m33*src.m31, m30*src.m02 + m31*src.m12 + m32*src.m22 + m33*src.m32, m30*src.m03 + m31*src.m13 + m32*src.m23 + m33*src.m33,
			};
			return dst;
		}

		DsVec4<TYPE> operator *(const DsVec4<TYPE>& src) const
		{
			DsVec4<TYPE> dst =
			{
				m00*src.x + m01*src.y + m02*src.z + m03*src.w,
				m10*src.x + m11*src.y + m12*src.z + m13*src.w,
				m20*src.x + m21*src.y + m22*src.z + m23*src.w,
				m30*src.x + m31*src.y + m32*src.z + m33*src.w,
			};
			return dst;
		}

		//回転成分だけ掛ける
		DsVec3<TYPE> MultiRot(const DsVec3<TYPE>& src) const
		{
			DsVec3<TYPE> dst =
			{
				m00*src.x + m01*src.y + m02*src.z,
				m10*src.x + m11*src.y + m12*src.z,
				m20*src.x + m21*src.y + m22*src.z,
			};
			return dst;
		}

		DsMat44<TYPE> operator +(const DsMat44<TYPE>& src) const
		{
			DsMat44<TYPE> dst =
			{
				m00 + src.m00, m01 + src.m01, m02 + src.m02, m03 + src.m03,
				m10 + src.m10, m11 + src.m11, m12 + src.m12, m13 + src.m13,
				m20 + src.m20, m21 + src.m21, m22 + src.m22, m23 + src.m23,
				m30 + src.m30, m31 + src.m31, m32 + src.m32, m33 + src.m33,
			};
			return dst;
		}

		DsMat44<TYPE> operator -(const DsMat44<TYPE>& src) const
		{
			DsMat44<TYPE> dst =
			{
				m00 - src.m00, m01 - src.m01, m02 - src.m02, m03 - src.m03,
				m10 - src.m10, m11 - src.m11, m12 - src.m12, m13 - src.m13,
				m20 - src.m20, m21 - src.m21, m22 - src.m22, m23 - src.m23,
				m30 - src.m30, m31 - src.m31, m32 - src.m32, m33 - src.m33,
			};
			return dst;
		}

		DsMat44<TYPE>& operator +=(const DsMat44<TYPE>& src)
		{
			m00 += src.m00; m01 += src.m01; m02 += src.m02; m03 += src.m03;
			m10 += src.m10; m11 += src.m11; m12 += src.m12; m13 += src.m13;
			m20 += src.m20; m21 += src.m21; m22 += src.m22; m23 += src.m23;
			m30 += src.m30; m31 += src.m31; m32 += src.m32; m33 += src.m33;
			return (*this);
		}

		DsMat44<TYPE>& operator -=(const DsMat44<TYPE>& src)
		{
			m00 -= src.m00; m01 -= src.m01; m02 -= src.m02; m03 -= src.m03;
			m10 -= src.m10; m11 -= src.m11; m12 -= src.m12; m13 -= src.m13;
			m20 -= src.m20; m21 -= src.m21; m22 -= src.m22; m23 -= src.m23;
			m30 -= src.m30; m31 -= src.m31; m32 -= src.m32; m33 -= src.m33;
			return (*this);
		}

		DsMat44<TYPE>& operator =(const double src[4][4] )
		{
			m00 = src[0][0]; m01 = src[0][1]; m02 = src[0][2]; m03 = src[0][3];
			m10 = src[1][0]; m11 = src[1][1]; m12 = src[1][2]; m13 = src[1][3];
			m20 = src[2][0]; m21 = src[2][1]; m22 = src[2][2]; m23 = src[2][3];
			m30 = src[3][0]; m31 = src[3][1]; m32 = src[3][2]; m33 = src[3][3];
			return (*this);
		}
		DsMat44<TYPE>& operator =(const float src[4][4])
		{
			m00 = src[0][0]; m01 = src[0][1]; m02 = src[0][2]; m03 = src[0][3];
			m10 = src[1][0]; m11 = src[1][1]; m12 = src[1][2]; m13 = src[1][3];
			m20 = src[2][0]; m21 = src[2][1]; m22 = src[2][2]; m23 = src[2][3];
			m30 = src[3][0]; m31 = src[3][1]; m32 = src[3][2]; m33 = src[3][3];
			return (*this);
		}

		DsMat44<TYPE>& SetRotation(const DsMat33<TYPE>& src)
		{
			m00 = src.m00; m01 = src.m01; m02 = src.m02;
			m10 = src.m10; m11 = src.m11; m12 = src.m12;
			m20 = src.m20; m21 = src.m21; m22 = src.m22;
			return (*this);
		}


		DsMat33<TYPE> ToMat33() const
		{
			const DsMat33<TYPE> dst=
			{
				m00, m01, m02,
				m10, m11, m12,
				m20, m21, m22,
			};
			return dst;
		}

		DsMat44<TYPE> ToTransposition() const
		{
			const DsMat44<TYPE> dst =
			{
				m00, m10, m20, m30,
				m01, m11, m21, m31,
				m02, m12, m22, m32,
				m03, m13, m23, m33,
			};
			return dst;
		}

		bool IsNearEqual(const DsMat44<TYPE>& src) const
		{
			const TYPE err = static_cast<TYPE>(0.0000001);
			for (int i = 0; i < 16; ++i)
			{
				const TYPE sub = mat[i] - src.mat[i];
				if ((sub > err) || (sub < -err))
				{
					return false;
				}
			}
			return true;
		}

		DsVec3<TYPE> GetAxisX() const
		{
			DsVec3<TYPE> dst = { m00, m10, m20 };
			return dst;
		}

		DsVec3<TYPE> GetAxisY() const
		{
			DsVec3<TYPE> dst = { m01, m11, m21 };
			return dst;
		}

		DsVec3<TYPE> GetAxisZ() const
		{
			DsVec3<TYPE> dst = { m02, m12, m22 };
			return dst;
		}

		DsVec3<TYPE> GetPos() const
		{
			DsVec3<TYPE> dst = { m03, m13, m23 };
			return dst;
		}

		DsVec3<TYPE> GetPosT() const
		{
			DsVec3<TYPE> dst = { m30, m31, m32 };
			return dst;
		}

		static DsMat44<TYPE> SetAxis(const DsVec3<TYPE>& x, const DsVec3<TYPE>& y, const DsVec3<TYPE>& z)
		{
			DsMat44<TYPE> dst =
			{
				x.x, y.x, z.x, 0,
				x.y, y.y, z.y, 0,
				x.z, y.z, z.z, 0,
				0, 0, 0, 1,
			};
			return dst;
		}


		static DsMat44<TYPE> Identity()
		{
			DsMat44<TYPE> dst =
			{
				1, 0, 0, 0,
				0, 1, 0, 0,
				0, 0, 1, 0,
				0, 0, 0, 1,
			};
			return dst;
		}

		static DsMat44<TYPE> Inverse(const DsMat44<TYPE>& src)
		{
			DS_ASSERT(false, "未実装");
			return DsMat44<TYPE>();
		}

		static DsMat44<TYPE> RotateX(TYPE ang)
		{
			const DsMat44<TYPE> dst =
			{
				1, 0, 0, 0,
				0, cos(ang), -sin(ang), 0,
				0, sin(ang), cos(ang), 0,
				0, 0, 0, 1,
			};
			return dst;
		}

		static DsMat44<TYPE> RotateY(TYPE ang)
		{
			const DsMat44<TYPE> dst =
			{
				cos(ang), 0, sin(ang), 0,
				0, 1, 0, 0,
				-sin(ang), 0, cos(ang), 0,
				0, 0, 0, 1,
			};
			return dst;
		}

		static DsMat44<TYPE> RotateZ(TYPE ang)
		{
			const DsMat44<TYPE> dst =
			{
				cos(ang), -sin(ang), 0, 0,
				sin(ang), cos(ang), 0, 0,
				0, 0, 1, 0,
				0, 0, 0, 1, 
			};
			return dst;
		}

		static DsMat44<TYPE> RotateVec(const DsVec3<TYPE>& vec)
		{
			const DsVec3<TYPE> n = DsVec3<TYPE>::Normalize(vec);
			const TYPE r = vec.Length();
			const DsMat44<TYPE> dst =
			{
				static_cast<TYPE>(n.x*n.x*(1.0 - cos(r)) + cos(r)), static_cast<TYPE>(n.x*n.y*(1.0 - cos(r)) - n.z*sin(r)), static_cast<TYPE>(n.z*n.x*(1.0 - cos(r)) + n.y*sin(r)), 0,
				static_cast<TYPE>(n.x*n.y*(1.0 - cos(r)) + n.z*sin(r)), static_cast<TYPE>(n.y*n.y*(1.0 - cos(r)) + cos(r)), static_cast<TYPE>(n.y*n.z*(1.0 - cos(r)) - n.x*sin(r)), 0,
				static_cast<TYPE>(n.z*n.x*(1.0 - cos(r)) - n.y*sin(r)), static_cast<TYPE>(n.y*n.z*(1.0 - cos(r)) + n.x*sin(r)), static_cast<TYPE>(n.z*n.z*(1.0 - cos(r)) + cos(r)), 0,
				0, 0, 0, 1,
			};
			return dst;
		}

		static DsMat44<TYPE> Get(const DsMat33<TYPE>& rot, const DsVec3d& pos)
		{
			const DsMat44<TYPE> dst =
			{
				rot.m00, rot.m01, rot.m02, pos.x,
				rot.m10, rot.m11, rot.m12, pos.y,
				rot.m20, rot.m21, rot.m22, pos.z,
				0, 0, 0, 1,
			};
			return dst;
		}

		static DsMat44<TYPE> Get(const TYPE src[16])
		{
			const DsMat44<TYPE> dst =
			{
				src[0], src[1], src[2], src[3],
				src[4], src[5], src[6], src[7],
				src[8], src[9], src[10], src[11],
				src[12], src[13], src[14], src[15],
			};
			return dst;
		}

		static DsMat44<TYPE> GetTranspose(const DsMat33<TYPE>& rot, const DsVec3<TYPE>& pos)
		{
			const DsMat44<TYPE> dst =
			{
				rot.m00, rot.m10, rot.m20, 0,
				rot.m01, rot.m11, rot.m21, 0,
				rot.m02, rot.m12, rot.m22, 0,
				pos.x, pos.y, pos.z, 1,
			};
			return dst;
		}
	};

	inline DsMat44f ToMat44f(const DsMat44d& src)
	{
		const DsMat44f dst =
		{
			static_cast<float>(src.m00), static_cast<float>(src.m01), static_cast<float>(src.m02), static_cast<float>(src.m03),
			static_cast<float>(src.m10), static_cast<float>(src.m11), static_cast<float>(src.m12), static_cast<float>(src.m13),
			static_cast<float>(src.m20), static_cast<float>(src.m21), static_cast<float>(src.m22), static_cast<float>(src.m23),
			static_cast<float>(src.m30), static_cast<float>(src.m31), static_cast<float>(src.m32), static_cast<float>(src.m33),
		};
		return dst;
	}

	inline DsMat44d ToMat44d(const DsMat44f& src)
	{
		const DsMat44d dst =
		{
			static_cast<double>(src.m00), static_cast<double>(src.m01), static_cast<double>(src.m02), static_cast<double>(src.m03),
			static_cast<double>(src.m10), static_cast<double>(src.m11), static_cast<double>(src.m12), static_cast<double>(src.m13),
			static_cast<double>(src.m20), static_cast<double>(src.m21), static_cast<double>(src.m22), static_cast<double>(src.m23),
			static_cast<double>(src.m30), static_cast<double>(src.m31), static_cast<double>(src.m32), static_cast<double>(src.m33),
		};
		return dst;
	}
}

#endif