#ifndef _DS_VECTOR4F_H_
#define _DS_VECTOR4F_H_


namespace DsLib
{
	struct DsVec3f;
	struct DsVec3d;
	struct DsVec4d;
}

namespace DsLib
{
	struct DsVec4f
	{
		union
		{
			float v[4];
			struct
			{
				float x;
				float y;
				float z;
				float w;
			};
		};

		DsVec4f(float setX, float setY, float setZ, float setW)
			: x(setX)
			, y(setY)
			, z(setZ)
			, w(setW)
		{}

		inline DsVec4f(const DsVec3d& v3, const float _w);

		inline DsVec4f(const DsVec3f& v3, const float _w);

		DsVec4f(){}

		inline DsVec4f(const DsVec4d& src);

		inline DsVec4f& operator =(const DsVec4d& src);

		void Set(const float setX, const float setY, const float setZ, const float setW)
		{
			x = setX;
			y = setY;
			z = setZ;
			w = setW;
		}
		void Set(const DsVec3f& _v, const float _w)
		{
			x = _v.x;
			y = _v.x;
			z = _v.x;
			w = _w;
		}

		DsVec4f operator +(const DsVec4f& src) const
		{
			const DsVec4f dst = { x + src.x, y + src.y, z + src.z, w + src.w };
			return dst;
		}

		DsVec4f operator -(const DsVec4f& src) const
		{
			const DsVec4f dst = { x - src.x, y - src.y, z - src.z, w - src.w };
			return dst;
		}

		DsVec4f operator *(const float& src) const
		{
			const DsVec4f dst = { x*src, y*src, z*src, w*src };
			return dst;
		}

		DsVec4f operator /(const float& src) const
		{
			const DsVec4f dst = { x / src, y / src, z / src, w / src };
			return dst;
		}

		DsVec4f operator -() const
		{
			const DsVec4f dst = { -x, -y, -z, -w };
			return dst;
		}

		DsVec4f& operator +=(const DsVec4f& src)
		{
			x += src.x;
			y += src.y;
			z += src.z;
			w += src.w;
			return (*this);
		}

		DsVec4f& operator -=(const DsVec4f& src)
		{
			x -= src.x;
			y -= src.y;
			z -= src.z;
			w -= src.w;
			return (*this);
		}

		DsVec4f& operator *=(const float& src)
		{
			x *= src;
			y *= src;
			z *= src;
			w *= src;
			return (*this);
		}

		DsVec4f& operator /=(const float& src)
		{
			x /= src;
			y /= src;
			z /= src;
			w /= src;
			return (*this);
		}

		inline DsVec4f& operator =(const DsVec3f& src);

		inline operator DsVec3f() const;

		float& operator[](const int index)
		{
			assert((index < 4) && "indexの値がサイズを超えています\n");
			return v[index];
		}

		const float& operator[](const int index) const
		{
			assert((index < 4) && "indexの値がサイズを超えています\n");
			return v[index];
		}


		float Length() const
		{
			return (sqrt(x*x + y*y + z*z + w*w));
		}

		float Length3() const
		{
			return (sqrt(x*x + y*y + z*z));
		}


		bool IsNearZero() const
		{
			const float zero = (0.0000001f);
			return	(fabs(x) < zero) && (fabs(y) < zero) && (fabs(z) < zero) && (fabs(w) < zero);
		}

		//dirベクトル成分をなくす
		DsVec4f Flat(const DsVec4f& dir) const
		{
			const DsVec4f normal = Normal(dir);
			const float len = Dot(*this, normal);
			const DsVec4f flat = (*this) - (normal*len);
			return flat;
		}

		static DsVec4f Normal(const DsVec4f& src)
		{
			const float len = src.Length();
			if ((0.000001) >= len)
			{
				const DsVec4f dst = { 0, 0, 0, 0 };
				return dst;
			}
			else
			{
				const DsVec4f dst = { src.x / len, src.y / len, src.z / len, src.w / len };
				return dst;
			}
		}

		static DsVec4f Normal3(const DsVec4f& src)
		{
			const float len = src.Length3();
			if ((0.000001) >= len)
			{
				const DsVec4f dst = { 0, 0, 0, 0 };
				return dst;
			}
			else
			{
				const DsVec4f dst = { src.x / len, src.y / len, src.z / len, src.w };
				return dst;
			}
		}

		static DsVec4f Clamp4(const DsVec4f& v, double _min, double _max)
		{
			return DsVec4f(min(max(_min, v.x), _max), min(max(_min, v.y), _max), min(max(_min, v.z), _max), min(max(_min, v.w), _max));
		}

		static DsVec4f Clamp4(const DsVec4f& v, const DsVec4f& _min, const DsVec4f& _max)
		{
			return DsVec4f(
				min(max(_min.x, v.x), _max.x),
				min(max(_min.y, v.y), _max.y),
				min(max(_min.z, v.z), _max.z),
				min(max(_min.w, v.w), _max.w)
				);
		}

		static DsVec4f ToVec4(float x, float y, float z, float w)
		{
			const DsVec4f dst = { x, y, z, w };
			return dst;
		}

		static DsVec4f ToVec4(const float* v)
		{
			const DsVec4f dst = { v[0], v[1], v[2], v[3] };
			return dst;
		}

		static DsVec4f ToVec4_forMat16(const float* v)
		{
			const DsVec4f dst = { v[3], v[7], v[11], v[15] };
			return dst;
		}

		static DsVec4f Up()
		{
			return DsVec4f::GetY();
		}

		static float Dot(const DsVec4f& src1, const DsVec4f& src2)
		{
			return (src1.x*src2.x + src1.y*src2.y + src1.z*src2.z + src1.w*src2.w);
		}

		static float Dot3(const DsVec4f& src1, const DsVec4f& src2)
		{
			return (src1.x*src2.x + src1.y*src2.y + src1.z*src2.z);
		}

		static DsVec4f Cross3(const DsVec4f& src1, const DsVec4f& src2)
		{
			const DsVec4f dst =
			{
				src1.y*src2.z - src1.z*src2.y,
				src1.z*src2.x - src1.x*src2.z,
				src1.x*src2.y - src1.y*src2.x,
				0,
			};
			return dst;
		}

		static float PointDistance(const DsVec4f& plane, const DsVec3f& point)
		{
			return plane.x*point.x + plane.y*point.y + plane.z*point.z + plane.w;
		}

		//反時計回りにp1,p2,p3
		static DsVec4f CalcNormal(const DsVec4f& p1, const DsVec4f& p2, const DsVec4f& p3)
		{
			return Normal3(Cross3(p2 - p1, p3 - p1));
		}

		static const DsVec4f& GetX()
		{
			static const DsVec4f dst = {1, 0, 0, 0};
			return dst;
		}

		static const DsVec4f& GetY()
		{
			static const DsVec4f dst = {0, 1, 0, 0};
			return dst;
		}

		static const DsVec4f& GetZ()
		{
			static const DsVec4f dst = {0, 0, 1, 0};
			return dst;
		}

		static const DsVec4f& Zero()
		{
			static const DsVec4f dst = {0, 0, 0, 0};
			return dst;
		}

		static const DsVec4f& Infinity()
		{
			static const DsVec4f dst = { (FLT_MAX), (FLT_MAX), (FLT_MAX), (FLT_MAX) };
			return dst;
		}
	};
}

#endif