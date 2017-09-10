#ifndef _DS_VECTOR4_H_
#define _DS_VECTOR4_H_

#ifndef _DS_VECTOR4D_H_
#include "Math/DsVector4d.h"
#endif

#ifndef _DS_VECTOR4F_H_
#include "Math/DsVector4f.h"
#endif


namespace DsLib
{
	template<typename TYPE >
	struct DsVec3;
}

namespace DsLib
{
	template<typename TYPE >
	struct DsVec4
	{
		union
		{
			TYPE v[4];
			struct
			{
				TYPE x;
				TYPE y;
				TYPE z;
				TYPE w;
			};
		};

		DsVec4<TYPE>(TYPE setX, TYPE setY, TYPE setZ, TYPE setW)
			: x(setX)
			, y(setY)
			, z(setZ)
			, w(setW)
		{}
		DsVec4<TYPE>(const DsVec3<TYPE>& v3, const TYPE _w)
			: x(v3.x)
			, y(v3.y)
			, z(v3.z)
			, w(_w)
		{}

		DsVec4<TYPE>(){}

		template <class U>
		DsVec4<TYPE>(const DsVec4<U>& src)
			: x(static_cast<TYPE>(src.x))
			, y(static_cast<TYPE>(src.y))
			, z(static_cast<TYPE>(src.z))
			, w(static_cast<TYPE>(src.w))
		{}

		template <class U>
		DsVec4<TYPE>& operator =(const DsVec4<U>& src)
		{
			x = static_cast<TYPE>(src.x);
			y = static_cast<TYPE>(src.y);
			z = static_cast<TYPE>(src.z);
			w = static_cast<TYPE>(src.w);
			return (*this);
		}

		void Set(const TYPE setX, const TYPE setY, const TYPE setZ, const TYPE setW)
		{
			x = setX;
			y = setY;
			z = setZ;
			w = setW;
		}

		DsVec4<TYPE> operator +(const DsVec4<TYPE>& src) const
		{
			const DsVec4<TYPE> dst = { x + src.x, y + src.y, z + src.z, w + src.w };
			return dst;
		}

		DsVec4<TYPE> operator -(const DsVec4<TYPE>& src) const
		{
			const DsVec4<TYPE> dst = { x - src.x, y - src.y, z - src.z, w - src.w };
			return dst;
		}

		DsVec4<TYPE> operator *(const TYPE& src) const
		{
			const DsVec4<TYPE> dst = { x*src, y*src, z*src, w*src };
			return dst;
		}

		DsVec4<TYPE> operator /(const TYPE& src) const
		{
			const DsVec4<TYPE> dst = { x / src, y / src, z / src, w / src };
			return dst;
		}

		DsVec4<TYPE> operator -() const
		{
			const DsVec4<TYPE> dst = { -x, -y, -z, -w };
			return dst;
		}

		DsVec4<TYPE>& operator +=(const DsVec4<TYPE>& src)
		{
			x += src.x;
			y += src.y;
			z += src.z;
			w += src.w;
			return (*this);
		}

		DsVec4<TYPE>& operator -=(const DsVec4<TYPE>& src)
		{
			x -= src.x;
			y -= src.y;
			z -= src.z;
			w -= src.w;
			return (*this);
		}

		DsVec4<TYPE>& operator *=(const TYPE& src)
		{
			x *= src;
			y *= src;
			z *= src;
			w *= src;
			return (*this);
		}

		DsVec4<TYPE>& operator /=(const TYPE& src)
		{
			x /= src;
			y /= src;
			z /= src;
			w /= src;
			return (*this);
		}


		DsVec4<TYPE>& operator =(const DsVec3<TYPE>& src)
		{
			x = src.x;
			y = src.y;
			z = src.z;
			return (*this);
		}

		TYPE& operator[](const int index)
		{
			assert((index < 4) && "indexの値がサイズを超えています\n");
			return v[index];
		}

		const TYPE& operator[](const int index) const
		{
			assert((index < 4) && "indexの値がサイズを超えています\n");
			return v[index];
		}

		operator DsVec3<TYPE>() const { return DsVec3<TYPE>::ToVec3(x, y, z); }

		


