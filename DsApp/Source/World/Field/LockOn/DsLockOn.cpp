#include "DsAppPch.h"
#include "World/Field/LockOn/DsLockOn.h"
//���̃w�b�_
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
				//���߂��A���O�����ɂ���΍����_
				const double lenScore = 1.0 - (len / m_maxRange);
				const double dirScore = dot;
				const double score = lenScore + dirScore;
				if (hiScore < score) {
					hiScore = score;
					pBestPoint = pPoint;
				}

			}
			else {
				//���ɂ��郍�b�N�I���|�C���g�͏��O
			}

		}
		else {
			//�����ɏd�Ȃ��Ă���̂͏��O
			//�͈͊O�̂��̂͏��O
		}
	}

	m_pBestPoint = pBestPoint;
}


//���b�N�I���|�C���g���Ȃ����false
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
		//��苗�����ꂽ�烍�b�N�I������
		const double len = (m_pBestPoint->GetPos() - pos).LengthSq();
		if (MAX_RANGE_DEFAULT*MAX_RANGE_DEFAULT < len) {
			m_pBestPoint = NULL;
		}
	}
}