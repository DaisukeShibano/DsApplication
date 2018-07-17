#ifndef _DS_MATRIX33D_H_
#define _DS_MATRIX33D_H_

namespace DsLib
{

	/*
	コピーの方が早い
	*/


	struct DsMat33d
	{
		union
		{
			double mat[9];
			struct
			{
				double m00, m01, m02;
				double m10, m11, m12;
				double m20, m21, m22;
			};
		};

		//void operator =( const DsMat33d& src )
		//{
		//	for(int i=0; 9>i; ++i)
		//	{
		//		mat[i] = src.mat[i];
		//	}
		//}

		double& operator[](const int index )
		{
			DS_ASSERT( (index < 9), "indexの値がサイズを超えています\n" );
			return mat[ index ];
		}

		const double& operator[](const int index ) const
		{
			DS_ASSERT( (index < 9), "indexの値がサイズを超えています\n" );
			return mat[ index ];
		}

		DsMat33d operator *(const DsMat33d& src ) const
		{
#ifdef DS_SYS_USE_SIMD_
			const __m256d a1 = { m00, m10, m20, 0 }; //_mm256_set_pd(0, m20, m10, m00);
			const __m256d a2 = { m01, m11, m21, 0 }; //_mm256_set_pd(0, m21, m11, m01);
			const __m256d a3 = { m02, m12, m22, 0 }; //_mm256_set_pd(0, m22, m12, m02);
			//__m256d b1 = { src.m00, src.m00, src.m00, 0 };
			
			__m256d res1 = _mm256_mul_pd(a1, _mm256_set1_pd(src.m00));

			//b1.m256d_f64[0]=src.m10; b1.m256d_f64[1]=src.m10; b1.m256d_f64[2]=src.m10;
			__m256d res2 = _mm256_mul_pd(a2, _mm256_set1_pd(src.m10));

			//b1.m256d_f64[0]=src.m20; b1.m256d_f64[1]=src.m20; b1.m256d_f64[2]=src.m20;
			__m256d res3 = _mm256_mul_pd(a3, _mm256_set1_pd(src.m20));

			const __m256d dst1 = _mm256_add_pd(_mm256_add_pd(res1, res2), res3);


			//b1.m256d_f64[0] = src.m01; b1.m256d_f64[1] = src.m01; b1.m256d_f64[2] = src.m01;
			res1 = _mm256_mul_pd(a1, _mm256_set1_pd(src.m01));

			//b1.m256d_f64[0]=src.m11; b1.m256d_f64[1]=src.m11; b1.m256d_f64[2]=src.m11;
			res2 = _mm256_mul_pd(a2, _mm256_set1_pd(src.m11));

			//b1.m256d_f64[0]=src.m21; b1.m256d_f64[1]=src.m21; b1.m256d_f64[2]=src.m21;
			res3 = _mm256_mul_pd(a3, _mm256_set1_pd(src.m21));

			const __m256d dst2 = _mm256_add_pd(_mm256_add_pd(res1, res2), res3);


			//b1.m256d_f64[0] = src.m02; b1.m256d_f64[1] = src.m02; b1.m256d_f64[2] = src.m02;
			res1 = _mm256_mul_pd(a1, _mm256_set1_pd(src.m02));

			//b1.m256d_f64[0] = src.m12; b1.m256d_f64[1] = src.m12; b1.m256d_f64[2] = src.m12;
			res2 = _mm256_mul_pd(a2, _mm256_set1_pd(src.m12));

			//b1.m256d_f64[0] = src.m22; b1.m256d_f64[1] = src.m22; b1.m256d_f64[2] = src.m22;
			res3 = _mm256_mul_pd(a3, _mm256_set1_pd(src.m22));

			const __m256d dst3 = _mm256_add_pd(_mm256_add_pd(res1, res2), res3);

			const DsMat33d dst = 
			{
				dst1.m256d_f64[0], dst2.m256d_f64[0], dst3.m256d_f64[0],
				dst1.m256d_f64[1], dst2.m256d_f64[1], dst3.m256d_f64[1],
				dst1.m256d_f64[2], dst2.m256d_f64[2], dst3.m256d_f64[2],
			};
			return dst;
#else
			DsMat33d dst = 
			{
				m00*src.m00 + m01*src.m10 + m02*src.m20,	m00*src.m01 + m01*src.m11 + m02*src.m21,	m00*src.m02 + m01*src.m12 + m02*src.m22,
				m10*src.m00 + m11*src.m10 + m12*src.m20,	m10*src.m01 + m11*src.m11 + m12*src.m21,	m10*src.m02 + m11*src.m12 + m12*src.m22,
				m20*src.m00 + m21*src.m10 + m22*src.m20,	m20*src.m01 + m21*src.m11 + m22*src.m21,	m20*src.m02 + m21*src.m12 + m22*src.m22,
			};
			return dst;
#endif
		}

