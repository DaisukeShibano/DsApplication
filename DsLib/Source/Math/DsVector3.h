#ifndef _DS_VECTOR3_H_
#define _DS_VECTOR3_H_

#ifndef _DS_VECTOR3D_H_
#include "Math/DsVector3d.h"
#endif

#ifndef _DS_VECTOR3F_H_
#include "Math/DsVector3f.h"
#endif

namespace DsLib
{
	template<typename TYPE >
	struct DsVec4;
}

namespace DsLib
{
	template<typename TYPE >
	struct DsVec3
	{
		union
		{
			TYPE v[3];
			struct
			{
				TYPE x;
				TYPE y;
				TYPE z;
			};
		};

		DsVec3<TYPE>(TYPE setX, TYPE setY, TYPE setZ)
			: x(setX)
			, y(setY)
			, z(setZ)
		{}
		DsVec3<TYPE>(){}

		template <class U>
		DsVec3<TYPE>(const DsVec3<U>& src)
			: x(static_cast<TYPE>(src.x))
			, y(static_cast<TYPE>(src.y))
			, z(static_cast<TYPE>(src.z))
		{}

		template <class U>
		DsVec3<TYPE>& operator =(const DsVec3<U>& src)
		{
			x = static_cast<TYPE>(src.x);
			y = static_cast<TYPE>(src.y);
			z = static_cast<TYPE>(src.z);
			return (*this);
		}

		void Set(const TYPE setX, const TYPE setY, const TYPE setZ)
		{
			x = setX;
			y = setY;
			z = setZ;
		}

		TYPE& operator[](int i)
		{
			return  v[i];
		}
		TYPE operator[](int i) const
		{
			return  v[i];
		}

		DsVec3<TYPE> operator +(const DsVec3<TYPE>& src) const
		{
			const DsVec3<TYPE> dst = { x + src.x, y + src.y, z + src.z };
			return dst;
		}

		DsVec3<TYPE> operator -(const DsVec3<TYPE>& src) const
		{
			const DsVec3<TYPE> dst = { x - src.x, y - src.y, z - src.z };
			return dst;
		}

		DsVec3<TYPE> operator *(const TYPE& src) const
		{
			const DsVec3<TYPE> dst = { x*src, y*src, z*src };
			return dst;
		}

		DsVec3<TYPE> operator /(const TYPE& src) const
		{
			const DsVec3<TYPE> dst = { x / src, y / src, z / src };
			return dst;
		}

		DsVec3<TYPE> operator -() const
		{
			const DsVec3<TYPE> dst = { -x, -y, -z };
			return dst;
		}

		DsVec3<TYPE>& operator +=(const DsVec3<TYPE>& src)
		{
			x += src.x;
			y += src.y;
			z += src.z;
			return (*this);
		}

		DsVec3<TYPE>& operator -=(const DsVec3<TYPE>& src)
		{
			x -= src.x;
			y -= src.y;
			z -= src.z;
			return (*this);
		}

		DsVec3<TYPE>& operator *=(const TYPE& src)
		{
			x *= src;
			y *= src;
			z *= src;
			return (*this);
		}

		DsVec3<TYPE>& operator /=(const TYPE& src)
		{
			x /= src;
			y /= src;
			z /= src;
			return (*this);
		}

		DsVec3<TYPE>& operator =(const DsVec4<TYPE>& src)
		{
			x = src.x;
			y = src.y;
			z = src.z;
			return (*this);
		}

		operator DsVec4<TYPE>() const { return DsVec4<TYPE>::ToVec4(x, y, z, 0); }


		TYPE Length() const
		{
			return static_cast<TYPE>(sqrt(x*x + y*y + z*z));
		}

		//sqrtしないLength
		TYPE LengthSq() const
		{
			return static_cast<TYPE>(x*x + y*y + z*z);
		}

		//dirベクトル成分をなくす
		DsVec3<TYPE> Flat(const DsVec3<TYPE>& dir) const
		{
			const DsVec3<TYPE> normal = Normalize(dir);
			const TYPE len = Dot(*this, normal);
			const DsVec3<TYPE> flat = (*this) - (normal*len);
			return flat;
		}

		bool IsNearZero(const TYPE zero = static_cast<TYPE>(0.0001) ) const
		{
			return	(fabs(x) < zero) && (fabs(y) < zero) && (fabs(z) < zero);
		}

