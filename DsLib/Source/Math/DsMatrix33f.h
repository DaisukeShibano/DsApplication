#ifndef _DS_MATRIX33F_H_
#define _DS_MATRIX33F_H_

namespace DsLib
{
	struct DsMat33f
	{
		union
		{
			float mat[9];
			struct
			{
				float m00, m01, m02;
				float m10, m11, m12;
				float m20, m21, m22;
			};
		};

		//void operator =( const DsMat33f& src )
		//{
		//	for(int i=0; 9>i; ++i)
		//	{
		//		mat[i] = src.mat[i];
		//	}
		//}

		float& operator[](const int index )
		{
			DS_ASSERT( (index < 9), "indexの値がサイズを超えています\n" );
			return mat[ index ];
		}

		const float& operator[](const int index ) const
		{
			DS_ASSERT( (index < 9), "indexの値がサイズを超えています\n" );
			return mat[ index ];
		}

		DsMat33f operator *(const DsMat33f& src ) const
		{
			DsMat33f dst = 
			{
				m00*src.m00 + m01*src.m10 + m02*src.m20,	m00*src.m01 + m01*src.m11 + m02*src.m21,	m00*src.m02 + m01*src.m12 + m02*src.m22,
				m10*src.m00 + m11*src.m10 + m12*src.m20,	m10*src.m01 + m11*src.m11 + m12*src.m21,	m10*src.m02 + m11*src.m12 + m12*src.m22,
				m20*src.m00 + m21*src.m10 + m22*src.m20,	m20*src.m01 + m21*src.m11 + m22*src.m21,	m20*src.m02 + m21*src.m12 + m22*src.m22,
			};
			return dst;
		}

		DsVec3f operator *(const DsVec3f& src ) const
		{
			DsVec3f dst =
			{
				m00*src.x + m01*src.y + m02*src.z,
				m10*src.x + m11*src.y + m12*src.z,
				m20*src.x + m21*src.y + m22*src.z,
			};
			return dst;
		}

		DsMat33f operator +(const DsMat33f& src) const
		{
			DsMat33f dst =
			{
				m00+src.m00, m01+src.m01, m02+src.m02, 
				m10+src.m10, m11+src.m11, m12+src.m12, 
				m20+src.m20, m21+src.m21, m22+src.m22, 
			};
			return dst;
		}

		DsMat33f operator -(const DsMat33f& src) const
		{
			DsMat33f dst =
			{
				m00-src.m00, m01-src.m01, m02-src.m02, 
				m10-src.m10, m11-src.m11, m12-src.m12, 
				m20-src.m20, m21-src.m21, m22-src.m22, 
			};
			return dst;
		}

		DsMat33f& operator +=(const DsMat33f& src)
		{
			m00+=src.m00; m01+=src.m01; m02+=src.m02; 
			m10+=src.m10; m11+=src.m11; m12+=src.m12; 
			m20+=src.m20; m21+=src.m21; m22+=src.m22; 	
			return (*this);
		}

		DsMat33f& operator -=(const DsMat33f& src)
		{
			m00-=src.m00; m01-=src.m01; m02-=src.m02; 
			m10-=src.m10; m11-=src.m11; m12-=src.m12; 
			m20-=src.m20; m21-=src.m21; m22-=src.m22; 	
			return (*this);
		}

		DsMat33f MultiDiag(const float val) const
		{
			DsMat33f dst =
			{
				m00*val, m01, m02,
				m10, m11*val, m12,
				m20, m21, m22*val,
			};
			return dst;
		}

		DsVec3f GetAxisX() const
		{
			DsVec3f dst = { m00, m10, m20};
			return dst;
		}

		DsVec3f GetAxisY() const
		{
			DsVec3f dst = { m01, m11, m21};
			return dst;
		}

		DsVec3f GetAxisZ() const
		{
			DsVec3f dst = { m02, m12, m22};
			return dst;
		}

		DsMat33f ToTransposition() const
		{
			const DsMat33f dst =
			{
				m00, m10, m20,
				m01, m11, m21,
				m02, m12, m22,
			};
			return dst;
		}

		bool IsNearEqual(const DsMat33f& src) const
		{
			const float err = (0.0000001f);
			for (int i = 0; i < 9; ++i)
			{
				const float sub = mat[i] - src.mat[i];
				if ((sub > err) || (sub < -err))
				{
					return false;
				}
			}
			return true;
		}

		bool IsNearEqual(const DsMat33f& src, const float err) const
		{
			for (int i = 0; i < 9; ++i)
			{
				const float sub = mat[i] - src.mat[i];
				if ((sub > err) || (sub < -err))
				{
					return false;
				}
			}
			return true;
		}

		static DsMat33f SetAxis( const DsVec3f& x, const DsVec3f& y, const DsVec3f& z )
		{
			DsMat33f dst =
			{
				x.x, y.x, z.x,
				x.y, y.y, z.y,
				x.z, y.z, z.z,
			};
			return dst;
		}