		DsVec3d operator *(const DsVec3d& src ) const
		{
#ifdef DS_SYS_USE_SIMD_
			const __m256d a1 ={m00, m10, m20, 0};
			const __m256d a2 ={m01, m11, m21, 0};
			const __m256d a3 ={m02, m12, m22, 0};
			const __m256d b1 ={src.x, src.x, src.x, 0};
			const __m256d b2 ={src.y, src.y, src.y, 0};
			const __m256d b3 ={src.z, src.z, src.z, 0};
			const __m256d res1 = _mm256_mul_pd(a1, b1);
			const __m256d res2 = _mm256_mul_pd(a2, b2);
			const __m256d res3 = _mm256_mul_pd(a3, b3);
			const __m256d res4 = _mm256_add_pd(_mm256_add_pd(res1, res2), res3);
			const DsVec3d dst =
			{
				res4.m256d_f64[0],
				res4.m256d_f64[1],
				res4.m256d_f64[2],
			};
			return dst;
#else
			DsVec3d dst =
			{
				m00*src.x + m01*src.y + m02*src.z,
				m10*src.x + m11*src.y + m12*src.z,
				m20*src.x + m21*src.y + m22*src.z,
			};
			return dst;
#endif
		}

		DsMat33d operator +(const DsMat33d& src) const
		{
			DsMat33d dst =
			{
				m00+src.m00, m01+src.m01, m02+src.m02, 
				m10+src.m10, m11+src.m11, m12+src.m12, 
				m20+src.m20, m21+src.m21, m22+src.m22, 
			};
			return dst;
		}

		DsMat33d operator -(const DsMat33d& src) const
		{
			DsMat33d dst =
			{
				m00-src.m00, m01-src.m01, m02-src.m02, 
				m10-src.m10, m11-src.m11, m12-src.m12, 
				m20-src.m20, m21-src.m21, m22-src.m22, 
			};
			return dst;
		}

		DsMat33d& operator +=(const DsMat33d& src)
		{
			m00+=src.m00; m01+=src.m01; m02+=src.m02; 
			m10+=src.m10; m11+=src.m11; m12+=src.m12; 
			m20+=src.m20; m21+=src.m21; m22+=src.m22; 	
			return (*this);
		}

		DsMat33d& operator -=(const DsMat33d& src)
		{
			m00-=src.m00; m01-=src.m01; m02-=src.m02; 
			m10-=src.m10; m11-=src.m11; m12-=src.m12; 
			m20-=src.m20; m21-=src.m21; m22-=src.m22; 	
			return (*this);
		}

		DsMat33d MultiDiag(const double val) const
		{
			DsMat33d dst =
			{
				m00*val, m01, m02,
				m10, m11*val, m12,
				m20, m21, m22*val,
			};
			return dst;
		}

		DsVec3d GetAxisX() const
		{
			DsVec3d dst = { m00, m10, m20};
			return dst;
		}

		DsVec3d GetAxisY() const
		{
			DsVec3d dst = { m01, m11, m21};
			return dst;
		}

		DsVec3d GetAxisZ() const
		{
			DsVec3d dst = { m02, m12, m22};
			return dst;
		}

		DsMat33d ToTransposition() const
		{
			const DsMat33d dst =
			{
				m00, m10, m20,
				m01, m11, m21,
				m02, m12, m22,
			};
			return dst;
		}

		bool IsNearEqual(const DsMat33d& src) const
		{
			const double err = (0.0000001);
			for (int i = 0; i < 9; ++i)
			{
				const double sub = mat[i] - src.mat[i];
				if ((sub > err) || (sub < -err))
				{
					return false;
				}
			}
			return true;
		}

		bool IsNearEqual(const DsMat33d& src, const double err) const
		{
			for (int i = 0; i < 9; ++i)
			{
				const double sub = mat[i] - src.mat[i];
				if ((sub > err) || (sub < -err))
				{
					return false;
				}
			}
			return true;
		}

		static DsMat33d SetAxis( const DsVec3d& x, const DsVec3d& y, const DsVec3d& z )
		{
			DsMat33d dst =
			{
				x.x, y.x, z.x,
				x.y, y.y, z.y,
				x.z, y.z, z.z,
			};
			return dst;
		}