		//このベクトルに垂直な平面上のベクトルを取得
		void GetVerticalPlane(DsVec3<TYPE>& cX, DsVec3<TYPE>& cY) const
		{
			const DsVec3<TYPE> cZ = DsVec3<TYPE>::Abs( DsVec3<TYPE>::Normalize(*this) );
			if ((cZ - DsVec3<TYPE>::GetY()).IsNearZero(static_cast<TYPE>(0.1))){
				cX = DsVec3<TYPE>::Normalize(DsVec3<TYPE>::Cross(-DsVec3<TYPE>::GetZ(), cZ));
			}
			else{
				cX = DsVec3<TYPE>::Normalize(DsVec3<TYPE>::Cross(DsVec3<TYPE>::GetY(), cZ));
			}
			cY = DsVec3<TYPE>::Normalize(DsVec3<TYPE>::Cross(cZ, cX));
		}

		static DsVec3<TYPE> Abs(const DsVec3<TYPE>& a)
		{
			return DsVec3<TYPE>( fabs(a.x), fabs(a.y), fabs(a.z) );
		}

		static DsVec3<TYPE> Clamp3(const DsVec3<TYPE>& v, double _min, double _max)
		{
			return DsVec3<TYPE>(min(max(_min, v.x), _max), min(max(_min, v.y), _max), min(max(_min, v.z), _max));
		}

		static DsVec3<TYPE> Clamp3(const DsVec3<TYPE>& v, const DsVec3<TYPE>& _min, const DsVec3<TYPE>& _max)
		{
			return DsVec3<TYPE>(
				min(max(_min.x, v.x), _max.x),
				min(max(_min.y, v.y), _max.y),
				min(max(_min.z, v.z), _max.z)
				);
		}

		static DsVec3<TYPE> Normalize(const DsVec3<TYPE>& src)
		{
			const TYPE len = src.Length();
			if (static_cast<TYPE>(0.000001) >= len)
			{
				const DsVec3<TYPE> dst = { 0, 0, 0 };
				return dst;
			}
			else
			{
				const DsVec3<TYPE> dst = { src.x / len, src.y / len, src.z / len };
				return dst;
			}
		}

		static DsVec3<TYPE> ToVec3(TYPE x, TYPE y, TYPE z)
		{
			const DsVec3<TYPE> dst = { x, y, z };
			return dst;
		}

		static DsVec3<TYPE> ToVec3(const TYPE* v)
		{
			const DsVec3<TYPE> dst = { v[0], v[1], v[2] };
			return dst;
		}

		static DsVec3<TYPE> ToVec3_forMat16(const TYPE* v)
		{
			const DsVec3<TYPE> dst = { v[3], v[7], v[11] };
			return dst;
		}

		static DsVec3<TYPE> Up()
		{
			return DsVec3<TYPE>::GetY();
		}

		static DsVec3<TYPE> Div(const DsVec3<TYPE>& src1, const DsVec3<TYPE>& src2)
		{
			const DsVec3<TYPE> dst = { src1.x / src2.x, src1.y / src2.y, src1.z / src2.z };
			return dst;
		}

		static TYPE Dot(const DsVec3<TYPE>& src1, const DsVec3<TYPE>& src2)
		{
			return (src1.x*src2.x + src1.y*src2.y + src1.z*src2.z);
		}

		static DsVec3<TYPE> Cross(const DsVec3<TYPE>& src1, const DsVec3<TYPE>& src2)
		{
			const DsVec3<TYPE> dst =
			{
				src1.y*src2.z - src1.z*src2.y,
				src1.z*src2.x - src1.x*src2.z,
				src1.x*src2.y - src1.y*src2.x,
			};
			return dst;
		}

		//反時計回りにp1,p2,p3
		static DsVec3<TYPE> CalcNormal(const DsVec3<TYPE>& p1, const DsVec3<TYPE>& p2, const DsVec3<TYPE>& p3)
		{
			return Normalize(Cross(p2 - p1, p3 - p1));
		}

		static const DsVec3<TYPE>& GetX()
		{
			static const DsVec3<TYPE> dst = {1, 0, 0};
			return dst;
		}

		static const DsVec3<TYPE>& GetY()
		{
			static const DsVec3<TYPE> dst = {0, 1, 0};
			return dst;
		}

		static const DsVec3<TYPE>& GetZ()
		{
			static const DsVec3<TYPE> dst = {0, 0, 1};
			return dst;
		}

		static const DsVec3<TYPE>& Zero()
		{
			static const DsVec3<TYPE> dst = {0, 0, 0};
			return dst;
		}

		static const DsVec3<TYPE>& Infinity()
		{
			static const DsVec3<TYPE> dst = { static_cast<TYPE>(99999999), static_cast<TYPE>(99999999), static_cast<TYPE>(99999999) };
			return dst;
		}

