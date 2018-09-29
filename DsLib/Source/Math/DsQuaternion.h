#ifndef _DS_QUQTERNION_
#define _DS_QUQTERNION_

namespace DsLib
{
	template<typename TYPE >
	struct DsQuaternion
	{
		DsQuaternion(){}
		DsQuaternion(TYPE _x, TYPE _y, TYPE _z, TYPE _w)
			:x(_x), y(_y), z(_z), w(_w){}

		DsQuaternion(const DsVec4d v)
			:x(v.x), y(v.y), z(v.z), w(v.w) {}
		
		DsQuaternion(const DsMat33d mat )
			:x(0), y(0), z(0), w(0)
		{
			(*this) = DsMathUtil::ToQuaternion<TYPE>(mat);
		}

		union
		{
			TYPE q[4];
			struct
			{
				TYPE x;
				TYPE y;
				TYPE z;
				TYPE w;
			};
		};

		DsQuaternion<TYPE> operator +(const DsQuaternion<TYPE>& src) const
		{
			return DsQuaternion<TYPE>(x+src.x, y+src.y, z+src.z, w+src.w);
		}
		
		DsQuaternion<TYPE> operator -(const DsQuaternion<TYPE>& src) const
		{
			return DsQuaternion<TYPE>(x - src.x, y - src.y, z - src.z, w - src.w);
		}
		
		DsQuaternion<TYPE> operator *(TYPE val) const
		{
			return DsQuaternion<TYPE>(x*val, y*val, z*val, w*val);
		}

		DsQuaternion<TYPE> operator*(const DsQuaternion<TYPE>& r) const
		{
			DsQuaternion<TYPE> ret;
			ret.x = (x*r.x - y * r.y - z * r.z - w * r.w);
			ret.y = (x*r.y + y * r.x + z * r.w - w * r.z);
			ret.z = (x*r.z - y * r.w + z * r.x + w * r.y);
			ret.w = (x*r.w + y * r.z - z * r.y + w * r.x);
			return ret;
		}

		DsQuaternion<TYPE> operator /(TYPE val) const
		{
			return DsQuaternion<TYPE>(x/val, y/val, z/val, w/val);
		}

		TYPE Dot(const DsQuaternion& b) const
		{
			return (x*b.x + y*b.y + z*b.z + w*b.w);
		}

		//é©ï™Å©from ÇÃï‚ä‘ÅB rate=ÇOÇ≈é©ï™ÅArate=1Ç≈from
		DsQuaternion<TYPE> LinearInterpolation(const DsQuaternion<TYPE>& from, TYPE rate) const
		{
			const TYPE dot = min(1.0, max(-1.0, Dot(from)));
			if ((dot < -0.00000001) ||(0.9999999 < dot))
			{
				return (*this);
			}
			const TYPE w = acos(dot);
			const TYPE sn = sin(w);
			const TYPE fromC = (sin(rate*w) / sn);
			const TYPE toC = (sin((static_cast<TYPE>(1.0) - rate)*w) / sn);
			return ((*this)*toC) + (from*fromC);
		}

		//ï ÇÃé¿ëï
		//https://wgld.org/s/sample_021/
		/*
		rate   0 ÅÀ 1 ( form ÅÀ to )
		*/
		static DsQuaternion<TYPE> LinearInterpolation(const DsQuaternion<TYPE>& from, const DsQuaternion<TYPE>& to, TYPE rate) 
		{
			DsQuaternion<TYPE> ret;
			const TYPE ht = min(1.0, max(-1.0, from.Dot(to)));
			TYPE hs = 1.0 - ht * ht;
			if (hs <= 0.0) {
				ret = from;
			}
			else {
				hs = sqrt(hs);
				if (fabs(hs) < 0.0000001) {
					ret = (from * 0.5) + (to * 0.5);
				}
				else {
					const TYPE ph = acos(ht);
					const TYPE pt = ph * rate;
					const TYPE t0 = sin(ph - pt) / hs;
					const TYPE t1 = sin(pt) / hs;
					ret = (from * t0) + (to * t1);
				}
			}
			return ret;
		}

		static DsQuaternion<TYPE> Inverse(const DsQuaternion<TYPE>& in)
		{
			DsQuaternion<TYPE> ret;
			const TYPE n = in.x*in.x + in.y*in.y + in.z*in.z + in.w*in.w;
			ret.x = in.x / n;
			ret.y = -in.y / n;
			ret.z = -in.z / n;
			ret.w = -in.w / n;
			return ret;
		}
	};

	typedef DsQuaternion<double> DsQuaterniond;
	typedef DsQuaternion<float>	DsQuaternionf;
}



#endif