		static DsMat33d Identity()
		{
			DsMat33d dst =
			{
				1.0, 0.0, 0.0,
				0.0, 1.0, 0.0,
				0.0, 0.0, 1.0,
			};
			return dst;
		}

		static DsMat33d Inverse(const DsMat33d& src)
		{
			double det = src.m00*src.m11*src.m22 + src.m10*src.m21*src.m02 + src.m20*src.m01*src.m12 - src.m00*src.m21*src.m12 - src.m20*src.m11*src.m02 - src.m10*src.m01*src.m22;
			if(0.0 >= fabs(det))
			{
				return src;
			}

			const DsMat33d dst =
			{
				(src.m11*src.m22-src.m12*src.m21)/det,	(src.m02*src.m21-src.m01*src.m22)/det,	(src.m01*src.m12-src.m02*src.m11)/det,
				(src.m12*src.m20-src.m10*src.m22)/det,	(src.m00*src.m22-src.m02*src.m20)/det,	(src.m02*src.m10-src.m00*src.m12)/det,
				(src.m10*src.m21-src.m11*src.m20)/det,	(src.m01*src.m20-src.m00*src.m21)/det,	(src.m00*src.m11-src.m01*src.m10)/det,
			};
			return dst;
		}

		static DsMat33d RotateX(double rad)
		{
			const DsMat33d dst =
			{
				1.0, 0.0, 0.0,
				0.0, cos(rad), -sin(rad),
				0.0, sin(rad), cos(rad),
			};
			return dst;
		}

		static DsMat33d RotateY(double rad)
		{
			const DsMat33d dst =
			{
				cos(rad), 0.0, sin(rad),
				0.0, 1.0, 0.0,
				-sin(rad), 0.0, cos(rad),
			};
			return dst;
		}

		static DsMat33d RotateZ(double rad)
		{
			const DsMat33d dst =
			{
				cos(rad), -sin(rad), 0.0,
				sin(rad), cos(rad), 0.0,
				0.0, 0.0, 1.0,
			};
			return dst;
		}

		//vecの長さが角度
		static DsMat33d RotateVec( const DsVec3d& vec )
		{
			const DsVec3d n = DsVec3d::Normalize(vec);
			const double r = vec.Length();
			const DsMat33d dst =
			{
				(n.x*n.x*(1.0 - cos(r)) + cos(r)),	   (n.x*n.y*(1.0 - cos(r)) - n.z*sin(r)),	(n.z*n.x*(1.0 - cos(r)) + n.y*sin(r)),
				(n.x*n.y*(1.0 - cos(r)) + n.z*sin(r)), (n.y*n.y*(1.0 - cos(r)) + cos(r)),		(n.y*n.z*(1.0 - cos(r)) - n.x*sin(r)),
				(n.z*n.x*(1.0 - cos(r)) - n.y*sin(r)), (n.y*n.z*(1.0 - cos(r)) + n.x*sin(r)),   (n.z*n.z*(1.0 - cos(r)) + cos(r)),
			};
			return dst;
		}

		static DsMat33d ToMat33( const double mat16[16] )
		{
			const DsMat33d dst =
			{
				mat16[0], mat16[1], mat16[2],
				mat16[4], mat16[5], mat16[6],
				mat16[8], mat16[9], mat16[10],
			};
			return dst;
		}

		static DsMat33d ToMat33(const double mat33[3][3])
		{
			const DsMat33d dst =
			{
				mat33[0][0], mat33[0][1], mat33[0][2],
				mat33[1][0], mat33[1][1], mat33[1][2],
				mat33[2][0], mat33[2][1], mat33[2][2],
			};
			return dst;
		}
		static DsMat33d ToMat33(const double mat33[3][4])
		{
			const DsMat33d dst =
			{
				mat33[0][0], mat33[0][1], mat33[0][2],
				mat33[1][0], mat33[1][1], mat33[1][2],
				mat33[2][0], mat33[2][1], mat33[2][2],
			};
			return dst;
		}
		static DsMat33d ToMat33(const double mat33[6][6])
		{
			const DsMat33d dst =
			{
				mat33[0][0], mat33[0][1], mat33[0][2],
				mat33[1][0], mat33[1][1], mat33[1][2],
				mat33[2][0], mat33[2][1], mat33[2][2],
			};
			return dst;
		}
		static void ToMat16(const DsMat33d& src, double dst[16])
		{
			dst[0] = src.m00; dst[1] = src.m01; dst[2] = src.m02;
			dst[4] = src.m10; dst[5] = src.m11; dst[6] = src.m12;
			dst[8] = src.m20; dst[9] = src.m21; dst[10] = src.m22;
		}
	};
}

#endif