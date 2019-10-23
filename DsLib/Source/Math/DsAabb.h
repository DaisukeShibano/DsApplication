#pragma once

#ifndef _DS_AABB_H_
#define _DS_AABB_H_

namespace DsLib
{
	class DsDrawCommand;
}
namespace DsLib
{
	template<typename _VAL, typename _VEC>
	class DsAabbBase
	{
	public:
		DsAabbBase()
		: m_maxX(0)
		, m_maxY(0)
		, m_maxZ(0)
		, m_center(_VEC::Zero())
		, m_extents(_VEC::Zero())
		{}

		~DsAabbBase()
		{}

	public:
		void Setup(_VAL maxX, _VAL maxY, _VAL maxZ, _VAL minX, _VAL minY, _VAL minZ)
		{
			m_maxX = max.x;
			m_maxY = max.y;
			m_maxZ = max.z;
			m_minX = min.x;
			m_minY = min.y;
			m_minZ = min.z;
			m_center = _VEC((m_maxX + m_minX)*0.5, (m_maxY + m_minY)*0.5, (m_maxZ + m_minZ)*0.5);
			m_extents = _VEC((m_maxX - m_minX)*0.5, (m_maxY - m_minY)*0.5, (m_maxZ - m_minZ)*0.5);
		}

		void Setup(const _VEC& max, const _VEC& min)
		{
			m_maxX = max.x;
			m_maxY = max.y;
			m_maxZ = max.z;
			m_minX = min.x;
			m_minY = min.y;
			m_minZ = min.z;
			m_center = _VEC((m_maxX + m_minX)*0.5, (m_maxY + m_minY)*0.5, (m_maxZ + m_minZ)*0.5);
			m_extents = _VEC((m_maxX - m_minX)*0.5, (m_maxY - m_minY)*0.5, (m_maxZ - m_minZ)*0.5);
		}

		_VEC GetMax() const { return _VEC(m_maxX, m_maxY, m_maxZ); }
		_VEC GetMin() const { return _VEC(m_minX, m_minY, m_minZ); }
		_VEC GetCenter() const { return m_center; }
		_VEC GetExtents() const { return m_extents; }

		//一番長い長さ
		_VAL GetMaxLen() const
		{
			const _VAL x = m_maxX - m_minX;
			const _VAL y = m_maxY - m_minY;
			const _VAL z = m_maxZ - m_minZ;
			return max(x, max(y, z));
		}

		_VAL LenX(){ return m_maxX - m_minX; }
		_VAL LenY(){ return m_maxY - m_minY; }
		_VAL LenZ(){ return m_maxZ - m_minZ; }

		_VAL GetMaxX() const { return m_maxX; }
		_VAL GetMaxY() const { return m_maxY; }
		_VAL GetMaxZ() const { return m_maxZ; }
		_VAL GetMinX() const { return m_minX; }
		_VAL GetMinY() const { return m_minY; }
		_VAL GetMinZ() const { return m_minZ; }

	public:
		static inline bool IsContain(const DsAabbBase<_VAL, _VEC>& a, const DsAabbBase<_VAL, _VEC>& b)
		{
#ifdef DS_SYS_USE_SIMD_aaaaaaaaaaaaaaaaaaaaaaa
			const __m256d dist = _mm256_andnot_pd(_mm256_set1_pd(-0.0), _mm256_sub_pd(*(__m256d*)(a.m_pos.v), *(__m256d*)(b.m_pos.v)) );//最上位ビットだけクリアして絶対値とる
			const __m256d max = _mm256_add_pd(*(__m256d*)(a.m_maxV), *(__m256d*)(b.m_maxV));
			const __m256d mask = _mm256_cmp_pd(dist, max, _CMP_LT_OS);
			return mask.m256d_f64[0] && mask.m256d_f64[1] && mask.m256d_f64[2];
#else
			//if (fabs(a.m_pos.x - b.m_pos.x) > (a.m_maxX + b.m_maxX)){
			//	return false;
			//}
			//if (fabs(a.m_pos.y - b.m_pos.y) > (a.m_maxY + b.m_maxY)){
			//	return false;
			//}
			//if (fabs(a.m_pos.z - b.m_pos.z) > (a.m_maxZ + b.m_maxZ)){
			//	return false;
			//}
			//return true;
			//return (fabs(a.m_pos.x - b.m_pos.x) < (a.m_maxX + b.m_maxX)) && (fabs(a.m_pos.y - b.m_pos.y) < (a.m_maxY + b.m_maxY)) && (fabs(a.m_pos.z - b.m_pos.z) < (a.m_maxZ + b.m_maxZ));
			return (a.m_minX <= b.m_maxX) && (a.m_minY <= b.m_maxY) && (a.m_minZ <= b.m_maxZ) && (b.m_minX <= a.m_maxX) && (b.m_minY <= a.m_maxY) && (b.m_minZ <= a.m_maxZ);
#endif
		}

	public:
		void Draw(DsDrawCommand& com) const
		{
			const _VEC v[8] =
			{
				//上面
				_VEC(m_maxX, m_maxY, m_maxZ),
				_VEC(m_maxX, m_maxY, m_minZ),
				_VEC(m_minX, m_maxY, m_minZ),
				_VEC(m_minX, m_maxY, m_maxZ),
				//下面
				_VEC(m_maxX, m_minY, m_maxZ),
				_VEC(m_maxX, m_minY, m_minZ),
				_VEC(m_minX, m_minY, m_minZ),
				_VEC(m_minX, m_minY, m_maxZ),
			};

			for (int i = 0; i < 4; ++i)
			{
				const int next = (i + 1) % 4;
				com.DrawLine(v[i], v[next]);
				com.DrawLine(v[i + 4], v[next + 4]);
				com.DrawLine(v[i], v[i + 4]);
			}
		}

	public:
		union{
			_VAL m_maxV[3];
			struct{
				_VAL m_maxX;
				_VAL m_maxY;
				_VAL m_maxZ;
			};
		};

		union {
			_VAL m_minV[3];
			struct {
				_VAL m_minX;
				_VAL m_minY;
				_VAL m_minZ;
			};
		};

		_VEC m_center;
		_VEC m_extents;
	};

	typedef DsAabbBase<double, DsVec3d> DsAabb;
	typedef DsAabbBase<double, DsVec3d> DsAabbd;
	typedef DsAabbBase<float, DsVec3f> DsAabbf;


	template<typename TYPE>
	class DsAabbUtil
	{
	public:
		//a[0]=maxX a[1]=maxY a[2]=maxZ
		//a[3]=minX a[4]=minY a[5]=minZ
		static bool IsContain(const TYPE* a, const TYPE* b)
		{
			return (a[3] <= b[0]) && (a[4] <= b[1]) && (a[5] <= b[2]) && (b[3] <= a[0]) && (b[4] <= a[1]) && (b[5] <= a[2]);
		}
	};
}

#endif