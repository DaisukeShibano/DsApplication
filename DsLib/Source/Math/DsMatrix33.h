#ifndef _DS_MATRIX33_H_
#define _DS_MATRIX33_H_

#ifndef _DS_MATRIX33D_H_
#include "Math/DsMatrix33d.h"
#endif

#ifndef _DS_MATRIX33F_H_
#include "Math/DsMatrix33f.h"
#endif


namespace DsLib
{
	template<typename TYPE>
	struct DsMat33
	{
		union
		{
			TYPE mat[9];
			struct
			{
				TYPE m00, m01, m02;
				TYPE m10, m11, m12;
				TYPE m20, m21, m22;
			};
		};

		//void operator =( const DsMat33<TYPE>& src )
		//{
		//	for(int i=0; 9>i; ++i)
		//	{
		//		mat[i] = src.mat[i];
		//	}
		//}

		TYPE& operator[](const int index )
		{
			assert( (index < 9) && "indexの値がサイズを超えています\n" );
			return mat[ index ];
		}

		const TYPE& operator[](const int index ) const
		{
			assert( (index < 9) && "indexの値がサイズを超えています\n" );
			return mat[ index ];
		}

		DsMat33<TYPE> operator *(const DsMat33<TYPE>& src ) const
		{
			DsMat33<TYPE> dst = 
			{
				m00*src.m00 + m01*src.m10 + m02*src.m20,	m00*src.m01 + m01*src.m11 + m02*src.m21,	m00*src.m02 + m01*src.m12 + m02*src.m22,
				m10*src.m00 + m11*src.m10 + m12*src.m20,	m10*src.m01 + m11*src.m11 + m12*src.m21,	m10*src.m02 + m11*src.m12 + m12*src.m22,
				m20*src.m00 + m21*src.m10 + m22*src.m20,	m20*src.m01 + m21*src.m11 + m22*src.m21,	m20*src.m02 + m21*src.m12 + m22*src.m22,
			};
			return dst;
		}

		DsVec3<TYPE> operator *(const DsVec3<TYPE>& src ) const
		{
			DsVec3<TYPE> dst =
			{
				m00*src.x + m01*src.y + m02*src.z,
				m10*src.x + m11*src.y + m12*src.z,
				m20*src.x + m21*src.y + m22*src.z,
			};
			return dst;
		}

		DsMat33<TYPE> operator +(const DsMat33<TYPE>& src) const
		{
			DsMat33<TYPE> dst =
			{
				m00+src.m00, m01+src.m01, m02+src.m02, 
				m10+src.m10, m11+src.m11, m12+src.m12, 
				m20+src.m20, m21+src.m21, m22+src.m22, 
			};
			return dst;
		}

		DsMat33<TYPE> operator -(const DsMat33<TYPE>& src) const
		{
			DsMat33<TYPE> dst =
			{
				m00-src.m00, m01-src.m01, m02-src.m02, 
				m10-src.m10, m11-src.m11, m12-src.m12, 
				m20-src.m20, m21-src.m21, m22-src.m22, 
			};
			return dst;
		}

		DsMat33<TYPE>& operator +=(const DsMat33<TYPE>& src)
		{
			m00+=src.m00; m01+=src.m01; m02+=src.m02; 
			m10+=src.m10; m11+=src.m11; m12+=src.m12; 
			m20+=src.m20; m21+=src.m21; m22+=src.m22; 	
			return (*this);
		}

		DsMat33<TYPE>& operator -=(const DsMat33<TYPE>& src)
		{
			m00-=src.m00; m01-=src.m01; m02-=src.m02; 
			m10-=src.m10; m11-=src.m11; m12-=src.m12; 
			m20-=src.m20; m21-=src.m21; m22-=src.m22; 	
			return (*this);
		}

		DsMat33<TYPE> MultiDiag(const TYPE val) const
		{
			DsMat33<TYPE> dst =
			{
				m00*val, m01, m02,
				m10, m11*val, m12,
				m20, m21, m22*val,
			};
			return dst;
		}

		DsVec3<TYPE> GetAxisX() const
		{
			DsVec3<TYPE> dst = { m00, m10, m20};
			return dst;
		}

		DsVec3<TYPE> GetAxisY() const
		{
			DsVec3<TYPE> dst = { m01, m11, m21};
			return dst;
		}

		DsVec3<TYPE> GetAxisZ() const
		{
			DsVec3<TYPE> dst = { m02, m12, m22};
			return dst;
		}

		DsMat33<TYPE> ToTransposition() const
		{
			const DsMat33<TYPE> dst =
			{
				m00, m10, m20,
				m01, m11, m21,
				m02, m12, m22,
			};
			return dst;
		}

