#include "DsAppPch.h"
#include "World/Field/LockOn/DsLockOn.h"
//他のヘッダ
#include "World/Field/LockOn/DsLockOnPoint.h"

using namespace DsApp;

namespace
{
	const double MAX_RANGE_DEFAULT = 10;//m
}

DsLockOn::DsLockOn()
	: m_points()
	, m_pBestPoint(NULL)
	, m_maxRange(MAX_RANGE_DEFAULT)
{
}

DsLockOn::~DsLockOn()
{
}

void DsLockOn::Register(DsLockOnPoint* pPoint)
{
	if (pPoint) {
		auto it = m_points.find(pPoint);
		if (it == m_points.end()) {
			m_points.insert(pPoint);
		}
	}
}

void DsLockOn::Unregister(DsLockOnPoint* pPoint)
{
	if (pPoint) {
		m_points.erase(pPoint);
		if (m_pBestPoint == pPoint) {
			m_pBestPoint = NULL;
		}
	}
}


void DsLockOn::LockOn(const DsVec3d& pos, const DsVec3d dir)
{
	DsLockOnPoint* pBestPoint = NULL;
	double hiScore = 0.0;
	for(DsLockOnPoint* pPoint : m_points){
		const DsVec3d toPoint = pPoint->GetPos() - pos;
		const double len = toPoint.Length();
		if( (0.0 != len) && (len <= m_maxRange) ) {
			const double dot = DsVec3d::Dot(dir, toPoint / len);

			if (0.0 < dot) {
				//より近く、より前方向にあれば高得点
				const double lenScore = 1.0 - (len / m_maxRange);
				const double dirScore = dot;
				const double score = lenScore + dirScore;
				if (hiScore < score) {
					hiScore = score;
					pBestPoint = pPoint;
				}

			}
			else {
				//後ろにあるロックオンポイントは除外
			}

		}
		else {
			//自分に重なっているのは除外
			//範囲外のものは除外
		}
	}

	m_pBestPoint = pBestPoint;
}


//ロックオンポイントがなければfalse
bool DsLockOn::GetLockOnPos(DsVec3d& pos)const
{
	bool ret = false;
	if (m_pBestPoint) {
		pos = m_pBestPoint->GetPos();
		ret = true;
	}
	return ret;
}

void DsLockOn::Update(double dt, const DsVec3d& pos)
{
	if (m_pBestPoint) {
		//一定距離離れたらロックオン解除
		const double len = (m_pBestPoint->GetPos() - pos).LengthSq();
		if (MAX_RANGE_DEFAULT*MAX_RANGE_DEFAULT < len) {
			m_pBestPoint = NULL;
		}
	}
}