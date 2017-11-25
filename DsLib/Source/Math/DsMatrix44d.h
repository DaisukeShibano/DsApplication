#ifndef _DS_MATRIX44D_H_
#define _DS_MATRIX44D_H_

namespace DsLib
{
	struct DsVec4d;
	struct DsVec3d;
	struct DsMat33d;
}


/*
 ギリコピーの方が遅い。
*/


namespace DsLib
{
	struct DsMat44d
	{
		union
		{
			double mat[16];
			struct
			{
				double m00, m01, m02, m03;
				double m10, m11, m12, m13;
				double m20, m21, m22, m23;
				double m30, m31, m32, m33;
			};
			struct
			{
				DsVec4d v[4];
			};
		};

		//void operator =(const DsMat44d& src)
		//{
		//	for (int i = 0; 16>i; ++i)
		//	{
		//		mat[i] = src.mat[i];
		//	}
		//}

		DsVec4d& operator[](const int index)
		{
			assert((index < 4) && "indexの値がサイズを超えています\n");
			return v[index];
		}

		const DsVec4d& operator[](const int index) const
		{
			assert((index < 4) && "indexの値がサイズを超えています\n");
			return v[index];
		}

		DsMat44d operator *(const DsMat44d& src) const
		{
			DsMat44d dst =
			{
				m00*src.m00 + m01*src.m10 + m02*src.m20 + m03*src.m30, m00*src.m01 + m01*src.m11 + m02*src.m21 + m03*src.m31, m00*src.m02 + m01*src.m12 + m02*src.m22 + m03*src.m32, m00*src.m03 + m01*src.m13 + m02*src.m23 + m03*src.m33,
				m10*src.m00 + m11*src.m10 + m12*src.m20 + m13*src.m30, m10*src.m01 + m11*src.m11 + m12*src.m21 + m13*src.m31, m10*src.m02 + m11*src.m12 + m12*src.m22 + m13*src.m32, m10*src.m03 + m11*src.m13 + m12*src.m23 + m13*src.m33,
				m20*src.m00 + m21*src.m10 + m22*src.m20 + m23*src.m30, m20*src.m01 + m21*src.m11 + m22*src.m21 + m23*src.m31, m20*src.m02 + m21*src.m12 + m22*src.m22 + m23*src.m32, m20*src.m03 + m21*src.m13 + m22*src.m23 + m23*src.m33,
				m30*src.m00 + m31*src.m10 + m32*src.m20 + m33*src.m30, m30*src.m01 + m31*src.m11 + m32*src.m21 + m33*src.m31, m30*src.m02 + m31*src.m12 + m32*src.m22 + m33*src.m32, m30*src.m03 + m31*src.m13 + m32*src.m23 + m33*src.m33,
			};
			return dst;
		}

		DsVec4d operator *(const DsVec4d& src) const
		{
			DsVec4d dst =
			{
				m00*src.x + m01*src.y + m02*src.z + m03*src.w,
				m10*src.x + m11*src.y + m12*src.z + m13*src.w,
				m20*src.x + m21*src.y + m22*src.z + m23*src.w,
				m30*src.x + m31*src.y + m32*src.z + m33*src.w,
			};
			return dst;
		}

		//回転成分だけ掛ける
		DsVec3d MultiRot(const DsVec3d& src) const
		{
			DsVec3d dst =
			{
				m00*src.x + m01*src.y + m02*src.z,
				m10*src.x + m11*src.y + m12*src.z,
				m20*src.x + m21*src.y + m22*src.z,
			};
			return dst;
		}

		DsMat44d operator +(const DsMat44d& src) const
		{
			DsMat44d dst =
			{
				m00 + src.m00, m01 + src.m01, m02 + src.m02, m03 + src.m03,
				m10 + src.m10, m11 + src.m11, m12 + src.m12, m13 + src.m13,
				m20 + src.m20, m21 + src.m21, m22 + src.m22, m23 + src.m23,
				m30 + src.m30, m31 + src.m31, m32 + src.m32, m33 + src.m33,
			};
			return dst;
		}