		bool IsNearEqual(const DsMat33<TYPE>& src) const
		{
			const TYPE err = static_cast<TYPE>(0.0000001);
			for (int i = 0; i < 9; ++i)
			{
				const TYPE sub = mat[i] - src.mat[i];
				if ((sub > err) || (sub < -err))
				{
					return false;
				}
			}
			return true;
		}

		bool IsNearEqual(const DsMat33<TYPE>& src, const TYPE err) const
		{
			for (int i = 0; i < 9; ++i)
			{
				const TYPE sub = mat[i] - src.mat[i];
				if ((sub > err) || (sub < -err))
				{
					return false;
				}
			}
			return true;
		}

		static DsMat33<TYPE> SetAxis( const DsVec3<TYPE>& x, const DsVec3<TYPE>& y, const DsVec3<TYPE>& z )
		{
			DsMat33<TYPE> dst =
			{
				x.x, y.x, z.x,
				x.y, y.y, z.y,
				x.z, y.z, z.z,
			};
			return dst;
		}


		static DsMat33<TYPE> Identity()
		{
			DsMat33<TYPE> dst =
			{
				1.0, 0.0, 0.0,
				0.0, 1.0, 0.0,
				0.0, 0.0, 1.0,
			};
			return dst;
		}

		static DsMat33<TYPE> Inverse(const DsMat33<TYPE>& src)
		{
			double det = src.m00*src.m11*src.m22 + src.m10*src.m21*src.m02 + src.m20*src.m01*src.m12 - src.m00*src.m21*src.m12 - src.m20*src.m11*src.m02 - src.m10*src.m01*src.m22;
			if(0.0 >= fabs(det))
			{
				return src;
			}

			const DsMat33<TYPE> dst =
			{
				(src.m11*src.m22-src.m12*src.m21)/det,	(src.m02*src.m21-src.m01*src.m22)/det,	(src.m01*src.m12-src.m02*src.m11)/det,
				(src.m12*src.m20-src.m10*src.m22)/det,	(src.m00*src.m22-src.m02*src.m20)/det,	(src.m02*src.m10-src.m00*src.m12)/det,
				(src.m10*src.m21-src.m11*src.m20)/det,	(src.m01*src.m20-src.m00*src.m21)/det,	(src.m00*src.m11-src.m01*src.m10)/det,
			};
			return dst;
		}

		static DsMat33<TYPE> RotateX(TYPE ang)
		{
			const DsMat33<TYPE> dst =
			{
				1.0, 0.0, 0.0,
				0.0, cos(ang), -sin(ang),
				0.0, sin(ang), cos(ang),
			};
			return dst;
		}

		static DsMat33<TYPE> RotateY(TYPE ang)
		{
			const DsMat33<TYPE> dst =
			{
				cos(ang), 0.0, sin(ang),
				0.0, 1.0, 0.0,
				-sin(ang), 0.0, cos(ang),
			};
			return dst;
		}

		static DsMat33<TYPE> RotateZ(TYPE ang)
		{
			const DsMat33<TYPE> dst =
			{
				cos(ang), -sin(ang), 0.0,
				sin(ang), cos(ang), 0.0,
				0.0, 0.0, 1.0,
			};
			return dst;
		}

		//vecの長さが角度
		static DsMat33<TYPE> RotateVec( const DsVec3<TYPE>& vec )
		{
			const DsVec3<TYPE> n = DsVec3<TYPE>::Normalize(vec);
			const TYPE r = vec.Length();
			const DsMat33<TYPE> dst =
			{
				static_cast<TYPE>(n.x*n.x*(1.0 - cos(r)) + cos(r)),		static_cast<TYPE>(n.x*n.y*(1.0 - cos(r)) - n.z*sin(r)),	static_cast<TYPE>(n.z*n.x*(1.0 - cos(r)) + n.y*sin(r)),
				static_cast<TYPE>(n.x*n.y*(1.0 - cos(r)) + n.z*sin(r)), static_cast<TYPE>(n.y*n.y*(1.0 - cos(r)) + cos(r)),		static_cast<TYPE>(n.y*n.z*(1.0 - cos(r)) - n.x*sin(r)),
				static_cast<TYPE>(n.z*n.x*(1.0 - cos(r)) - n.y*sin(r)), static_cast<TYPE>(n.y*n.z*(1.0 - cos(r)) + n.x*sin(r)), static_cast<TYPE>(n.z*n.z*(1.0 - cos(r)) + cos(r)),
			};
			return dst;
		}

