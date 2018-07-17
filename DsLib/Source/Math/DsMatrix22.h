#ifndef _DS_MATRIX22_H_
#define _DS_MATRIX22_H_

namespace DsLib
{
	template<typename TYPE>
	struct DsMat22
	{
		union
		{
			TYPE mat[4];
			struct
			{
				TYPE m00, m01;
				TYPE m10, m11;
			};
		};

		//void operator =( const DsMat22<TYPE>& src )
		//{
		//	for(int i=0; 9>i; ++i)
		//	{
		//		mat[i] = src.mat[i];
		//	}
		//}

		TYPE& operator[](const int index )
		{
			DS_ASSERT( (index < 9), "indexの値がサイズを超えています\n" );
			return mat[ index ];
		}

		const TYPE& operator[](const int index ) const
		{
			DS_ASSERT( (index < 9), "indexの値がサイズを超えています\n" );
			return mat[ index ];
		}

		DsMat22<TYPE> operator *(const DsMat22<TYPE>& src ) const
		{
			DsMat22<TYPE> dst = 
			{
				m00*src.m00 + m01*src.m10,	m00*src.m01 + m01*src.m11,
				m10*src.m00 + m11*src.m10,	m10*src.m01 + m11*src.m11,
			};
			return dst;
		}

		DsVec2<TYPE> operator *(const DsVec2<TYPE>& src ) const
		{
			DsVec2<TYPE> dst =
			{
				m00*src.x + m01*src.y,
				m10*src.x + m11*src.y,
			};
			return dst;
		}

		DsMat22<TYPE> operator +(const DsMat22<TYPE>& src) const
		{
			DsMat22<TYPE> dst =
			{
				m00+src.m00, m01+src.m01, 
				m10+src.m10, m11+src.m11, 
			};
			return dst;
		}

		DsMat22<TYPE> operator -(const DsMat22<TYPE>& src) const
		{
			DsMat22<TYPE> dst =
			{
				m00-src.m00, m01-src.m01,
				m10-src.m10, m11-src.m11,
			};
			return dst;
		}

		DsMat22<TYPE>& operator +=(const DsMat22<TYPE>& src)
		{
			m00+=src.m00; m01+=src.m01; 
			m10+=src.m10; m11+=src.m11; 
			return (*this);
		}

		DsMat22<TYPE>& operator -=(const DsMat22<TYPE>& src)
		{
			m00-=src.m00; m01-=src.m01;
			m10-=src.m10; m11-=src.m11;
			return (*this);
		}

		DsMat22<TYPE> MultiDiag(const TYPE val) const
		{
			DsMat22<TYPE> dst =
			{
				m00*val, m01,
				m10, m11*val,
			};
			return dst;
		}

		DsVec2<TYPE> GetAxisX() const
		{
			DsVec2<TYPE> dst = { m00, m10};
			return dst;
		}

		DsVec2<TYPE> GetAxisY() const
		{
			DsVec2<TYPE> dst = { m01, m11};
			return dst;
		}

		DsMat22<TYPE> ToTransposition() const
		{
			const DsMat22<TYPE> dst =
			{
				m00, m10, 
				m01, m11, 
			};
			return dst;
		}

		bool IsNearEqual(const DsMat22<TYPE>& src) const
		{
			const TYPE err = static_cast<TYPE>(0.0000001);
			for (int i = 0; i < 4; ++i)
			{
				const TYPE sub = mat[i] - src.mat[i];
				if ((sub > err) || (sub < -err))
				{
					return false;
				}
			}
			return true;
		}

		bool IsNearEqual(const DsMat22<TYPE>& src, const TYPE err) const
		{
			for (int i = 0; i < 4; ++i)
			{
				const TYPE sub = mat[i] - src.mat[i];
				if ((sub > err) || (sub < -err))
				{
					return false;
				}
			}
			return true;
		}

		static DsMat22<TYPE> SetAxis( const DsVec2<TYPE>& x, const DsVec2<TYPE>& y)
		{
			DsMat22<TYPE> dst =
			{
				x.x, y.x,
				x.y, y.y,
			};
			return dst;
		}


		static DsMat22<TYPE> Identity()
		{
			DsMat22<TYPE> dst =
			{
				1.0, 0.0,
				0.0, 1.0,
			};
			return dst;
		}

		static DsMat22<TYPE> Inverse(const DsMat22<TYPE>& src)
		{
			double det = src.m00*src.m11 - src.m01*src.m10;
			if(0.0 >= fabs(det))
			{
				return src;
			}

			const DsMat22<TYPE> dst =
			{
				src.m11 / det, -src.m01 / det,
				-src.m10 / det, src.m00 / det,
			};
			return dst;
		}

		static DsMat22<TYPE> ToMat22( const TYPE mat4[4] )
		{
			const DsMat22<TYPE> dst =
			{
				mat4[0], mat4[1], 
				mat4[2], mat4[3], 
			};
			return dst;
		}

		static DsMat22<TYPE> ToMat22(const TYPE mat22[2][2])
		{
			const DsMat22<TYPE> dst =
			{
				mat33[0][0], mat33[0][1],
				mat33[1][0], mat33[1][1],
			};
			return dst;
		}

		static void ToMat4(const DsMat22<TYPE>& src, TYPE dst[4])
		{
			dst[0] = src.m00; dst[1] = src.m01; 
			dst[2] = src.m10; dst[3] = src.m11; 
		}
	};

	typedef DsMat22<double>	DsMat22d;
	typedef DsMat22<float>	DsMat22f;

	inline DsMat22f ToMat22f(const DsMat22d& src )
	{
		const DsMat22f dst =
		{
			static_cast<float>(src.m00), static_cast<float>(src.m01),
			static_cast<float>(src.m10), static_cast<float>(src.m11),
		};
		return dst;
	}

	inline DsMat22d ToMat22d(const DsMat22f& src )
	{
		const DsMat22d dst =
		{
			static_cast<double>(src.m00), static_cast<double>(src.m01),
			static_cast<double>(src.m10), static_cast<double>(src.m11),
		};
		return dst;
	}
}

#endif