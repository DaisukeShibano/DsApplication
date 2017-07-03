#ifndef __DS_MATH_UTIL__
#define __DS_MATH_UTIL__

#ifndef _DS_QUQTERNION_
#include "Math/DsQuaternion.h"
#endif

#ifndef _DS_MATRIX33_H_
#include "Math/DsMatrix33.h"
#endif


#define RadToDeg(a)(a*static_cast<double>(180.0f / M_PI))
#define DegToRad(a)(a*static_cast<double>(M_PI / 180.0f))
#define Clamp(val, _min, _max)( min(max(_min, val), _max) )

namespace DsLib
{
	namespace DsMathUtil
	{
		extern double DS_INFINITY_D;
		extern double DS_MAX_MASS;

		template<typename TYPE>
		DsQuaternion<TYPE> ToQuaternion(const DsMat33d& mat)
		{
			DsQuaternion<TYPE> ret;

			// 最大成分を検索
			TYPE elem[4]; // 0:x, 1:y, 2:z, 3:w
			elem[0] = mat.m00 - mat.m11 - mat.m22 + 1.0f;
			elem[1] = -mat.m00 + mat.m11 - mat.m22 + 1.0f;
			elem[2] = -mat.m00 - mat.m11 + mat.m22 + 1.0f;
			elem[3] = mat.m00 + mat.m11 + mat.m22 + 1.0f;

			unsigned biggestIndex = 0;
			for (int i = 1; i < 4; i++) {
				if (elem[i] > elem[biggestIndex])
					biggestIndex = i;
			}

			if (elem[biggestIndex] < 0.0f)
			{
				return ret; // 引数の行列に間違いあり！
			}

			// 最大要素の値を算出
			TYPE *q[4] = { &ret.x, &ret.y, &ret.z, &ret.w };
			TYPE v = static_cast<TYPE>(sqrt(elem[biggestIndex]) * 0.5f);
			*q[biggestIndex] = v;
			TYPE mult = 0.25f / v;

			switch (biggestIndex) {
			case 0: // x
				*q[1] = (mat.m01 + mat.m10) * mult;
				*q[2] = (mat.m20 + mat.m02) * mult;
				*q[3] = (mat.m12 - mat.m21) * mult;
				break;
			case 1: // y
				*q[0] = (mat.m01 + mat.m10) * mult;
				*q[2] = (mat.m12 + mat.m21) * mult;
				*q[3] = (mat.m20 - mat.m02) * mult;
				break;
			case 2: // z
				*q[0] = (mat.m20 + mat.m02) * mult;
				*q[1] = (mat.m12 + mat.m21) * mult;
				*q[3] = (mat.m01 - mat.m10) * mult;
				break;
			case 3: // w
				*q[0] = (mat.m12 - mat.m21) * mult;
				*q[1] = (mat.m20 - mat.m02) * mult;
				*q[2] = (mat.m01 - mat.m10) * mult;
				break;
			}
			return ret;
		}

		template<typename TYPE>
		DsMat33d ToMat33d(const DsQuaternion<TYPE>& q)
		{
			DsMat33d ret;
			const double qx = static_cast<double>(q.x);
			const double qy = static_cast<double>(q.y);
			const double qz = static_cast<double>(q.z);
			const double qw = static_cast<double>(q.w);

			ret.m00 = 1.0f - 2.0f * qy * qy - 2.0f * qz * qz;
			ret.m01 = 2.0f * qx * qy + 2.0f * qw * qz;
			ret.m02 = 2.0f * qx * qz - 2.0f * qw * qy;

			ret.m10 = 2.0f * qx * qy - 2.0f * qw * qz;
			ret.m11 = 1.0f - 2.0f * qx * qx - 2.0f * qz * qz;
			ret.m12 = 2.0f * qy * qz + 2.0f * qw * qx;

			ret.m20 = 2.0f * qx * qz + 2.0f * qw * qy;
			ret.m21 = 2.0f * qy * qz - 2.0f * qw * qx;
			ret.m22 = 1.0f - 2.0f * qx * qx - 2.0f * qy * qy;

			return ret;
		}

		template<typename TYPE>
		DsMat33f ToMat33f(const DsQuaternion<TYPE>& q)
		{
			DsMat33f ret;
			const float qx = static_cast<float>(q.x);
			const float qy = static_cast<float>(q.y);
			const float qz = static_cast<float>(q.z);
			const float qw = static_cast<float>(q.w);

			ret.m00 = 1.0f - 2.0f * qy * qy - 2.0f * qz * qz;
			ret.m01 = 2.0f * qx * qy + 2.0f * qw * qz;
			ret.m02 = 2.0f * qx * qz - 2.0f * qw * qy;

			ret.m10 = 2.0f * qx * qy - 2.0f * qw * qz;
			ret.m11 = 1.0f - 2.0f * qx * qx - 2.0f * qz * qz;
			ret.m12 = 2.0f * qy * qz + 2.0f * qw * qx;

			ret.m20 = 2.0f * qx * qz + 2.0f * qw * qy;
			ret.m21 = 2.0f * qy * qz - 2.0f * qw * qx;
			ret.m22 = 1.0f - 2.0f * qx * qx - 2.0f * qy * qy;

			return ret;
		}


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

	}
}

#endif