		//nは正規化ずみ
		static DsMat33d RotateAxis(const DsVec3<TYPE>& n, TYPE r)
		{
			const DsMat33<TYPE> dst =
			{
				static_cast<TYPE>(n.x*n.x*(1.0 - cos(r)) + cos(r)),	     static_cast<TYPE>(n.x*n.y*(1.0 - cos(r)) - n.z*sin(r)),	static_cast<TYPE>(n.z*n.x*(1.0 - cos(r)) + n.y*sin(r)),
				static_cast<TYPE>(n.x*n.y*(1.0 - cos(r)) + n.z*sin(r)),  static_cast<TYPE>(n.y*n.y*(1.0 - cos(r)) + cos(r)),		static_cast<TYPE>(n.y*n.z*(1.0 - cos(r)) - n.x*sin(r)),
				static_cast<TYPE>(n.z*n.x*(1.0 - cos(r)) - n.y*sin(r)),  static_cast<TYPE>(n.y*n.z*(1.0 - cos(r)) + n.x*sin(r)),    static_cast<TYPE>(n.z*n.z*(1.0 - cos(r)) + cos(r)),
			};
			return dst;
		}

		static DsMat33<TYPE> ToMat33( const TYPE mat16[16] )
		{
			const DsMat33<TYPE> dst =
			{
				mat16[0], mat16[1], mat16[2],
				mat16[4], mat16[5], mat16[6],
				mat16[8], mat16[9], mat16[10],
			};
			return dst;
		}

		static DsMat33<TYPE> ToMat33(const TYPE mat33[3][3])
		{
			const DsMat33<TYPE> dst =
			{
				mat33[0][0], mat33[0][1], mat33[0][2],
				mat33[1][0], mat33[1][1], mat33[1][2],
				mat33[2][0], mat33[2][1], mat33[2][2],
			};
			return dst;
		}

		static void ToMat16(const DsMat33<TYPE>& src, TYPE dst[16])
		{
			dst[0] = src.m00; dst[1] = src.m01; dst[2] = src.m02;
			dst[4] = src.m10; dst[5] = src.m11; dst[6] = src.m12;
			dst[8] = src.m20; dst[9] = src.m21; dst[10] = src.m22;
		}
	};

	inline DsMat33f ToMat33f(const DsMat33d& src )
	{
		const DsMat33f dst =
		{
			static_cast<float>(src.m00), static_cast<float>(src.m01), static_cast<float>(src.m02),
			static_cast<float>(src.m10), static_cast<float>(src.m11), static_cast<float>(src.m12),
			static_cast<float>(src.m20), static_cast<float>(src.m21), static_cast<float>(src.m22),
		};
		return dst;
	}

	inline DsMat33d ToMat33d(const DsMat33f& src )
	{
		const DsMat33d dst =
		{
			static_cast<double>(src.m00), static_cast<double>(src.m01), static_cast<double>(src.m02),
			static_cast<double>(src.m10), static_cast<double>(src.m11), static_cast<double>(src.m12),
			static_cast<double>(src.m20), static_cast<double>(src.m21), static_cast<double>(src.m22),
		};
		return dst;
	}


	////n*nのマトリクス
	class DsMat
	{
	public:
		explicit DsMat(int n)
		:m_size(n)
		,m_pMat(0)
		{
			if( n > 0)
			{
				m_pMat = new double[n*n];
				assert(m_pMat!=0 && "メモリ確保失敗");
			}
			else
			{
				assert(0 && "マトリクスのサイズが0以下");
			}
		}

		~DsMat()
		{
			delete [] m_pMat;
			m_pMat = 0;
		};

		int Dm()const{return m_size;} 

		double& Ref(int col, int row )
		{
			assert( ( (col<m_size) && (row<m_size) ) && "サイズ不一致\n");
			return m_pMat[col*m_size + row];
		}

		double& Ref(int col, int row ) const
		{
			assert( ( (col<m_size) && (row<m_size) ) && "サイズ不一致\n");
			return m_pMat[col*m_size + row];
		}

		void Set( DsVec& v, int col )
		{
			assert( ( (col<m_size) && (v.Dm()==Dm()) ) && "サイズ不一致\n");
			for(int i=0; v.Dm()>i; ++i )
			{
				m_pMat[col*m_size + i] = v[i];
			}
		}

		static void Multi(DsVec& dst, const DsMat& mat, const DsVec& src )
		{
			assert( (dst.Dm() == mat.Dm()) && (mat.Dm() == src.Dm()) && "サイズ不一致\n");
			
			for(int col=0; mat.Dm() > col; ++col)
			{
				dst[col] = 0.0;
				for(int row=0; mat.Dm() > row; ++row)
				{
					dst[col] += mat.Ref(col,row) * src[row];
				}
			}
		}


	private:
		const int m_size;
		double* m_pMat;
	};

}

#endif