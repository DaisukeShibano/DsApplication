#ifndef _DS_VECTOR3D_H_
#define _DS_VECTOR3D_H_


namespace DsLib
{
	struct DsVec3f;
	struct DsVec4d;
}

namespace DsLib
{
	struct DsVec3d
	{
#define _DS_SHUFFLE(X,Y,Z,W) _MM_SHUFFLE(W,Z,Y,X)
		union
		{
#ifdef DS_SYS_USE_SIMD_
			__m256d m256d;
			//__m256 m256;
#endif
			double v[4];
			struct
			{
				double x;
				double y;
				double z;
				double ___w___;
			};
		};

		DsVec3d(double setX, double setY, double setZ)
			: x(setX)
			, y(setY)
			, z(setZ)
			, ___w___(0)
		{}
		DsVec3d(){}

		inline DsVec3d(const DsVec3f& src);

		inline DsVec3d& operator =(const DsVec3f& src);

		void Set(const double setX, const double setY, const double setZ)
		{
			x = setX;
			y = setY;
			z = setZ;
		}

		DsVec3d operator +(const DsVec3d& src) const
		{
#ifdef DS_SYS_USE_SIMD_
			const __m256d res = _mm256_add_pd(m256d, src.m256d);
			return DsVec3d(res.m256d_f64[0], res.m256d_f64[1], res.m256d_f64[2]);
#else
			const DsVec3d dst = { x + src.x, y + src.y, z + src.z };
			return dst;
#endif
		}

		DsVec3d operator -(const DsVec3d& src) const
		{
#ifdef DS_SYS_USE_SIMD_
			const __m256d res = _mm256_sub_pd(m256d, src.m256d);
			return DsVec3d(res.m256d_f64[0], res.m256d_f64[1], res.m256d_f64[2]);
#else
			const DsVec3d dst = { x - src.x, y - src.y, z - src.z };
			return dst;
#endif
		}

		DsVec3d operator *(const double& src) const
		{
#ifdef DS_SYS_USE_SIMD_
			const __m256d srcV = _mm256_set1_pd(src);
			const __m256d res = _mm256_mul_pd(m256d, srcV);
			return DsVec3d(res.m256d_f64[0], res.m256d_f64[1], res.m256d_f64[2]);
#else
			const DsVec3d dst = { x*src, y*src, z*src };
			return dst;
#endif
		}

		DsVec3d operator /(const double& src) const
		{
#ifdef DS_SYS_USE_SIMD_
			const __m256d srcV = _mm256_set1_pd(src);
			const __m256d res = _mm256_div_pd(m256d, srcV);
			return DsVec3d(res.m256d_f64[0], res.m256d_f64[1], res.m256d_f64[2]);
#else
			const DsVec3d dst = { x / src, y / src, z / src };
			return dst;
#endif
		}

		DsVec3d operator -() const
		{
			const DsVec3d dst = { -x, -y, -z };
			return dst;
		}

		DsVec3d& operator +=(const DsVec3d& src)
		{
#ifdef DS_SYS_USE_SIMD_
			const __m256d res = _mm256_add_pd(m256d, src.m256d);
			x=res.m256d_f64[0];
			y=res.m256d_f64[1];
			z=res.m256d_f64[2];
			return (*this);
#else
			x += src.x;
			y += src.y;
			z += src.z;
			return (*this);
#endif
		}

		DsVec3d& operator -=(const DsVec3d& src)
		{
#ifdef DS_SYS_USE_SIMD_
			const __m256d res = _mm256_sub_pd(m256d, src.m256d);
			x=res.m256d_f64[0];
			y=res.m256d_f64[1];
			z=res.m256d_f64[2];
			return (*this);
#else
			x -= src.x;
			y -= src.y;
			z -= src.z;
			return (*this);
#endif
		}

		DsVec3d& operator *=(const double& src)
		{
#ifdef DS_SYS_USE_SIMD_
			const __m256d tmp = _mm256_set1_pd(src);
			const __m256d res = _mm256_mul_pd(m256d, tmp);
			x=res.m256d_f64[0];
			y=res.m256d_f64[1];
			z=res.m256d_f64[2];
			return (*this);
#else
			x *= src;
			y *= src;
			z *= src;
			return (*this);
#endif
		}

		DsVec3d& operator /=(const double& src)
		{
#ifdef DS_SYS_USE_SIMD_
			const __m256d tmp = { src, src, src, 1 };
			const __m256d res = _mm256_div_pd(m256d, tmp);
			x = res.m256d_f64[0];
			y = res.m256d_f64[1];
			z = res.m256d_f64[2];
			return (*this);
#else
			x /= src;
			y /= src;
			z /= src;
			return (*this);
#endif
		}

