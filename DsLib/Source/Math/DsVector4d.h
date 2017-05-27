#ifndef _DS_VECTOR4D_H_
#define _DS_VECTOR4D_H_


namespace DsLib
{
	struct DsVec4f;
	struct DsVec3d;
}

namespace DsLib
{
	struct DsVec4d
	{
		union
		{
			double v[4];
			struct
			{
				double x;
				double y;
				double z;
				double w;
			};
		};

		DsVec4d(double setX, double setY, double setZ, double setW)
			: x(setX)
			, y(setY)
			, z(setZ)
			, w(setW)
		{}

		inline DsVec4d(const DsVec3d& v3, const double _w);

		DsVec4d(){}

		inline DsVec4d(const DsVec4f& src);

		inline DsVec4d& operator =(const DsVec4f& src);

		void Set(const double setX, const double setY, const double setZ, const double setW)
		{
			x = setX;
			y = setY;
			z = setZ;
			w = setW;
		}

		void Set(const DsVec3d& _v, const double _w)
		{
			x = _v.x;
			y = _v.x;
			z = _v.x;
			w = _w;
		}

		DsVec4d operator +(const DsVec4d& src) const
		{
			const DsVec4d dst = { x + src.x, y + src.y, z + src.z, w + src.w };
			return dst;
		}

		DsVec4d operator -(const DsVec4d& src) const
		{
			const DsVec4d dst = { x - src.x, y - src.y, z - src.z, w - src.w };
			return dst;
		}

		DsVec4d operator *(const double& src) const
		{
			const DsVec4d dst = { x*src, y*src, z*src, w*src };
			return dst;
		}

		DsVec4d operator /(const double& src) const
		{
			const DsVec4d dst = { x / src, y / src, z / src, w / src };
			return dst;
		}

		DsVec4d operator -() const
		{
			const DsVec4d dst = { -x, -y, -z, -w };
			return dst;
		}

		DsVec4d& operator +=(const DsVec4d& src)
		{
			x += src.x;
			y += src.y;
			z += src.z;
			w += src.w;
			return (*this);
		}

		DsVec4d& operator -=(const DsVec4d& src)
		{
			x -= src.x;
			y -= src.y;
			z -= src.z;
			w -= src.w;
			return (*this);
		}

		DsVec4d& operator *=(const double& src)
		{
			x *= src;
			y *= src;
			z *= src;
			w *= src;
			return (*this);
		}

		DsVec4d& operator /=(const double& src)
		{
			x /= src;
			y /= src;
			z /= src;
			w /= src;
			return (*this);
		}

		DsVec4d& operator =(const DsVec3d& src);

		inline operator DsVec3d() const;

		double& operator[](const int index)
		{
			assert((index < 4) && "indexの値がサイズを超えています\n");
			return v[index];
		}

		const double& operator[](const int index) const
		{
			assert((index < 4) && "indexの値がサイズを超えています\n");
			return v[index];
		}


		double Length() const
		{
			return (sqrt(x*x + y*y + z*z + w*w));
		}

		double Length3() const
		{
			return (sqrt(x*x + y*y + z*z));
		}


		bool IsNearZero() const
		{
			const double zero = (0.0000001);
			return	(fabs(x) < zero) && (fabs(y) < zero) && (fabs(z) < zero) && (fabs(w) < zero);
		}

		//dirベクトル成分をなくす
		DsVec4d Flat(const DsVec4d& dir) const
		{
			const DsVec4d normal = Normal(dir);
			const double len = Dot(*this, normal);
			const DsVec4d flat = (*this) - (normal*len);
			return flat;
		}

		static DsVec4d Normal(const DsVec4d& src)
		{
			const double len = src.Length();
			if ((0.000001) >= len)
			{
				const DsVec4d dst = { 0, 0, 0, 0 };
				return dst;
			}
			else
			{
				const DsVec4d dst = { src.x / len, src.y / len, src.z / len, src.w / len };
				return dst;
			}
		}

		static DsVec4d Normal3(const DsVec4d& src)
		{
			const double len = src.Length3();
			if ((0.000001) >= len)
			{
				const DsVec4d dst = { 0, 0, 0, 0 };
				return dst;
			}
			else
			{
				const DsVec4d dst = { src.x / len, src.y / len, src.z / len, src.w };
				return dst;
			}
		}

		static DsVec4d ToVec4(double x, double y, double z, double w)
		{
			const DsVec4d dst = { x, y, z, w };
			return dst;
		}

		static DsVec4d ToVec4(const double* v)
		{
			const DsVec4d dst = { v[0], v[1], v[2], v[3] };
			return dst;
		}

		static DsVec4d ToVec4_forMat16(const double* v)
		{
			const DsVec4d dst = { v[3], v[7], v[11], v[15] };
			return dst;
		}

		static DsVec4d Up()
		{
			return DsVec4d::GetY();
		}

		static double Dot(const DsVec4d& src1, const DsVec4d& src2)
		{
			return (src1.x*src2.x + src1.y*src2.y + src1.z*src2.z + src1.w*src2.w);
		}

		static double Dot3(const DsVec4d& src1, const DsVec4d& src2)
		{
			return (src1.x*src2.x + src1.y*src2.y + src1.z*src2.z);
		}

		static DsVec4d Cross3(const DsVec4d& src1, const DsVec4d& src2)
		{
			const DsVec4d dst =
			{
				src1.y*src2.z - src1.z*src2.y,
				src1.z*src2.x - src1.x*src2.z,
				src1.x*src2.y - src1.y*src2.x,
				0,
			};
			return dst;
		}

		static double PointDistance(const DsVec4d& plane, const DsVec3d& point) 
		{
			return plane.x*point.x + plane.y*point.y + plane.z*point.z + plane.w;
		}

		//反時計回りにp1,p2,p3
		static DsVec4d CalcNormal(const DsVec4d& p1, const DsVec4d& p2, const DsVec4d& p3)
		{
			return Normal3(Cross3(p2 - p1, p3 - p1));
		}

		static const DsVec4d& GetX()
		{
			static const DsVec4d dst = {1, 0, 0, 0};
			return dst;
		}

		static const DsVec4d& GetY()
		{
			static const DsVec4d dst = {0, 1, 0, 0};
			return dst;
		}

		static const DsVec4d& GetZ()
		{
			static const DsVec4d dst = {0, 0, 1, 0};
			return dst;
		}

		static const DsVec4d& Zero()
		{
			static const DsVec4d dst = {0, 0, 0, 0};
			return dst;
		}

		static const DsVec4d& Infinity()
		{
			static const DsVec4d dst = { (DBL_MAX), (DBL_MAX), (DBL_MAX), (DBL_MAX) };
			return dst;
		}
	};
}

#endif