		static DsMat33f Identity()
		{
			DsMat33f dst =
			{
				1.0, 0.0, 0.0,
				0.0, 1.0, 0.0,
				0.0, 0.0, 1.0,
			};
			return dst;
		}

		static DsMat33f Inverse(const DsMat33f& src)
		{
			float det = src.m00*src.m11*src.m22 + src.m10*src.m21*src.m02 + src.m20*src.m01*src.m12 - src.m00*src.m21*src.m12 - src.m20*src.m11*src.m02 - src.m10*src.m01*src.m22;
			if(0.0f >= fabs(det))
			{
				return src;
			}

			const DsMat33f dst =
			{
				(src.m11*src.m22-src.m12*src.m21)/det,	(src.m02*src.m21-src.m01*src.m22)/det,	(src.m01*src.m12-src.m02*src.m11)/det,
				(src.m12*src.m20-src.m10*src.m22)/det,	(src.m00*src.m22-src.m02*src.m20)/det,	(src.m02*src.m10-src.m00*src.m12)/det,
				(src.m10*src.m21-src.m11*src.m20)/det,	(src.m01*src.m20-src.m00*src.m21)/det,	(src.m00*src.m11-src.m01*src.m10)/det,
			};
			return dst;
		}

		static DsMat33f RotateX(float ang)
		{
			const DsMat33f dst =
			{
				1.0f, 0.0f, 0.0f,
				0.0f, cosf(ang), -sinf(ang),
				0.0f, sinf(ang), cosf(ang),
			};
			return dst;
		}

		static DsMat33f RotateY(float ang)
		{
			const DsMat33f dst =
			{
				cosf(ang), 0.0f, sinf(ang),
				0.0f, 1.0f, 0.0f,
				-sinf(ang), 0.0f, cosf(ang),
			};
			return dst;
		}

		static DsMat33f RotateZ(float ang)
		{
			const DsMat33f dst =
			{
				cosf(ang), -sinf(ang), 0.0f,
				sinf(ang), cosf(ang), 0.0f,
				0.0f, 0.0f, 1.0f,
			};
			return dst;
		}

		//vecの長さが角度
		static DsMat33f RotateVec( const DsVec3f& vec )
		{
			const DsVec3f n = DsVec3f::Normalize(vec);
			const float r = vec.Length();
			const DsMat33f dst =
			{
				(n.x*n.x*(1.0f - cosf(r)) + cosf(r)),	   (n.x*n.y*(1.0f - cosf(r)) - n.z*sinf(r)),	(n.z*n.x*(1.0f - cosf(r)) + n.y*sinf(r)),
				(n.x*n.y*(1.0f - cosf(r)) + n.z*sinf(r)), (n.y*n.y*(1.0f - cosf(r)) + cosf(r)),		(n.y*n.z*(1.0f - cosf(r)) - n.x*sinf(r)),
				(n.z*n.x*(1.0f - cosf(r)) - n.y*sinf(r)), (n.y*n.z*(1.0f - cosf(r)) + n.x*sinf(r)),   (n.z*n.z*(1.0f - cosf(r)) + cosf(r)),
			};
			return dst;
		}
		//nは正規化ずみ
		static DsMat33f RotateAxis(const DsVec3f& n, float r)
		{
			const DsMat33f dst =
			{
				(n.x*n.x*(1.0f - cosf(r)) + cosf(r)),	   (n.x*n.y*(1.0f - cosf(r)) - n.z*sinf(r)),	(n.z*n.x*(1.0f - cosf(r)) + n.y*sinf(r)),
				(n.x*n.y*(1.0f - cosf(r)) + n.z*sinf(r)), (n.y*n.y*(1.0f - cosf(r)) + cosf(r)),		(n.y*n.z*(1.0f - cosf(r)) - n.x*sinf(r)),
				(n.z*n.x*(1.0f - cosf(r)) - n.y*sinf(r)), (n.y*n.z*(1.0f - cosf(r)) + n.x*sinf(r)),   (n.z*n.z*(1.0f - cosf(r)) + cosf(r)),
			};
			return dst;
		}

		static DsMat33f ToMat33( const float mat16[16] )
		{
			const DsMat33f dst =
			{
				mat16[0], mat16[1], mat16[2],
				mat16[4], mat16[5], mat16[6],
				mat16[8], mat16[9], mat16[10],
			};
			return dst;
		}

		static DsMat33f ToMat33(const float mat33[3][3])
		{
			const DsMat33f dst =
			{
				mat33[0][0], mat33[0][1], mat33[0][2],
				mat33[1][0], mat33[1][1], mat33[1][2],
				mat33[2][0], mat33[2][1], mat33[2][2],
			};
			return dst;
		}

		static void ToMat16(const DsMat33f& src, float dst[16])
		{
			dst[0] = src.m00; dst[1] = src.m01; dst[2] = src.m02;
			dst[4] = src.m10; dst[5] = src.m11; dst[6] = src.m12;
			dst[8] = src.m20; dst[9] = src.m21; dst[10] = src.m22;
		}
	};
}

#endif