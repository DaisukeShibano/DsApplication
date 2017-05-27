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
		DsQuaternion<TYPE> operator /(TYPE val) const
		{
			return DsQuaternion<TYPE>(x/val, y/val, z/val, w/val);
		}

		TYPE Dot(const DsQuaternion& b) const
		{
			return (x*b.x + y*b.y + z*b.z + w*b.w);
		}

		//é©ï™Å©from ÇÃï‚ä‘ÅB rate=ÇOÇ≈é©ï™ÅArate=1Ç≈from
		DsQuaternion LinearInterpolation(const DsQuaternion& from, TYPE rate) const
		{
			const TYPE dot = min(1.0, max(-1.0, Dot(from)));
			//ÇOèúéZÇ™î≠ê∂ÇµÇƒÇµÇ‹Ç§ÇΩÇﬂÅB
			if ((dot < 0.00000001) || (0.99999999 < dot))
			{
				return (*this);
			}
			const TYPE w = acos(dot);
			const TYPE sn = sin(w);
			const TYPE fromC = (sin(rate*w) / sn);
			const TYPE toC = (sin((static_cast<TYPE>(1.0) - rate)*w) / sn);
			return ((*this)*toC) + (from*fromC);
		}
	};

	typedef DsQuaternion<double> DsQuaterniond;
	typedef DsQuaternion<float>	DsQuaternionf;
}



#endif