		TYPE Length() const
		{
			return static_cast<TYPE>(sqrt(x*x + y*y + z*z + w*w));
		}

		TYPE Length3() const
		{
			return static_cast<TYPE>(sqrt(x*x + y*y + z*z));
		}


		bool IsNearZero() const
		{
			const TYPE zero = static_cast<TYPE>(0.0000001);
			return	(fabs(x) < zero) && (fabs(y) < zero) && (fabs(z) < zero) && (fabs(w) < zero);
		}

		//dirベクトル成分をなくす
		DsVec4<TYPE> Flat(const DsVec4<TYPE>& dir) const
		{
			const DsVec4<TYPE> normal = Normal(dir);
			const TYPE len = Dot(*this, normal);
			const DsVec4<TYPE> flat = (*this) - (normal*len);
			return flat;
		}

		static DsVec4<TYPE> Normal(const DsVec4<TYPE>& src)
		{
			const TYPE len = src.Length();
			if (static_cast<TYPE>(0.000001) >= len)
			{
				const DsVec4<TYPE> dst = { 0, 0, 0, 0 };
				return dst;
			}
			else
			{
				const DsVec4<TYPE> dst = { src.x / len, src.y / len, src.z / len, src.w / len };
				return dst;
			}
		}

		static DsVec4<TYPE> Normal3(const DsVec4<TYPE>& src)
		{
			const TYPE len = src.Length3();
			if (static_cast<TYPE>(0.000001) >= len)
			{
				const DsVec4<TYPE> dst = { 0, 0, 0, 0 };
				return dst;
			}
			else
			{
				const DsVec4<TYPE> dst = { src.x / len, src.y / len, src.z / len, src.w };
				return dst;
			}
		}

		static DsVec4<TYPE> Clamp4(const DsVec4<TYPE>& v, double _min, double _max)
		{
			return DsVec4<TYPE>(min(max(_min, v.x), _max), min(max(_min, v.y), _max), min(max(_min, v.z), _max), min(max(_min, v.w), _max));
		}

		static DsVec4<TYPE> Clamp4(const DsVec4<TYPE>& v, const DsVec4<TYPE>& _min, const DsVec4<TYPE>& _max)
		{
			return DsVec4<TYPE>(
				min(max(_min.x, v.x), _max.x),
				min(max(_min.y, v.y), _max.y),
				min(max(_min.z, v.z), _max.z),
				min(max(_min.w, v.w), _max.w)
				);
		}

		static DsVec4<TYPE> ToVec4(TYPE x, TYPE y, TYPE z, TYPE w)
		{
			const DsVec4<TYPE> dst = { x, y, z, w };
			return dst;
		}

		static DsVec4<TYPE> ToVec4(const TYPE* v)
		{
			const DsVec4<TYPE> dst = { v[0], v[1], v[2], v[3] };
			return dst;
		}

		static DsVec4<TYPE> ToVec4_forMat16(const TYPE* v)
		{
			const DsVec4<TYPE> dst = { v[3], v[7], v[11], v[15] };
			return dst;
		}

		static DsVec4<TYPE> Up()
		{
			return DsVec4<TYPE>::GetY();
		}

		static TYPE Dot(const DsVec4<TYPE>& src1, const DsVec4<TYPE>& src2)
		{
			return (src1.x*src2.x + src1.y*src2.y + src1.z*src2.z + src1.w*src2.w);
		}

		static TYPE Dot3(const DsVec4<TYPE>& src1, const DsVec4<TYPE>& src2)
		{
			return (src1.x*src2.x + src1.y*src2.y + src1.z*src2.z);
		}

		static DsVec4<TYPE> Cross3(const DsVec4<TYPE>& src1, const DsVec4<TYPE>& src2)
		{
			const DsVec4<TYPE> dst =
			{
				src1.y*src2.z - src1.z*src2.y,
				src1.z*src2.x - src1.x*src2.z,
				src1.x*src2.y - src1.y*src2.x,
				0,
			};
			return dst;
		}

		//反時計回りにp1,p2,p3
		static DsVec4<TYPE> CalcNormal(const DsVec4<TYPE>& p1, const DsVec4<TYPE>& p2, const DsVec4<TYPE>& p3)
		{
			return Normal3(Cross3(p2 - p1, p3 - p1));
		}

