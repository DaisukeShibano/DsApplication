#ifndef _DS_MATRIX44F_H_
#define _DS_MATRIX44F_H_

namespace DsLib
{
	struct DsVec4f;
	struct DsVec3f;
	struct DsMat33f;
}

/*
ギリコピーの方が早い
*/

namespace DsLib
{
	struct DsMat44f
	{
		union
		{
			float mat[16];
			struct
			{
				float m00, m01, m02, m03;
				float m10, m11, m12, m13;
				float m20, m21, m22, m23;
				float m30, m31, m32, m33;
			};
			struct
			{
				DsVec4f v[4];
			};
		};

		//void operator =(const DsMat44f& src)
		//{
		//	for (int i = 0; 16>i; ++i)
		//	{
		//		mat[i] = src.mat[i];
		//	}
		//}

		DsVec4f& operator[](const int index)
		{
			DS_ASSERT(index < 4, "indexの値がサイズを超えています\n");
			return v[index];
		}

		const DsVec4f& operator[](const int index) const
		{
			DS_ASSERT(index < 4, "indexの値がサイズを超えています\n");
			return v[index];
		}

		DsMat44f operator *(const DsMat44f& src) const
		{
			DsMat44f dst =
			{
				m00*src.m00 + m01*src.m10 + m02*src.m20 + m03*src.m30, m00*src.m01 + m01*src.m11 + m02*src.m21 + m03*src.m31, m00*src.m02 + m01*src.m12 + m02*src.m22 + m03*src.m32, m00*src.m03 + m01*src.m13 + m02*src.m23 + m03*src.m33,
				m10*src.m00 + m11*src.m10 + m12*src.m20 + m13*src.m30, m10*src.m01 + m11*src.m11 + m12*src.m21 + m13*src.m31, m10*src.m02 + m11*src.m12 + m12*src.m22 + m13*src.m32, m10*src.m03 + m11*src.m13 + m12*src.m23 + m13*src.m33,
				m20*src.m00 + m21*src.m10 + m22*src.m20 + m23*src.m30, m20*src.m01 + m21*src.m11 + m22*src.m21 + m23*src.m31, m20*src.m02 + m21*src.m12 + m22*src.m22 + m23*src.m32, m20*src.m03 + m21*src.m13 + m22*src.m23 + m23*src.m33,
				m30*src.m00 + m31*src.m10 + m32*src.m20 + m33*src.m30, m30*src.m01 + m31*src.m11 + m32*src.m21 + m33*src.m31, m30*src.m02 + m31*src.m12 + m32*src.m22 + m33*src.m32, m30*src.m03 + m31*src.m13 + m32*src.m23 + m33*src.m33,
			};
			return dst;
		}

		DsVec4f operator *(const DsVec4f& src) const
		{
			DsVec4f dst =
			{
				m00*src.x + m01*src.y + m02*src.z + m03*src.w,
				m10*src.x + m11*src.y + m12*src.z + m13*src.w,
				m20*src.x + m21*src.y + m22*src.z + m23*src.w,
				m30*src.x + m31*src.y + m32*src.z + m33*src.w,
			};
			return dst;
		}

		//回転成分だけ掛ける
		DsVec3f MultiRot(const DsVec3f& src) const
		{
			DsVec3f dst =
			{
				m00*src.x + m01*src.y + m02*src.z,
				m10*src.x + m11*src.y + m12*src.z,
				m20*src.x + m21*src.y + m22*src.z,
			};
			return dst;
		}

		DsMat44f operator +(const DsMat44f& src) const
		{
			DsMat44f dst =
			{
				m00 + src.m00, m01 + src.m01, m02 + src.m02, m03 + src.m03,
				m10 + src.m10, m11 + src.m11, m12 + src.m12, m13 + src.m13,
				m20 + src.m20, m21 + src.m21, m22 + src.m22, m23 + src.m23,
				m30 + src.m30, m31 + src.m31, m32 + src.m32, m33 + src.m33,
			};
			return dst;
		}

		DsMat44f operator -(const DsMat44f& src) const
		{
			DsMat44f dst =
			{
				m00 - src.m00, m01 - src.m01, m02 - src.m02, m03 - src.m03,
				m10 - src.m10, m11 - src.m11, m12 - src.m12, m13 - src.m13,
				m20 - src.m20, m21 - src.m21, m22 - src.m22, m23 - src.m23,
				m30 - src.m30, m31 - src.m31, m32 - src.m32, m33 - src.m33,
			};
			return dst;
		}

		DsMat44f& operator +=(const DsMat44f& src)
		{
			m00 += src.m00; m01 += src.m01; m02 += src.m02; m03 += src.m03;
			m10 += src.m10; m11 += src.m11; m12 += src.m12; m13 += src.m13;
			m20 += src.m20; m21 += src.m21; m22 += src.m22; m23 += src.m23;
			m30 += src.m30; m31 += src.m31; m32 += src.m32; m33 += src.m33;
			return (*this);
		}

