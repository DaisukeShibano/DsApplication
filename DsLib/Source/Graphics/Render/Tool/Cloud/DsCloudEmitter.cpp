#include "DsPch.h"
#include "Graphics/Render/Tool/Cloud/DsCloudEmitter.h"
#include "Graphics/Render/Tool/Cloud/DsCloudIns.h"
#include "Graphics/Camera/DsCamera.h"

using namespace DsLib;

namespace
{
	static const float _SORT_TIME = 10.0f;//[s] //zソート実行間隔
}

DsCloudEmitter::DsCloudEmitter(const DsCamera& cam)
: m_clouds()
, m_cam(cam)
, m_isEnable(false)
, m_sortTime(-1.0f)
{
}

DsCloudEmitter::~DsCloudEmitter()
{
	for each(DsCloudIns* pCloud in m_clouds)
	{
		delete pCloud;
	}
}

void DsCloudEmitter::Initialize(unsigned int texId)
{
	m_sortTime = _SORT_TIME;
	const int initNum = 32;
	for (int i = 0; i < initNum; ++i)
	{
		DsCloudIns* pCloud = new DsCloudIns(texId);
		if (pCloud)
		{
			m_clouds.push_back(pCloud);
		}
	}
}

void DsCloudEmitter::Update(float dt)
{
	for each(DsCloudIns* pCloud in m_clouds)
	{
		if (!pCloud->IsDead())
		{
			pCloud->Update(dt, m_cam.GetRot());
		}
		else
		{
			//カメラの周囲に発生
			const double rad = static_cast<double>(rand() % 628) / 100.0f;
			const double lenOffset = static_cast<double>(rand() % static_cast<int>(m_cam.GetFar()) / 4);
			const double heigh = static_cast<double>(rand() % 400) + 100.0f;
			const double len = m_cam.GetFar() / 2.0f + lenOffset;
			const DsVec3d& camPos = m_cam.GetPos();
			const DsVec3d pos(len*cos(rad) + camPos.x, heigh, len*sin(rad) + camPos.z);
			pCloud->Initialize(pos);
		}
	}

	if (_SORT_TIME <= m_sortTime)
	{
		_SortEffects();
		m_sortTime = 0.0f;
	}

	m_sortTime += dt;
}

void DsCloudEmitter::Enable()
{
	m_isEnable = true;
}

void DsCloudEmitter::Disable()
{
	m_isEnable = false;
}

void DsCloudEmitter::_SortEffects()
{
	class Prd
	{
	public:
		Prd(const DsVec3f& camPos) :m_camPos(camPos){};

		bool operator()(const DsCloudIns* left, const DsCloudIns* right)
		{
			const double leftVal = (left->GetPos() - m_camPos).Length();
			const double rightVal = (right->GetPos() - m_camPos).Length();

			//カメラに近い順
			return (leftVal > rightVal);
		}
	private:
		const DsVec3f m_camPos;
	};

	std::sort( m_clouds.begin(), m_clouds.end(), Prd(m_cam.GetPos()) );
}