		/*
		@param s1 線分１の始点座標
		@param v1 線分１のベクトル
		@param s2 線分２の始点座標
		@param v2 線分２のベクトル
		@par aとbが交差する座標を求める。求まらなかったらfalseを返す
		*/
		static bool GetIntersection(const DsVec3<TYPE>& s1, const DsVec3<TYPE>& v1,
									const DsVec3<TYPE>& s2, const DsVec3<TYPE>& v2,
									DsVec3<TYPE>& dst)
		{
			//http://marupeke296.com/COL_2D_No10_SegmentAndSegment.html
			const DsVec3<TYPE> v = s2 - s1;
			const TYPE cross = Cross(v1, v2).Length();
			if (static_cast<TYPE>(0.0) == cross)
			{
				return false;
			}

			const TYPE t2 = Cross(v, v1).Length() / cross;
			const TYPE t1 = Cross(v, v2).Length() / cross;

			if( ( (static_cast<TYPE>(0.0) <= t1) && (t1 <= static_cast<TYPE>(1.0)) ) &&
				( (static_cast<TYPE>(0.0) <= t2) && (t2 <= static_cast<TYPE>(1.0)) )
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

	inline DsVec3f ToVec3f( const DsVec3d& src )
	{
		const DsVec3f dst =
		{
			static_cast<float>(src.x),
			static_cast<float>(src.y),
			static_cast<float>(src.z),
		};
		return dst;
	}

	inline DsVec3d ToVec3d( const DsVec3f& src )
	{
		const DsVec3d dst =
		{
			static_cast<double>(src.x),
			static_cast<double>(src.y),
			static_cast<double>(src.z),
		};
		return dst;
	}


	//nベクトル
	class DsVec
	{
	public:
		explicit DsVec( int n )
		:m_size(n)
		,m_pVec(0)
		{
			if( 0 < n )
			{
				m_pVec = new double[n];
				assert(m_pVec!=0 && "メモリ確保失敗\n");
			}
			else
			{
				assert(0 && "ベクトルのサイズが０以下\n");
			}
		}

		~DsVec()
		{
			delete[] m_pVec;
			m_pVec = 0;
		}

		int Dm() const { return m_size; }


		void Set(const DsVec3d& a, const DsVec3d& b, const DsVec3d& c, const DsVec3d& d )
		{
			assert( (12 <= m_size ) && "ベクトルのサイズを超えたアクセス\n");
			m_pVec[0]=a.x; m_pVec[1]=a.y; m_pVec[2]=a.z;
			m_pVec[3]=b.x; m_pVec[4]=b.y; m_pVec[5]=b.z;
			m_pVec[6]=c.x; m_pVec[7]=c.y; m_pVec[8]=c.z;
			m_pVec[9]=d.x; m_pVec[10]=d.y; m_pVec[11]=d.z;
		}

		double& operator [](int n)
		{
			assert( (0 < n) && (m_size < n) && "ベクトルのサイズを超えたアクセス\n");
			return m_pVec[n];
		}

		double& operator [](int n) const
		{
			assert( (0 < n) && (m_size < n) && "ベクトルのサイズを超えたアクセス\n");
			return m_pVec[n];
		}

		double operator *(const DsVec& src) const
		{
			assert( (src.Dm() == m_size) && "サイズ不一致\n");
			double ret=0.0;
			for(int i=0; m_size > i; ++i)
			{
				ret += (m_pVec[i]*src[i]); 
			}
			return ret;
		}

	private:
		const int m_size;
		double* m_pVec;

	};



	inline DsVec3d::DsVec3d(const DsVec3f& src)
		: x(static_cast<double>(src.x))
		, y(static_cast<double>(src.y))
		, z(static_cast<double>(src.z))
	{}

	inline DsVec3d& DsVec3d::operator = (const DsVec3f& src)
	{
		x = static_cast<double>(src.x);
		y = static_cast<double>(src.y);
		z = static_cast<double>(src.z);
		return (*this);
	}

	inline DsVec3f::DsVec3f(const DsVec3d& src)
		: x(static_cast<float>(src.x))
		, y(static_cast<float>(src.y))
		, z(static_cast<float>(src.z))
	{}
	inline DsVec3f& DsVec3f::operator = (const DsVec3d& src)
	{
		x = static_cast<float>(src.x);
		y = static_cast<float>(src.y);
		z = static_cast<float>(src.z);
		return (*this);
	}

}

#endif