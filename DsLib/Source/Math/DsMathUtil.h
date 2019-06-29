#ifndef __DS_MATH_UTIL__
#define __DS_MATH_UTIL__

#ifndef _DS_QUQTERNION_
#include "Math/DsQuaternion.h"
#endif

#ifndef _DS_MATRIX33_H_
#include "Math/DsMatrix33.h"
#endif

#ifndef _DS_MATRIX33D_H_
#include "Math/DsMatrix33d.h"
#endif

#ifndef _DS_MATRIX44F_H_
#include "Math/DsMatrix44f.h"
#endif

#ifndef _DS_MATRIX44D_H_
#include "Math/DsMatrix44d.h"
#endif


#define RadToDeg(a)(a*static_cast<double>(180.0 / M_PI))
#define DegToRad(a)(a*static_cast<double>(M_PI / 180.0))
#define Clamp(val, _min, _max)( min(max( (_min), (val) ), (_max) ) )
#define DsACos(x)( acos( Clamp( (x), -1.0, 1.0 ) ) )
#define DsASin(x)( asin( Clamp( (x), -1.0, 1.0 ) ) )
#define DsATan(y, x)( atan2( y, x) )

namespace DsLib
{
	namespace DsMathUtil
	{
		extern double DS_INFINITY_D;

		template<int N>
		void MultiMat(double dst[N][N], const double src1[N][N], const double src2[N][N])
		{
			DS_ASSERT(&dst[0][0] != &src1[0][0], "出力と入力は同じ違うインスタンスにしてください");
			DS_ASSERT(&dst[0][0] != &src2[0][0], "出力と入力は同じ違うインスタンスにしてください");
			for (int i = 0; N>i; ++i)
			{
				for (int j = 0; N>j; ++j)
				{
					dst[i][j] = 0.0;
					for (int k = 0; N>k; ++k)
					{
						dst[i][j] += src1[i][k] * src2[k][j];
					}
				}
			}
		}

		template<int ROW, int COL>
		void MultiMat(double dst[ROW][COL], const double src1[ROW][12], const double src2[12][COL])
		{
			DS_ASSERT(&dst[0][0] != &src1[0][0], "出力と入力は同じ違うインスタンスにしてください");
			DS_ASSERT(&dst[0][0] != &src2[0][0], "出力と入力は同じ違うインスタンスにしてください");
			for (int i = 0; ROW>i; ++i)
			{
				for (int j = 0; COL>j; ++j)
				{
					dst[i][j] = 0.0;
					for (int k = 0; 12>k; ++k)
					{
						dst[i][j] += src1[i][k] * src2[k][j];
					}
				}
			}
		}


		template<int ROW, int COL>
		void MultiMat(double dst[ROW][COL], const double src1[ROW][6], const double src2[6][COL])
		{
			DS_ASSERT(&dst[0][0] != &src1[0][0], "出力と入力は同じ違うインスタンスにしてください");
			DS_ASSERT(&dst[0][0] != &src2[0][0], "出力と入力は同じ違うインスタンスにしてください");
			for (int i = 0; ROW>i; ++i)
			{
				for (int j = 0; COL>j; ++j)
				{
					dst[i][j] = 0.0;
					for (int k = 0; 6>k; ++k)
					{
						dst[i][j] += src1[i][k] * src2[k][j];
					}
				}
			}
		}

		template<int ROW, int COL, int N>
		void MultiMatMN(double dst[ROW][COL], const double src1[ROW][N], const double src2[N][COL])
		{
			DS_ASSERT(&dst[0][0] != &src1[0][0],  "出力と入力は同じ違うインスタンスにしてください");
			DS_ASSERT(&dst[0][0] != &src2[0][0],  "出力と入力は同じ違うインスタンスにしてください");
			for (int i = 0; ROW>i; ++i)
			{
				for (int j = 0; COL>j; ++j)
				{
					dst[i][j] = 0.0;
					for (int k = 0; N>k; ++k)
					{
						dst[i][j] += src1[i][k] * src2[k][j];
					}
				}
			}
		}

		template<int ROW, int COL>
		void MultiVec(double dst[ROW], const double src1[ROW][COL], const double src2[COL])
		{
			DS_ASSERT(&dst[0] != &src2[0], "出力と入力は同じ違うインスタンスにしてください");
			for (int i = 0; ROW>i; ++i)
			{
				dst[i] = 0.0;
				for (int j = 0; COL>j; ++j)
				{
					dst[i] += src1[i][j] * src2[j];
				}
			}
		}

		template<int ROW >
		void MultiVec(double& dst, const double src1[ROW], const double src2[ROW])
		{
			dst = 0.0;
			for (int i = 0; ROW>i; ++i)
			{
				dst += src1[i] * src2[i];
			}
		}

		inline
		DsMat44f ToMat44f(const DsMat44d& src)
		{
			const DsMat44f ret =
			{
				static_cast<float>(src.m00), static_cast<float>(src.m01), static_cast<float>(src.m02), static_cast<float>(src.m03),
				static_cast<float>(src.m10), static_cast<float>(src.m11), static_cast<float>(src.m12), static_cast<float>(src.m13),
				static_cast<float>(src.m20), static_cast<float>(src.m21), static_cast<float>(src.m22), static_cast<float>(src.m23),
				static_cast<float>(src.m30), static_cast<float>(src.m31), static_cast<float>(src.m32), static_cast<float>(src.m33),
			};
			return ret;
		}

	}
}

#endif