		DsMat44f& operator -=(const DsMat44f& src)
		{
			m00 -= src.m00; m01 -= src.m01; m02 -= src.m02; m03 -= src.m03;
			m10 -= src.m10; m11 -= src.m11; m12 -= src.m12; m13 -= src.m13;
			m20 -= src.m20; m21 -= src.m21; m22 -= src.m22; m23 -= src.m23;
			m30 -= src.m30; m31 -= src.m31; m32 -= src.m32; m33 -= src.m33;
			return (*this);
		}

		DsMat44f& operator =(const float src[4][4] )
		{
			m00 = src[0][0]; m01 = src[0][1]; m02 = src[0][2]; m03 = src[0][3];
			m10 = src[1][0]; m11 = src[1][1]; m12 = src[1][2]; m13 = src[1][3];
			m20 = src[2][0]; m21 = src[2][1]; m22 = src[2][2]; m23 = src[2][3];
			m30 = src[3][0]; m31 = src[3][1]; m32 = src[3][2]; m33 = src[3][3];
			return (*this);
		}
		DsMat44f& operator =(const double src[4][4])
		{
			m00 = static_cast<float>(src[0][0]); m01 = static_cast<float>(src[0][1]); m02 = static_cast<float>(src[0][2]); m03 = static_cast<float>(src[0][3]);
			m10 = static_cast<float>(src[1][0]); m11 = static_cast<float>(src[1][1]); m12 = static_cast<float>(src[1][2]); m13 = static_cast<float>(src[1][3]);
			m20 = static_cast<float>(src[2][0]); m21 = static_cast<float>(src[2][1]); m22 = static_cast<float>(src[2][2]); m23 = static_cast<float>(src[2][3]);
			m30 = static_cast<float>(src[3][0]); m31 = static_cast<float>(src[3][1]); m32 = static_cast<float>(src[3][2]); m33 = static_cast<float>(src[3][3]);
			return (*this);
		}

		DsMat44f& SetRotation(const DsMat33f& src)
		{
			m00 = src.m00; m01 = src.m01; m02 = src.m02;
			m10 = src.m10; m11 = src.m11; m12 = src.m12;
			m20 = src.m20; m21 = src.m21; m22 = src.m22;
			return (*this);
		}


		DsMat33f ToMat33() const
		{
			const DsMat33f dst=
			{
				m00, m01, m02,
				m10, m11, m12,
				m20, m21, m22,
			};
			return dst;
		}

		DsMat44f ToTransposition() const
		{
			const DsMat44f dst =
			{
				m00, m10, m20, m30,
				m01, m11, m21, m31,
				m02, m12, m22, m32,
				m03, m13, m23, m33,
			};
			return dst;
		}

		bool IsNearEqual(const DsMat44f& src) const
		{
			const float err = (0.0000001f);
			for (int i = 0; i < 16; ++i)
			{
				const float sub = mat[i] - src.mat[i];
				if ((sub > err) || (sub < -err))
				{
					return false;
				}
			}
			return true;
		}

		DsVec3f GetAxisX() const
		{
			DsVec3f dst = { m00, m10, m20 };
			return dst;
		}

		DsVec3f GetAxisY() const
		{
			DsVec3f dst = { m01, m11, m21 };
			return dst;
		}

		DsVec3f GetAxisZ() const
		{
			DsVec3f dst = { m02, m12, m22 };
			return dst;
		}

		DsVec3f GetPos() const
		{
			DsVec3f dst = { m03, m13, m23 };
			return dst;
		}
		void SetPos(const DsVec3f& v)
		{
			m03 = v.x;
			m13 = v.y;
			m23 = v.z;
		}

		DsVec3f GetPosT() const
		{
			DsVec3f dst = { m30, m31, m32 };
			return dst;
		}

		static DsMat44f SetAxis(const DsVec3f& x, const DsVec3f& y, const DsVec3f& z)
		{
			DsMat44f dst =
			{
				x.x, y.x, z.x, 0,
				x.y, y.y, z.y, 0,
				x.z, y.z, z.z, 0,
				0, 0, 0, 1,
			};
			return dst;
		}
		static DsMat44f SetAxisPos(const DsVec3f& x, const DsVec3f& y, const DsVec3f& z, const DsVec3f& p)
		{
			DsMat44f dst =
			{
				x.x, y.x, z.x, p.x,
				x.y, y.y, z.y, p.y,
				x.z, y.z, z.z, p.z,
				0, 0, 0, 1,
			};
			return dst;
		}


