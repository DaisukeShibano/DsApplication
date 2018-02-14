#ifndef __DS_COLLISION_RESULT__
#define __DS_COLLISION_RESULT__

#ifndef __DS_ACTOR__
#include "Actor/DsActor.h"
#endif

namespace DsPhysics
{
	class DsCollisionResult
	{
	private:
		enum
		{
			DEFAULT_COL_NUM = 16,
		};

	public:
		struct ColInfo
		{
			ColInfo( DsVec3d& p, DsVec3d& n, double d, DsActorId& o1, DsActorId& o2)
				: colPos(p)
				, colNormal(n)
				, depth(d)
				, ownerId_1(o1)
				, ownerId_2(o2)
			{}
			DsVec3d& colPos;
			DsVec3d& colNormal;
			double	depth;
			DsActorId& ownerId_1;
			DsActorId& ownerId_2;
		};
		typedef std::vector<ColInfo> ColInfos;

	public:
		DsCollisionResult()
			: m_colPos(DEFAULT_COL_NUM)
			, m_colNormal(DEFAULT_COL_NUM)
			, m_depth(DEFAULT_COL_NUM)
			, m_ownerId_1(DEFAULT_COL_NUM)
			, m_ownerId_2(DEFAULT_COL_NUM)
			, m_colCounter(0)
			, m_infos()
		{
			//m_colPos.clear();
			//m_colNormal.clear();
			//m_depth.clear();
			//m_ownerId_1.clear();
			//m_ownerId_2.clear();
			//m_infos.clear();
		}

		DsCollisionResult(const DsCollisionResult& src)
		{
			(*this) = src;
		}
		
		void operator =(const DsCollisionResult& src )
		{
			//実際に使われてる衝突点外はコピーしなくていい
			m_colCounter = src.m_colCounter;
			{
				m_colPos = src.m_colPos;
				m_colNormal = src.m_colNormal;
				m_depth = src.m_depth;
				m_ownerId_1 = src.m_ownerId_1;
				m_ownerId_2 = src.m_ownerId_2;
				//m_infosはコピーしなくていい。コピーしたくないから参照で返す用の衝突点情報
			}
		}

		virtual ~DsCollisionResult(){}

	public:
		//衝突点を１点に纏める
		static DsCollisionResult Average( const DsCollisionResult& src )
		{
			DsCollisionResult ret;
			if( src.GetColNum() > 0 )
			{
				DsVec3d avePos = DsVec3d::Zero();
				DsVec3d aveNlm = DsVec3d::Zero();
				double aveDepth = 0.0;
				const int n = src.GetColNum();
				for(int i=0; i<n; ++i)
				{
					avePos += src.m_colPos[i];
					aveNlm += src.m_colNormal[i];
					aveDepth += src.m_depth[i];
				}
				avePos /= static_cast<double>(n);
				aveNlm = DsVec3d::Normalize(aveNlm);
				aveDepth /= static_cast<double>(n);
			
				ret.AddInfo(avePos, aveNlm, aveDepth, src.m_ownerId_1[0], src.m_ownerId_2[0] );
			}
			return ret;
		}


	public:
		int GetColNum() const { return m_colCounter; }

		void GetInfo(const int idx, DsVec3d& pos, DsVec3d& normal, double &depth, DsActorId& id1, DsActorId& id2) const
		{
			if( m_colCounter > idx )
			{
				pos = m_colPos[ idx ];
				normal = m_colNormal[ idx ];
				depth = m_depth[ idx ];
				id1 = m_ownerId_1[ idx ];
				id2 = m_ownerId_2[ idx ];
			}
		}

		void AddInfo(const DsVec3d& pos, const DsVec3d& normal, const double depth, const DsActorId& id1, const DsActorId& id2)
		{
			if( DEFAULT_COL_NUM > m_colCounter )
			{
				DS_ASSERT( -0.0000001 < depth, "めり込み量がマイナス" );

				m_colPos[m_colCounter]=pos;
				m_colNormal[m_colCounter] = normal;
				m_depth[m_colCounter] = depth;
				m_ownerId_1[m_colCounter] = id1;
				m_ownerId_2[m_colCounter] = id2;
				++m_colCounter;
			}
		}

		void AddInfo(const DsCollisionResult& add )
		{
			for (int i = 0; i < add.m_colCounter; ++i)
			{
				AddInfo(add.m_colPos[i], add.m_colNormal[i], add.m_depth[i], add.m_ownerId_1[i], add.m_ownerId_2[i]);
			}
		}

		void Clear()
		{
			//m_colPos.clear();
			//m_colNormal.clear();
			//m_depth.clear();
			//m_ownerId_1.clear();
			//m_ownerId_2.clear();
			//m_infos.clear();
			m_colCounter = 0;
		}

		const DsActorId GetHitActorId_1() const
		{
			if (m_ownerId_1.empty())
			{
				return DsActorId();
			}
			else
			{
				return m_ownerId_1[0];
			}
		}

		const DsActorId GetHitActorId_2() const
		{
			if (m_ownerId_2.empty())
			{
				return DsActorId();
			}
			else
			{
				return m_ownerId_2[0];
			}
		}

		const std::vector<DsVec3d>& RefPos() const{ return m_colPos; }
		const std::vector<DsVec3d>& RefNormal() const{ return m_colNormal; }//owner1を跳ね返す方向
		const std::vector<double>&	RefDepth() const { return m_depth; }//必ずプラス
		const std::vector<DsActorId>& RefOwnerId1() const { return m_ownerId_1; }
		const std::vector<DsActorId>& RefOwnerId2() const { return m_ownerId_2; }

		
		//basePosに一番近い衝突点を取得。衝突点がなければfalseを返す
		bool GetNearColPoint(const DsVec3d& basePos, DsVec3d& nearPos ) const
		{
			if( 0 >= m_colCounter ) return false; 

			nearPos = m_colPos[0];
			double minLen = fabs( basePos.Length() - m_colPos[0].Length() );
			for( int i=1; m_colCounter > i; ++i )
			{
				const double len = fabs( basePos.Length() - m_colPos[i].Length() );
				if( len < minLen )
				{
					minLen = len;
					nearPos = m_colPos[i];
				}
			}
			return true;
		}

		//衝突点を整理する
		void ModefyInfo();

		//衝突点の情報を得る
		ColInfos& MakeInfos();

		//加工せず全ての衝突点を得る
		ColInfos& MakeInfosAll()
		{
			m_infos.clear();
			for (int i = 0; i < m_colCounter; ++i)
			{
				ColInfo info(m_colPos[i], m_colNormal[i], m_depth[i], m_ownerId_1[i], m_ownerId_2[i]);
				m_infos.push_back(info);
			}
			return m_infos;
		}

	public:
		std::vector<DsVec3d> m_colPos;
		std::vector<DsVec3d> m_colNormal;
		std::vector<double>	m_depth;
		std::vector<DsActorId> m_ownerId_1;
		std::vector<DsActorId> m_ownerId_2;
		int m_colCounter;
		ColInfos m_infos;
	};
}

#endif