		static const DsVec4<TYPE>& GetX()
		{
			static const DsVec4<TYPE> dst = {1, 0, 0, 0};
			return dst;
		}

		static const DsVec4<TYPE>& GetY()
		{
			static const DsVec4<TYPE> dst = {0, 1, 0, 0};
			return dst;
		}

		static const DsVec4<TYPE>& GetZ()
		{
			static const DsVec4<TYPE> dst = {0, 0, 1, 0};
			return dst;
		}

		static const DsVec4<TYPE>& Zero()
		{
			static const DsVec4<TYPE> dst = {0, 0, 0, 0};
			return dst;
		}

		static const DsVec4<TYPE>& Infinity()
		{
			static const DsVec4<TYPE> dst = { static_cast<TYPE>(99999999), static_cast<TYPE>(99999999), static_cast<TYPE>(99999999), static_cast<TYPE>(99999999) };
			return dst;
		}
	};

	inline DsVec4f ToVec4f(const DsVec4d& src)
	{
		const DsVec4f dst =
		{
			static_cast<float>(src.x),
			static_cast<float>(src.y),
			static_cast<float>(src.z),
			static_cast<float>(src.w),
		};
		return dst;
	}

	inline DsVec4d ToVec4d(const DsVec4f& src)
	{
		const DsVec4d dst =
		{
			static_cast<double>(src.x),
			static_cast<double>(src.y),
			static_cast<double>(src.z),
			static_cast<double>(src.w),
		};
		return dst;
	}



	inline DsVec4d& DsVec4d::operator = (const DsVec4f& src)
	{
		x = static_cast<double>(src.x);
		y = static_cast<double>(src.y);
		z = static_cast<double>(src.z);
		w = static_cast<double>(src.w);
		return (*this);
	}
	inline DsVec4d::DsVec4d(const DsVec4f& src)
		: x(static_cast<double>(src.x))
		, y(static_cast<double>(src.y))
		, z(static_cast<double>(src.z))
		, w(static_cast<double>(src.w))
	{}

	inline DsVec4f::DsVec4f(const DsVec4d& src)
		: x(static_cast<float>(src.x))
		, y(static_cast<float>(src.y))
		, z(static_cast<float>(src.z))
		, w(static_cast<float>(src.w))
	{}
	inline DsVec4f& DsVec4f::operator = (const DsVec4d& src)
	{
		x = static_cast<float>(src.x);
		y = static_cast<float>(src.y);
		z = static_cast<float>(src.z);
		w = static_cast<float>(src.w);
		return (*this);
	}



	inline DsVec4d::DsVec4d(const DsVec3d& v3, const double _w)
		: x(v3.x)
		, y(v3.y)
		, z(v3.z)
		, w(_w)
	{}
	inline DsVec4d& DsVec4d::operator = (const DsVec3d& src)
	{
		x = src.x;
		y = src.y;
		z = src.z;
		return (*this);
	}
	inline DsVec4d::operator DsVec3d() const { return DsVec3d(x, y, z); }

	inline DsVec4f::DsVec4f(const DsVec3d& v3, const float _w)
		: x(static_cast<float>(v3.x))
		, y(static_cast<float>(v3.y))
		, z(static_cast<float>(v3.z))
		, w(_w)
	{}
	inline DsVec4f::DsVec4f(const DsVec3f& v3, const float _w)
		: x(static_cast<float>(v3.x))
		, y(static_cast<float>(v3.y))
		, z(static_cast<float>(v3.z))
		, w(_w)
	{}
	inline DsVec4f& DsVec4f::operator = (const DsVec3f& src)
	{
		x = src.x;
		y = src.y;
		z = src.z;
		return (*this);
	}
	inline DsVec4f::operator DsVec3f() const { return DsVec3f(x, y, z); }


	inline DsVec3d& DsVec3d::operator =(const DsVec4d& src)
	{
		x = src.x;
		y = src.y;
		z = src.z;
		return (*this);
	}
	
	inline DsVec3f& DsVec3f::operator =(const DsVec4f& src)
	{
		x = static_cast<float>(src.x);
		y = static_cast<float>(src.y);
		z = static_cast<float>(src.z);
		return (*this);
	}
	

}

#endif