		static DsMat44f Identity()
		{
			DsMat44f dst =
			{
				1, 0, 0, 0,
				0, 1, 0, 0,
				0, 0, 1, 0,
				0, 0, 0, 1,
			};
			return dst;
		}

		static DsMat44f Inverse(const DsMat44f& src)
		{
			DS_ASSERT(false, "未実装");
			DsMat44f ret = { 0 };
;			return ret;
		}

		static DsMat44f RotateX(float ang)
		{
			const DsMat44f dst =
			{
				1, 0, 0, 0,
				0, cosf(ang), -sinf(ang), 0,
				0, sinf(ang), cosf(ang), 0,
				0, 0, 0, 1,
			};
			return dst;
		}

		static DsMat44f RotateY(float ang)
		{
			const DsMat44f dst =
			{
				cosf(ang), 0, sinf(ang), 0,
				0, 1, 0, 0,
				-sinf(ang), 0, cosf(ang), 0,
				0, 0, 0, 1,
			};
			return dst;
		}

		static DsMat44f RotateZ(float ang)
		{
			const DsMat44f dst =
			{
				cosf(ang), -sinf(ang), 0, 0,
				sinf(ang), cosf(ang), 0, 0,
				0, 0, 1, 0,
				0, 0, 0, 1, 
			};
			return dst;
		}

		static DsMat44f RotateVec(const DsVec3f& vec)
		{
			const DsVec3f n = DsVec3f::Normalize(vec);
			const float r = vec.Length();
			const DsMat44f dst =
			{
				(n.x*n.x*(1.0f - cosf(r)) + cosf(r)),     (n.x*n.y*(1.0f - cosf(r)) - n.z*sinf(r)), (n.z*n.x*(1.0f - cosf(r)) + n.y*sinf(r)), 0,
				(n.x*n.y*(1.0f - cosf(r)) + n.z*sinf(r)), (n.y*n.y*(1.0f - cosf(r)) + cosf(r)),     (n.y*n.z*(1.0f - cosf(r)) - n.x*sinf(r)), 0,
				(n.z*n.x*(1.0f - cosf(r)) - n.y*sinf(r)), (n.y*n.z*(1.0f - cosf(r)) + n.x*sinf(r)), (n.z*n.z*(1.0f - cosf(r)) + cosf(r)), 0,
				0, 0, 0, 1,
			};
			return dst;
		}

		//nは正規化済み
		static DsMat44f RotateAxis(const DsVec3f& n, float r)
		{
			const DsMat44f dst =
			{
				(n.x*n.x*(1.0f - cosf(r)) + cosf(r)),     (n.x*n.y*(1.0f - cosf(r)) - n.z*sinf(r)), (n.z*n.x*(1.0f - cosf(r)) + n.y*sinf(r)), 0,
				(n.x*n.y*(1.0f - cosf(r)) + n.z*sinf(r)), (n.y*n.y*(1.0f - cosf(r)) + cosf(r)),     (n.y*n.z*(1.0f - cosf(r)) - n.x*sinf(r)), 0,
				(n.z*n.x*(1.0f - cosf(r)) - n.y*sinf(r)), (n.y*n.z*(1.0f - cosf(r)) + n.x*sinf(r)), (n.z*n.z*(1.0f - cosf(r)) + cosf(r)), 0,
				0, 0, 0, 1,
			};
			return dst;
		}

		static DsMat44f Get(const DsMat33f& rot, const DsVec3f& pos)
		{
			const DsMat44f dst =
			{
				rot.m00, rot.m01, rot.m02, pos.x,
				rot.m10, rot.m11, rot.m12, pos.y,
				rot.m20, rot.m21, rot.m22, pos.z,
				0, 0, 0, 1,
			};
			return dst;
		}

		static DsMat44f Get(const float src[16])
		{
			const DsMat44f dst =
			{
				src[0], src[1], src[2], src[3],
				src[4], src[5], src[6], src[7],
				src[8], src[9], src[10], src[11],
				src[12], src[13], src[14], src[15],
			};
			return dst;
		}

		static DsMat44f GetTranspose(const DsMat33f& rot, const DsVec3f& pos)
		{
			const DsMat44f dst =
			{
				rot.m00, rot.m10, rot.m20, 0,
				rot.m01, rot.m11, rot.m21, 0,
				rot.m02, rot.m12, rot.m22, 0,
				pos.x, pos.y, pos.z, 1,
			};
			return dst;
		}

		static DsMat44f GetTranspose(const DsMat44f& src)
		{
			const DsMat44f dst =
			{
				src.m00, src.m10, src.m20, src.m30,
				src.m01, src.m11, src.m21, src.m31,
				src.m02, src.m12, src.m22, src.m32,
				src.m03, src.m13, src.m23, src.m33,
			};
			return dst;
		}
	};
}

#endif