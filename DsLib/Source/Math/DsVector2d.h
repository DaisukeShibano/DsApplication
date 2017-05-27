#ifndef _DS_VECTOR2D_H_
#define _DS_VECTOR2D_H_

namespace DsLib
{
	template<typename TYPE >
	struct DsVec2
	{
		union
		{
			TYPE v[2];
			struct
			{
				TYPE x;
				TYPE y;
			};
		};

		DsVec2<TYPE>(TYPE setX, TYPE setY)
			: x(setX)
			, y(setY)
		{}
		DsVec2<TYPE>(){}

		void Set(const TYPE setX, const TYPE setY )
		{
			x = setX;
			y = setY;
		}

		DsVec2<TYPE> operator +(const DsVec2<TYPE>& src) const
		{
			const DsVec2<TYPE> dst = { x + src.x, y + src.y };
			return dst;
		}

		DsVec2<TYPE> operator -(const DsVec2<TYPE>& src) const
		{
			const DsVec2<TYPE> dst = { x - src.x, y - src.y };
			return dst;
		}

		DsVec2<TYPE> operator *(const TYPE& src) const
		{
			const DsVec2<TYPE> dst = { x*src, y*src };
			return dst;
		}

		DsVec2<TYPE> operator /(const TYPE& src) const
		{
			const DsVec2<TYPE> dst = { x / src, y / src };
			return dst;
		}

		DsVec2<TYPE> operator -() const
		{
			const DsVec2<TYPE> dst = { -x, -y };
			return dst;
		}

		DsVec2<TYPE>& operator +=(const DsVec2<TYPE>& src)
		{
			x += src.x;
			y += src.y;
			return (*this);
		}

		DsVec2<TYPE>& operator -=(const DsVec2<TYPE>& src)
		{
			x -= src.x;
			y -= src.y;
			return (*this);
		}

		DsVec2<TYPE>& operator *=(const TYPE& src)
		{
			x *= src;
			y *= src;
			return (*this);
		}

		DsVec2<TYPE>& operator /=(const TYPE& src)
		{
			x /= src;
			y /= src;
			return (*this);
		}


		TYPE Length() const
		{
			return static_cast<TYPE>(sqrt(x*x + y*y));
		}

		//dirÉxÉNÉgÉãê¨ï™ÇÇ»Ç≠Ç∑
		DsVec2<TYPE>  Flat(const DsVec2<TYPE>& dir) const
		{
			const DsVec2<TYPE> normal = Normal(dir);
			const TYPE len = Dot(*this, normal);
			const DsVec2<TYPE> flat = (*this) - (normal*len);
			return flat;
		}

		static DsVec2<TYPE> Normal(const DsVec2<TYPE>& src)
		{
			const TYPE len = src.Length();
			if ( static_cast<TYPE>(0.000001) >= len)
			{
				const DsVec2<TYPE> dst = { 0, 0 };
				return dst;
			}
			else
			{
				const DsVec2<TYPE> dst = { src.x / len, src.y / len };
				return dst;
			}
		}

		static DsVec2<TYPE> Get(TYPE x, TYPE y)
		{
			const DsVec2<TYPE> dst = { x, y };
			return dst;
		}

		static DsVec2<TYPE> Up()
		{
			return DsVec2<TYPE>::GetY();
		}

		static TYPE Dot(const DsVec2<TYPE>& src1, const DsVec2<TYPE>& src2)
		{
			return (src1.x*src2.x + src1.y*src2.y + src1.z*src2.z);
		}

		static DsVec2<TYPE> Cross(const DsVec2<TYPE>& src1, const DsVec2<TYPE>& src2)
		{
			const DsVec2<TYPE> dst =
			{
				src1.y*src2.z - src1.z*src2.y,
				src1.z*src2.x - src1.x*src2.z,
				src1.x*src2.y - src1.y*src2.x,
			};
			return dst;
		}

		static const DsVec2<TYPE>& GetX()
		{
			static const DsVec2<TYPE> dst = { 1, 0 };
			return dst;
		}

		static const DsVec2<TYPE>& GetY()
		{
			static const DsVec2<TYPE> dst = { 0, 1};
			return dst;
		}

		static const DsVec2<TYPE>& Zero()
		{
			static const DsVec2<TYPE> dst = { 0, 0 };
			return dst;
		}
	};

	typedef DsVec2<double>	DsVec2d;
	typedef DsVec2<float>	DsVec2f;
	typedef DsVec2<int>		DsVec2i;

	template<typename TYPE, class SRC_CALSS>
	DsVec2<TYPE> CastDsVec2f(const SRC_CALSS& src)
	{
		const DsVec2<TYPE> dst =
		{
			static_cast<TYPE>(src.x),
			static_cast<TYPE>(src.y),
		};
		return dst;
	}
}

#endif