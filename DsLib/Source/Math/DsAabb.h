#ifndef _DS_AABB_H_
#define _DS_AABB_H_

namespace DsLib
{
	class DsDrawCommand;
}
namespace DsLib
{
	class DsAabb
	{
	public:
		DsAabb();
		virtual ~DsAabb();

	public:
		/*
		max +X側/+Y側/+Z側
		全部プラス
		*/
		void Setup(double maxX, double maxY, double maxZ, double minX, double minY, double minZ);
		void Setup(const DsVec3d& max, const DsVec3d& min);
		DsVec3d GetMax() const { return DsVec3d(m_maxX, m_maxY, m_maxZ); }
		DsVec3d GetMin() const { return DsVec3d(m_minX, m_minY, m_minZ); }
		DsVec3d GetCenter() const { return m_center; }
		
		//一番長い長さ
		const double GetMaxLen() const
		{
			const double x = m_maxX - m_minX;
			const double y = m_maxY - m_minY;
			const double z = m_maxZ - m_minZ;
			return max(x, max(y, z));
		}
		
	public:
		static inline bool IsContain(const DsAabb& a, const DsAabb& b)
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
			return (a.m_minX <= b.m_maxX) && (a.m_minY <= b.m_maxY) && (a.m_minZ <= b.m_maxZ)  && (b.m_minX <= a.m_maxX) && (b.m_minY <= a.m_maxY) && (b.m_minZ <= a.m_maxZ);
#endif
		}

	public:
		void Draw(DsDrawCommand& com) const;

	private:
		union{
			double m_maxV[3];
			struct{
				double m_maxX;
				double m_maxY;
				double m_maxZ;
			};
		};

		union {
			double m_minV[3];
			struct {
				double m_minX;
				double m_minY;
				double m_minZ;
			};
		};

		DsVec3d m_center;
	};
}

#endif