		inline DsVec3d& operator =(const DsVec4d& src);
		
		double Length() const
		{
#ifdef DS_SYS_USE_SIMD_
			const __m256d res = _mm256_mul_pd(m256d, m256d);
			return sqrt(res.m256d_f64[0] + res.m256d_f64[1] + res.m256d_f64[2]);
#else
			return sqrt(x*x + y*y + z*z);
#endif
		}

		//sqrtしないLength
		double LengthSq() const
		{
#ifdef DS_SYS_USE_SIMD_
			const __m256d res = _mm256_mul_pd(m256d, m256d);
			return res.m256d_f64[0] + res.m256d_f64[1] + res.m256d_f64[2];
#else
			return x*x + y*y + z*z;
#endif
		}

		//dirベクトル成分をなくす
		DsVec3d Flat(const DsVec3d& dir) const
		{
			const DsVec3d normal = Normalize(dir);
			const double len = Dot(*this, normal);
			const DsVec3d flat = (*this) - (normal*len);
			return flat;
		}

		bool IsNearZero(const double zero = (0.0001)) const
		{
#ifdef DS_SYS_USE_SIMD_
			const __m256d fabsV = _mm256_andnot_pd(_mm256_set1_pd(-0.0), m256d);//最上位ビットだけクリアして絶対値とる
			const __m256d mask = _mm256_cmp_pd(fabsV, _mm256_set1_pd(zero), _CMP_LT_OS);
			return mask.m256d_f64[0] && mask.m256d_f64[1] && mask.m256d_f64[2];
#else
			return	(fabs(x) < zero) && (fabs(y) < zero) && (fabs(z) < zero);
#endif
		}

		//このベクトルに垂直な平面上のベクトルを取得
		void GetVerticalPlane(DsVec3d& cX, DsVec3d& cY) const
		{
			const DsVec3d cZ = DsVec3d::Abs(DsVec3d::Normalize(*this));
			if ((cZ - DsVec3d::GetY()).IsNearZero((0.1))){
				cX = DsVec3d::Normalize(DsVec3d::Cross(-DsVec3d::GetZ(), cZ));
			}
			else{
				cX = DsVec3d::Normalize(DsVec3d::Cross(DsVec3d::GetY(), cZ));
			}
			cY = DsVec3d::Normalize(DsVec3d::Cross(cZ, cX));
		}

		static DsVec3d Abs(const DsVec3d& a)
		{
#ifdef DS_SYS_USE_SIMD_
			const __m256d fabsV = _mm256_andnot_pd(_mm256_set1_pd(-0.0), a.m256d);//最上位ビットだけクリアして絶対値とる
			return DsVec3d(fabsV.m256d_f64[0], fabsV.m256d_f64[1], fabsV.m256d_f64[2]);
#else
			return DsVec3d(fabs(a.x), fabs(a.y), fabs(a.z));
#endif
		}

		static DsVec3d Clamp3(const DsVec3d& v, double min, double max)
		{
			return DsVec3d(min(max(min, v.x), max), min(max(min, v.y), max), min(max(min, v.z), max));
		}

		static DsVec3d Normalize(const DsVec3d& src)
		{
			const double len = src.Length();
			if ((0.000001) >= len)
			{
				const DsVec3d dst = { 0, 0, 0 };
				return dst;
			}
			else
			{
#ifdef DS_SYS_USE_SIMD_
				const __m256d tmp = _mm256_set1_pd(len);
				const __m256d res = _mm256_div_pd(src.m256d, tmp);
				return DsVec3d(res.m256d_f64[0], res.m256d_f64[1], res.m256d_f64[2]);
#else
				const DsVec3d dst = { src.x / len, src.y / len, src.z / len };
				return dst;
#endif
			}
		}

		static DsVec3d ToVec3(double x, double y, double z)
		{
			const DsVec3d dst = { x, y, z };
			return dst;
		}

		static DsVec3d ToVec3(const double* v)
		{
			const DsVec3d dst = { v[0], v[1], v[2] };
			return dst;
		}

		static DsVec3d ToVec3_forMat16(const double* v)
		{
			const DsVec3d dst = { v[3], v[7], v[11] };
			return dst;
		}

		static DsVec3d Up()
		{
			return DsVec3d::GetY();
		}

		static DsVec3d Div(const DsVec3d& src1, const DsVec3d& src2)
		{
#ifdef DS_SYS_USE_SIMD_
			const __m256d res = _mm256_div_pd(src1.m256d, src2.m256d);
			return DsVec3d(res.m256d_f64[0], res.m256d_f64[1], res.m256d_f64[2]);
#else
			const DsVec3d dst = { src1.x / src2.x, src1.y / src2.y, src1.z / src2.z };
			return dst;
#endif
		}

