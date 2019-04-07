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
			(*this) = ToQuaternion(mat);
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

		//自分←from の補間。 rate=０で自分、rate=1でfrom
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

		//別の実装
		//https://wgld.org/s/sample_021/
		/*
		rate   0 ⇒ 1 ( form ⇒ to )
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

		static DsQuaternion<TYPE> ToQuaternion(const DsMat33d& mat)
		{
			DsQuaternion<TYPE> ret;

			// 最大成分を検索
			TYPE elem[4]; // 0:x, 1:y, 2:z, 3:w
			elem[0] = mat.m00 - mat.m11 - mat.m22 + 1.0;
			elem[1] = -mat.m00 + mat.m11 - mat.m22 + 1.0;
			elem[2] = -mat.m00 - mat.m11 + mat.m22 + 1.0;
			elem[3] = mat.m00 + mat.m11 + mat.m22 + 1.0;

			unsigned biggestIndex = 0;
			for (int i = 1; i < 4; i++) {
				if (elem[i] > elem[biggestIndex])
					biggestIndex = i;
			}

			if (elem[biggestIndex] < 0.0)
			{
				return ret; // 引数の行列に間違いあり！
			}

			// 最大要素の値を算出
			TYPE *q[4] = { &ret.x, &ret.y, &ret.z, &ret.w };
			TYPE v = static_cast<TYPE>(sqrt(elem[biggestIndex]) * 0.5);
			*q[biggestIndex] = v;
			TYPE mult = 0.25 / v;

			switch (biggestIndex) {
			case 0: // x
				*q[1] = (mat.m01 + mat.m10) * mult;
				*q[2] = (mat.m20 + mat.m02) * mult;
				*q[3] = (mat.m12 - mat.m21) * mult;
				break;
			case 1: // y
				*q[0] = (mat.m01 + mat.m10) * mult;
				*q[2] = (mat.m12 + mat.m21) * mult;
				*q[3] = (mat.m20 - mat.m02) * mult;
				break;
			case 2: // z
				*q[0] = (mat.m20 + mat.m02) * mult;
				*q[1] = (mat.m12 + mat.m21) * mult;
				*q[3] = (mat.m01 - mat.m10) * mult;
				break;
			case 3: // w
				*q[0] = (mat.m12 - mat.m21) * mult;
				*q[1] = (mat.m20 - mat.m02) * mult;
				*q[2] = (mat.m01 - mat.m10) * mult;
				break;
			}
			return ret;
		}

		static DsMat33d ToMat33d(const DsQuaternion<TYPE>& q)
		{
			DsMat33d ret;
			const double qx = static_cast<double>(q.x);
			const double qy = static_cast<double>(q.y);
			const double qz = static_cast<double>(q.z);
			const double qw = static_cast<double>(q.w);

			ret.m00 = 1.0 - 2.0 * qy * qy - 2.0 * qz * qz;
			ret.m01 = 2.0 * qx * qy + 2.0 * qw * qz;
			ret.m02 = 2.0 * qx * qz - 2.0 * qw * qy;

			ret.m10 = 2.0 * qx * qy - 2.0 * qw * qz;
			ret.m11 = 1.0 - 2.0 * qx * qx - 2.0f * qz * qz;
			ret.m12 = 2.0 * qy * qz + 2.0 * qw * qx;

			ret.m20 = 2.0 * qx * qz + 2.0 * qw * qy;
			ret.m21 = 2.0 * qy * qz - 2.0 * qw * qx;
			ret.m22 = 1.0 - 2.0 * qx * qx - 2.0 * qy * qy;

			return ret;
		}

		static DsMat33f ToMat33f(const DsQuaternion<TYPE>& q)
		{
			DsMat33f ret;
			const float qx = static_cast<float>(q.x);
			const float qy = static_cast<float>(q.y);
			const float qz = static_cast<float>(q.z);
			const float qw = static_cast<float>(q.w);

			ret.m00 = 1.0f - 2.0f * qy * qy - 2.0f * qz * qz;
			ret.m01 = 2.0f * qx * qy + 2.0f * qw * qz;
			ret.m02 = 2.0f * qx * qz - 2.0f * qw * qy;

			ret.m10 = 2.0f * qx * qy - 2.0f * qw * qz;
			ret.m11 = 1.0f - 2.0f * qx * qx - 2.0f * qz * qz;
			ret.m12 = 2.0f * qy * qz + 2.0f * qw * qx;

			ret.m20 = 2.0f * qx * qz + 2.0f * qw * qy;
			ret.m21 = 2.0f * qy * qz - 2.0f * qw * qx;
			ret.m22 = 1.0f - 2.0f * qx * qx - 2.0f * qy * qy;

			return ret;
		}
	};

	typedef DsQuaternion<double> DsQuaterniond;
	typedef DsQuaternion<float>	DsQuaternionf;
}



#endif