		DsMat44d operator -(const DsMat44d& src) const
		{
			DsMat44d dst =
			{
				m00 - src.m00, m01 - src.m01, m02 - src.m02, m03 - src.m03,
				m10 - src.m10, m11 - src.m11, m12 - src.m12, m13 - src.m13,
				m20 - src.m20, m21 - src.m21, m22 - src.m22, m23 - src.m23,
				m30 - src.m30, m31 - src.m31, m32 - src.m32, m33 - src.m33,
			};
			return dst;
		}

		DsMat44d& operator +=(const DsMat44d& src)
		{
			m00 += src.m00; m01 += src.m01; m02 += src.m02; m03 += src.m03;
			m10 += src.m10; m11 += src.m11; m12 += src.m12; m13 += src.m13;
			m20 += src.m20; m21 += src.m21; m22 += src.m22; m23 += src.m23;
			m30 += src.m30; m31 += src.m31; m32 += src.m32; m33 += src.m33;
			return (*this);
		}

		DsMat44d& operator -=(const DsMat44d& src)
		{
			m00 -= src.m00; m01 -= src.m01; m02 -= src.m02; m03 -= src.m03;
			m10 -= src.m10; m11 -= src.m11; m12 -= src.m12; m13 -= src.m13;
			m20 -= src.m20; m21 -= src.m21; m22 -= src.m22; m23 -= src.m23;
			m30 -= src.m30; m31 -= src.m31; m32 -= src.m32; m33 -= src.m33;
			return (*this);
		}

		DsMat44d& operator =(const double src[4][4] )
		{
			m00 = src[0][0]; m01 = src[0][1]; m02 = src[0][2]; m03 = src[0][3];
			m10 = src[1][0]; m11 = src[1][1]; m12 = src[1][2]; m13 = src[1][3];
			m20 = src[2][0]; m21 = src[2][1]; m22 = src[2][2]; m23 = src[2][3];
			m30 = src[3][0]; m31 = src[3][1]; m32 = src[3][2]; m33 = src[3][3];
			return (*this);
		}
		DsMat44d& operator =(const float src[4][4])
		{
			m00 = src[0][0]; m01 = src[0][1]; m02 = src[0][2]; m03 = src[0][3];
			m10 = src[1][0]; m11 = src[1][1]; m12 = src[1][2]; m13 = src[1][3];
			m20 = src[2][0]; m21 = src[2][1]; m22 = src[2][2]; m23 = src[2][3];
			m30 = src[3][0]; m31 = src[3][1]; m32 = src[3][2]; m33 = src[3][3];
			return (*this);
		}

		DsMat44d& SetRotation(const DsMat33d& src)
		{
			m00 = src.m00; m01 = src.m01; m02 = src.m02;
			m10 = src.m10; m11 = src.m11; m12 = src.m12;
			m20 = src.m20; m21 = src.m21; m22 = src.m22;
			return (*this);
		}


		DsMat33d ToMat33() const
		{
			const DsMat33d dst=
			{
				m00, m01, m02,
				m10, m11, m12,
				m20, m21, m22,
			};
			return dst;
		}

		DsMat44d ToTransposition() const
		{
			const DsMat44d dst =
			{
				m00, m10, m20, m30,
				m01, m11, m21, m31,
				m02, m12, m22, m32,
				m03, m13, m23, m33,
			};
			return dst;
		}

		bool IsNearEqual(const DsMat44d& src) const
		{
			const double err = (0.0000001);
			for (int i = 0; i < 16; ++i)
			{
				const double sub = mat[i] - src.mat[i];
				if ((sub > err) || (sub < -err))
				{
					return false;
				}
			}
			return true;
		}

		DsVec3d GetAxisX() const
		{
			DsVec3d dst = { m00, m10, m20 };
			return dst;
		}

		DsVec3d GetAxisY() const
		{
			DsVec3d dst = { m01, m11, m21 };
			return dst;
		}

		DsVec3d GetAxisZ() const
		{
			DsVec3d dst = { m02, m12, m22 };
			return dst;
		}