		static double Dot(const DsVec3d& src1, const DsVec3d& src2)
		{
#ifdef DS_SYS_USE_SIMD_
			__m256d res = _mm256_mul_pd(src1.m256d, src2.m256d);
			return res.m256d_f64[0] + res.m256d_f64[1] + res.m256d_f64[2];
#else
			return (src1.x*src2.x + src1.y*src2.y + src1.z*src2.z);
#endif
		}

		static DsVec3d Cross(const DsVec3d& src1, const DsVec3d& src2)
		{
#ifdef DS_SYS_USE_SIMD_
			const __m256d a1 = { src1.y, src1.z, src1.x};
			const __m256d b1 = { src2.z, src2.x, src2.y};

			const __m256d a2 = { src1.z, src1.x, src1.y };
			const __m256d b2 = { src2.y, src2.z, src2.x };

			
			//const __m256d v1 = _mm256_set_pd(src1.x, src1.y, src1.z, 0);
			//const __m256d v2 = _mm256_set_pd(src2.x, src2.y, src2.z, 0);
			//
			//const __m256d a1 = _mm256_shuffle_pd(v1, v1, _DS_SHUFFLE(1, 2, 0, 0));
			//const __m256d b1 = _mm256_shuffle_pd(v2, v2, _DS_SHUFFLE(2, 0, 1, 0));
			//
			//const __m256d a2 = _mm256_shuffle_pd(v1, v1, _DS_SHUFFLE(2, 0, 1, 0));
			//const __m256d b2 = _mm256_shuffle_pd(v2, v2, _DS_SHUFFLE(1, 2, 0, 0));

			//const __m256d res = _mm256_sub_pd(_mm256_mul_pd(_mm256_shuffle_pd(v1, v1, _DS_SHUFFLE(1, 2, 0, 0)), _mm256_shuffle_pd(v2, v2, _DS_SHUFFLE(2, 0, 1, 0))),
			//                                  _mm256_mul_pd(_mm256_shuffle_pd(v1, v1, _DS_SHUFFLE(2, 0, 1, 0)), _mm256_shuffle_pd(v2, v2, _DS_SHUFFLE(1, 2, 0, 0))) );
			const __m256d res = _mm256_sub_pd(_mm256_mul_pd(a1,b1), _mm256_mul_pd(a2,b2) );
			return DsVec3d(res.m256d_f64[0], res.m256d_f64[1], res.m256d_f64[2]);
#else
			const DsVec3d dst =
			{
				src1.y*src2.z - src1.z*src2.y,
				src1.z*src2.x - src1.x*src2.z,
				src1.x*src2.y - src1.y*src2.x,
			};
			return dst;
#endif
		}

		//反時計回りにp1,p2,p3
		static DsVec3d CalcNormal(const DsVec3d& p1, const DsVec3d& p2, const DsVec3d& p3)
		{
			return Normalize(Cross(p2 - p1, p3 - p1));
		}

		static const DsVec3d& GetX()
		{
			static const DsVec3d dst = { 1, 0, 0 };
			return dst;
		}

		static const DsVec3d& GetY()
		{
			static const DsVec3d dst = { 0, 1, 0 };
			return dst;
		}

		static const DsVec3d& GetZ()
		{
			static const DsVec3d dst = { 0, 0, 1 };
			return dst;
		}

		static const DsVec3d& Zero()
		{
			static const DsVec3d dst = { 0, 0, 0 };
			return dst;
		}

		static const DsVec3d& Infinity()
		{
			static const DsVec3d dst = { (FLT_MAX), (FLT_MAX), (FLT_MAX) };
			return dst;
		}

		/*
		@param s1 線分１の始点座標
		@param v1 線分１のベクトル
		@param s2 線分２の始点座標
		@param v2 線分２のベクトル
		@par aとbが交差する座標を求める。求まらなかったらfalseを返す
		*/
		static bool GetIntersection(const DsVec3d& s1, const DsVec3d& v1,
			const DsVec3d& s2, const DsVec3d& v2,
			DsVec3d& dst)
		{
			//http://marupeke296.com/COL_2D_No10_SegmentAndSegment.html
			const DsVec3d v = s2 - s1;
			const double cross = Cross(v1, v2).Length();
			if ((0.0) == cross)
			{
				return false;
			}

			const double t2 = Cross(v, v1).Length() / cross;
			const double t1 = Cross(v, v2).Length() / cross;

			if ((((0.0) <= t1) && (t1 <= (1.0))) &&
				(((0.0) <= t2) && (t2 <= (1.0)))
				)
			{
				dst = s1 + (v1*t1);
				return true;
			}
			else
			{
				return false;
			}

		}
	};
}

#endif