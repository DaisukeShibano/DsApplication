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
			const double* m = mat.mat;

			// ç≈ëÂê¨ï™Çåüçı
			const TYPE elem[4] = { // 0:x, 1:y, 2:z, 3:w
				m[0] - m[4] - m[8] + static_cast<TYPE>(1.0),
				-m[0] + m[4] - m[8] + static_cast<TYPE>(1.0),
				-m[0] - m[4] + m[8] + static_cast<TYPE>(1.0),
				m[0] + m[4] + m[8] + static_cast<TYPE>(1.0),
			};

			unsigned biggestIndex = 0;
			for (int i = 1; i < 4; i++) {
				if (elem[i] > elem[biggestIndex])
					biggestIndex = i;
			}

			if (elem[biggestIndex] < static_cast<TYPE>(0.0) )
			{
				return; // à¯êîÇÃçsóÒÇ…ä‘à·Ç¢Ç†ÇËÅI
			}

			// ç≈ëÂóvëfÇÃílÇéZèo
			TYPE v = static_cast<TYPE>(sqrt(elem[biggestIndex]) *0.5);
			q[biggestIndex] = v;
			TYPE mult = static_cast<TYPE>(0.25) / v;

			switch (biggestIndex) {
			case 0: // x
				q[1] = (m[1] + m[3]) * mult;
				q[2] = (m[6] + m[2]) * mult;
				q[3] = (m[5] - m[7]) * mult;
				break;
			case 1: // y
				q[0] = (m[1] + m[3]) * mult;
				q[2] = (m[5] + m[7]) * mult;
				q[3] = (m[6] - m[2]) * mult;
				break;
			case 2: // z
				q[0] = (m[6] + m[2]) * mult;
				q[1] = (m[5] + m[7]) * mult;
				q[3] = (m[1] - m[3]) * mult;
				break;
			case 3: // w
				q[0] = (m[5] - m[7]) * mult;
				q[1] = (m[6] - m[2]) * mult;
				q[2] = (m[1] - m[3]) * mult;
				break;
			}
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

		DsMat33d ToMat33d() const
		{
			DsMat33d ret;

			ret.mat[0] = 1.0 - 2.0 * y * y - 2.0 * z * z;
			ret.mat[1] = 2.0 * x * y + 2.0 * w * z;
			ret.mat[2] = 2.0 * x * z - 2.0 * w * y;

			ret.mat[3] = 2.0 * x * y - 2.0 * w * z;
			ret.mat[4] = 1.0 - 2.0 * x * x - 2.0 * z * z;
			ret.mat[5] = 2.0 * y * z + 2.0 * w * x;

			ret.mat[6] = 2.0 * x * z + 2.0 * w * y;
			ret.mat[7] = 2.0 * y * z - 2.0 * w * x;
			ret.mat[8] = 1.0 - 2.0 * x * x - 2.0 * y * y;

			return ret;
		}

		TYPE Dot(const DsQuaternion& b) const
		{
			return (x*b.x + y*b.y + z*b.z + w*b.w);
		}

		//é©ï™Å©from ÇÃï‚ä‘ÅB rate=ÇOÇ≈é©ï™ÅArate=1Ç≈from
		DsQuaternion<TYPE> LinearInterpolation(const DsQuaternion<TYPE>& from, TYPE rate) const
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