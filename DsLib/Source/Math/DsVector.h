#ifndef __DS_VECTOR__
#define __DS_VECTOR__

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

		void Set(const TYPE setX, const TYPE setY, const TYPE setZ )
		{
			x = setX;
			y = setY;
			z = setZ;
		}

		DsVec3<TYPE> operator +(const DsVec3<TYPE>& src ) const
		{
			const DsVec3<TYPE> dst = { x + src.x, y + src.y, z + src.z };
			return dst;
		}

		DsVec3<TYPE> operator -(const DsVec3<TYPE>& src ) const
		{
			const DsVec3<TYPE> dst = { x - src.x, y - src.y, z - src.z };
			return dst;
		}

		DsVec3<TYPE> operator *(const TYPE& src ) const
		{
			const DsVec3<TYPE> dst = { x*src, y*src, z*src };
			return dst;
		}

		DsVec3<TYPE> operator /(const TYPE& src ) const
		{
			const DsVec3<TYPE> dst = { x/src, y/src, z/src };
			return dst;
		}

		DsVec3<TYPE> operator -() const
		{
			const DsVec3<TYPE> dst = { -x, -y, -z };
			return dst;
		}

		DsVec3<TYPE>& operator +=(const DsVec3<TYPE>& src )
		{
			x += src.x;
			y += src.y;
			z += src.z;
			return (*this);
		}

		DsVec3<TYPE>& operator -=(const DsVec3<TYPE>& src )
		{
			x -= src.x;
			y -= src.y;
			z -= src.z;
			return (*this);
		}

		DsVec3<TYPE>& operator *=(const TYPE& src )
		{
			x *= src;
			y *= src;
			z *= src;
			return (*this);
		}

		DsVec3<TYPE>& operator /=(const TYPE& src )
		{
			x /= src;
			y /= src;
			z /= src;
			return (*this);
		}


		double Length() const
		{
			return sqrt(x*x + y*y + z*z);
		}

		//dirベクトル成分をなくす
		DsVec3<TYPE>  Flat(const DsVec3<TYPE>& dir ) const
		{
			const DsVec3<TYPE> normal = Normal(dir);
			const TYPE len = Dot(*this, normal);
			const DsVec3<TYPE> flat = (*this) - (normal*len);
			return flat;
		}

		static DsVec3<TYPE> Normal(const DsVec3<TYPE>& src )
		{
			const TYPE len = src.Length();
			if( 0.000001 > len)
			{
				const DsVec3<TYPE> dst = {0.0, 0.0, 0.0};	
				return dst;
			}
			else
			{
				const DsVec3<TYPE> dst = { src.x/len, src.y/len, src.z/len};
				return dst;
			}
		}

		static DsVec3<TYPE> Get(TYPE x, TYPE y, TYPE z)
		{
			const DsVec3<TYPE> dst = {x, y, z};
			return dst;
		}

		static DsVec3<TYPE> Up()
		{
			return DsVec3<TYPE>::GetY();
		}

		static TYPE Dot(const DsVec3<TYPE>& src1, const DsVec3<TYPE>& src2)
		{
			return ( src1.x*src2.x + src1.y*src2.y + src1.z*src2.z );
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

		static const DsVec3<TYPE>& GetX()
		{
			static const DsVec3<TYPE> dst = {1.0, 0.0, 0.0};
			return dst;
		}

		static const DsVec3<TYPE>& GetY()
		{
			static const DsVec3<TYPE> dst = {0.0, 1.0, 0.0};
			return dst;
		}

		static const DsVec3<TYPE>& GetZ()
		{
			static const DsVec3<TYPE> dst = {0.0, 0.0, 1.0};
			return dst;
		}

		static const DsVec3<TYPE>& Zero()
		{
			static const DsVec3<TYPE> dst = {0.0, 0.0, 0.0};
			return dst;
		}
	};

	typedef DsVec3<double>	DsVec3d;
	typedef DsVec3<float>	DsVec3f;

	inline DsVec3f CastDsVec3d( const DsVec3d& src )
	{
		const DsVec3f dst =
		{
			static_cast<float>(src.x),
			static_cast<float>(src.y),
			static_cast<float>(src.z),
		};
		return dst;
	}

	inline DsVec3d CastDsVec3f( const DsVec3f& src )
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


		void Set(DsVec3d a, DsVec3d b, DsVec3d c, DsVec3d d )
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
}

#endif