		DsVec3d GetPos() const
		{
			DsVec3d dst = { m03, m13, m23 };
			return dst;
		}

		DsVec3d GetPosT() const
		{
			DsVec3d dst = { m30, m31, m32 };
			return dst;
		}

		static DsMat44d SetAxis(const DsVec3d& x, const DsVec3d& y, const DsVec3d& z)
		{
			DsMat44d dst =
			{
				x.x, y.x, z.x, 0,
				x.y, y.y, z.y, 0,
				x.z, y.z, z.z, 0,
				0, 0, 0, 1,
			};
			return dst;
		}


		static DsMat44d Identity()
		{
			DsMat44d dst =
			{
				1, 0, 0, 0,
				0, 1, 0, 0,
				0, 0, 1, 0,
				0, 0, 0, 1,
			};
			return dst;
		}

		static DsMat44d Inverse(const DsMat44d& src)
		{
			DS_ASSERT(false, "未実装");
			DsMat44d ret = {0};
			return ret;
		}

		static DsMat44d RotateX(double ang)
		{
			const DsMat44d dst =
			{
				1, 0, 0, 0,
				0, cos(ang), -sin(ang), 0,
				0, sin(ang), cos(ang), 0,
				0, 0, 0, 1,
			};
			return dst;
		}

		static DsMat44d RotateY(double ang)
		{
			const DsMat44d dst =
			{
				cos(ang), 0, sin(ang), 0,
				0, 1, 0, 0,
				-sin(ang), 0, cos(ang), 0,
				0, 0, 0, 1,
			};
			return dst;
		}

		static DsMat44d RotateZ(double ang)
		{
			const DsMat44d dst =
			{
				cos(ang), -sin(ang), 0, 0,
				sin(ang), cos(ang), 0, 0,
				0, 0, 1, 0,
				0, 0, 0, 1, 
			};
			return dst;
		}

		static DsMat44d RotateVec(const DsVec3d& vec)
		{
			const DsVec3d n = DsVec3d::Normalize(vec);
			const double r = vec.Length();
			const DsMat44d dst =
			{
				(n.x*n.x*(1.0 - cos(r)) + cos(r)),     (n.x*n.y*(1.0 - cos(r)) - n.z*sin(r)), (n.z*n.x*(1.0 - cos(r)) + n.y*sin(r)), 0,
				(n.x*n.y*(1.0 - cos(r)) + n.z*sin(r)), (n.y*n.y*(1.0 - cos(r)) + cos(r)),     (n.y*n.z*(1.0 - cos(r)) - n.x*sin(r)), 0,
				(n.z*n.x*(1.0 - cos(r)) - n.y*sin(r)), (n.y*n.z*(1.0 - cos(r)) + n.x*sin(r)), (n.z*n.z*(1.0 - cos(r)) + cos(r)), 0,
				0, 0, 0, 1,
			};
			return dst;
		}

		static DsMat44d Get(const DsMat33d& rot, const DsVec3d& pos)
		{
			const DsMat44d dst =
			{
				rot.m00, rot.m01, rot.m02, pos.x,
				rot.m10, rot.m11, rot.m12, pos.y,
				rot.m20, rot.m21, rot.m22, pos.z,
				0, 0, 0, 1,
			};
			return dst;
		}

		static DsMat44d Get(const double src[16] )
		{
			const DsMat44d dst =
			{
				src[0], src[1], src[2], src[3],
				src[4], src[5], src[6], src[7],
				src[8], src[9], src[10], src[11],
				src[12], src[13], src[14], src[15],
			};
			return dst;
		}

		static DsMat44d GetTranspose(const DsMat33d& rot, const DsVec3d& pos)
		{
			const DsMat44d dst =
			{
				rot.m00, rot.m10, rot.m20, 0,
				rot.m01, rot.m11, rot.m21, 0,
				rot.m02, rot.m12, rot.m22, 0,
				pos.x, pos.y, pos.z, 1,
			};
			return dst;
		}

	};
}

#endif