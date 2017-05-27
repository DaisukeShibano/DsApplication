#ifndef _DS_VECTOR3F_H_
#define _DS_VECTOR3F_H_

namespace DsLib
{
	struct DsVec3d;
	struct DsVec4f;
}

namespace DsLib
{
	struct DsVec3f
	{
		union
		{
			float v[3];
			struct
			{
				float x;
				float y;
				float z;
			};

		};

		DsVec3f(float setX, float setY, float setZ)
			: x(setX)
			, y(setY)
			, z(setZ)
		{}
		DsVec3f(){}

		inline DsVec3f(const DsVec3d& src);

		inline DsVec3f& operator =(const DsVec3d& src);

		void Set(const float setX, const float setY, const float setZ)
		{
			x = setX;
			y = setY;
			z = setZ;
		}

		DsVec3f operator +(const DsVec3f& src) const
		{
			const DsVec3f dst = { x + src.x, y + src.y, z + src.z };
			return dst;
		}

		DsVec3f operator -(const DsVec3f& src) const
		{
			const DsVec3f dst = { x - src.x, y - src.y, z - src.z };
			return dst;
		}

		DsVec3f operator *(const float& src) const
		{
			const DsVec3f dst = { x*src, y*src, z*src };
			return dst;
		}

		DsVec3f operator /(const float& src) const
		{
			const DsVec3f dst = { x / src, y / src, z / src };
			return dst;
		}

		DsVec3f operator -() const
		{
			const DsVec3f dst = { -x, -y, -z };
			return dst;
		}

		DsVec3f& operator +=(const DsVec3f& src)
		{
			x += src.x;
			y += src.y;
			z += src.z;
			return (*this);
		}

		DsVec3f& operator -=(const DsVec3f& src)
		{
			x -= src.x;
			y -= src.y;
			z -= src.z;
			return (*this);
		}

		DsVec3f& operator *=(const float& src)
		{
			x *= src;
			y *= src;
			z *= src;
			return (*this);
		}

		DsVec3f& operator /=(const float& src)
		{
			x /= src;
			y /= src;
			z /= src;
			return (*this);
		}

		DsVec3f& operator =(const DsVec4f& src);
		

		float Length() const
		{
			return sqrt(x*x + y*y + z*z);
		}

		//sqrtしないLength
		float LengthSq() const
		{
			return x*x + y*y + z*z;
		}

		//dirベクトル成分をなくす
		DsVec3f Flat(const DsVec3f& dir) const
		{
			const DsVec3f normal = Normalize(dir);
			const float len = Dot(*this, normal);
			const DsVec3f flat = (*this) - (normal*len);
			return flat;
		}

		bool IsNearZero(const float zero = (0.0001)) const
		{
			return	(fabs(x) < zero) && (fabs(y) < zero) && (fabs(z) < zero);
		}

		//このベクトルに垂直な平面上のベクトルを取得
		void GetVerticalPlane(DsVec3f& cX, DsVec3f& cY) const
		{
			const DsVec3f cZ = DsVec3f::Abs(DsVec3f::Normalize(*this));
			if ((cZ - DsVec3f::GetY()).IsNearZero((0.1f))){
				cX = DsVec3f::Normalize(DsVec3f::Cross(-DsVec3f::GetZ(), cZ));
			}
			else{
				cX = DsVec3f::Normalize(DsVec3f::Cross(DsVec3f::GetY(), cZ));
			}
			cY = DsVec3f::Normalize(DsVec3f::Cross(cZ, cX));
		}

		static DsVec3f Abs(const DsVec3f& a)
		{
			return DsVec3f(fabs(a.x), fabs(a.y), fabs(a.z));
		}

		static DsVec3f Clamp3(const DsVec3f& v, float min, float max)
		{
			return DsVec3f(min(max(min, v.x), max), min(max(min, v.y), max), min(max(min, v.z), max));
		}

		static DsVec3f Normalize(const DsVec3f& src)
		{
			const float len = src.Length();
			if ((0.000001) >= len)
			{
				const DsVec3f dst = { 0, 0, 0 };
				return dst;
			}
			else
			{
				const DsVec3f dst = { src.x / len, src.y / len, src.z / len };
				return dst;
			}
		}

		static DsVec3f ToVec3(float x, float y, float z)
		{
			const DsVec3f dst = { x, y, z };
			return dst;
		}

		static DsVec3f ToVec3(const float* v)
		{
			const DsVec3f dst = { v[0], v[1], v[2] };
			return dst;
		}

		static DsVec3f ToVec3_forMat16(const float* v)
		{
			const DsVec3f dst = { v[3], v[7], v[11] };
			return dst;
		}

		static DsVec3f Up()
		{
			return DsVec3f::GetY();
		}

		static DsVec3f Div(const DsVec3f& src1, const DsVec3f& src2)
		{
			const DsVec3f dst = { src1.x / src2.x, src1.y / src2.y, src1.z / src2.z };
			return dst;
		}

		static float Dot(const DsVec3f& src1, const DsVec3f& src2)
		{
			return (src1.x*src2.x + src1.y*src2.y + src1.z*src2.z);
		}

		static DsVec3f Cross(const DsVec3f& src1, const DsVec3f& src2)
		{
			const DsVec3f dst =
			{
				src1.y*src2.z - src1.z*src2.y,
				src1.z*src2.x - src1.x*src2.z,
				src1.x*src2.y - src1.y*src2.x,
			};
			return dst;
		}

		//反時計回りにp1,p2,p3
		static DsVec3f CalcNormal(const DsVec3f& p1, const DsVec3f& p2, const DsVec3f& p3)
		{
			return Normalize(Cross(p2 - p1, p3 - p1));
		}

		static const DsVec3f& GetX()
		{
			static const DsVec3f dst = { 1, 0, 0 };
			return dst;
		}

		static const DsVec3f& GetY()
		{
			static const DsVec3f dst = { 0, 1, 0 };
			return dst;
		}

		static const DsVec3f& GetZ()
		{
			static const DsVec3f dst = { 0, 0, 1 };
			return dst;
		}

		static const DsVec3f& Zero()
		{
			static const DsVec3f dst = { 0, 0, 0 };
			return dst;
		}

		static const DsVec3f& Infinity()
		{
			static const DsVec3f dst = { (FLT_MAX), (FLT_MAX), (FLT_MAX) };
			return dst;
		}

		/*
		@param s1 線分１の始点座標
		@param v1 線分１のベクトル
		@param s2 線分２の始点座標
		@param v2 線分２のベクトル
		@par aとbが交差する座標を求める。求まらなかったらfalseを返す
		*/
		static bool GetIntersection(const DsVec3f& s1, const DsVec3f& v1,
			const DsVec3f& s2, const DsVec3f& v2,
			DsVec3f& dst)
		{
			//http://marupeke296.com/COL_2D_No10_SegmentAndSegment.html
			const DsVec3f v = s2 - s1;
			const float cross = Cross(v1, v2).Length();
			if ((0.0) == cross)
			{
				return false;
			}

			const float t2 = Cross(v, v1).Length() / cross;
			const float t1 = Cross(v, v2).Length() / cross;

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