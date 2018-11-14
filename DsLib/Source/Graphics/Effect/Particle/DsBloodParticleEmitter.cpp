#include "DsPch.h"
#include "Graphics/Effect/Particle/DsBloodParticleEmitter.h"
//他のヘッダ


using namespace DsLib;

namespace
{
	//パラメータでもいいかもだけど今はとりあえず決め打ち
	static const double PARTICLE_LIFE_TIME = 1.5;//パーティクル寿命
	static const int BASE_CREATE_NUM = 8;
	static const double GRAVITY = -9.8;
	static const double BASE_WIDTH = 0.1;
	static const double RAND_WIDTH_RATE = 0.3;//幅ランダム値
	static const double BASE_LEN = 1.0;
	static const double RAND_LEN_RATE = 0.5;//発生方向長さランダム値
	static const double BASE_VEL = 1.0;
	static const double RAND_VEL_RATE = 0.3;//速度ランダム値
	static const double RAND_Y_ANG = DegToRad(20.0);//迎角ランダム値


	double _Rand(double val, double rate)
	{
		double d = fabs(val) * fabs(rate);

		//小数点以下のランダム値を出すための一時底上げ
		int scale = 100;

		int rmax = static_cast<int>( (fabs(val) + fabs(d)) * static_cast<double>(scale) );
		int rmin = max(0, static_cast<int>( (fabs(val) - fabs(d)) * static_cast<double>(scale)));

		if (rmax != rmin) {
			int randVal = (rand() % (rmax - rmin)) + rmin;
			double ret = static_cast<double>(randVal);
			if (1 < scale) {//小数点分大きくしたので戻す
				d /= static_cast<double>(scale - 1);
			}
			return ret;
		}
		else {
			return val;
		}
	}

}

DsBloodParticleEmitter::DsBloodParticleEmitter()
	: m_particle()
	, m_reqPos()
	, m_reqDir()
	, m_texPath()
	, m_lineTime(-1)
	, m_isRequest(false)
{
	m_texPath = "TestParticle2.tga";
	m_particle.reserve(BASE_CREATE_NUM);
}

DsBloodParticleEmitter::~DsBloodParticleEmitter()
{
}

void DsBloodParticleEmitter::Update(double dt)
{
	const bool isCreate = m_isRequest && m_particle.empty();
	if (isCreate) {

		const DsVec3d xAxis = DsVec3d::Cross(DsVec3d::GetY(), m_reqDir);
		const DsVec3d yAxis = DsVec3d::Cross(m_reqDir, xAxis);
		DsVec3d vertical = yAxis;

		const int createNum = BASE_CREATE_NUM;
		const double drDir = M_PI * 2.0 / static_cast<double>(createNum);//createNumで一周
		for (int i = 0; i < createNum; ++i) {
			DsSquareParticle p;
			
			const double yAng = _Rand(0.0, RAND_Y_ANG);
			const DsMat33d drY = DsMat33d::RotateAxis(xAxis, -yAng);
			DsVec3d ver = drY * vertical;
			DsVec3d dir = drY * m_reqDir;

			//m_reqDirに垂直方向に出す
			p.pos[0] = m_reqPos;
			const double w = _Rand(BASE_WIDTH, RAND_WIDTH_RATE);
			p.pos[1] = m_reqPos + (ver * w);

			//m_reqDir方向へ伸ばす
			const double l = _Rand(BASE_LEN, RAND_LEN_RATE);
			p.pos[2] = p.pos[1] + (dir*l);
			p.pos[3] = dir * l;

			const double v = _Rand(BASE_VEL, RAND_VEL_RATE);
			p.speed = dir * v;

			p.lifeTime = PARTICLE_LIFE_TIME;

			//垂直方向を回転
			vertical = DsMat33d::RotateAxis(m_reqDir, static_cast<double>(i)*drDir) * vertical;

			m_particle.push_back(p);
		}

	}
}

void DsBloodParticleEmitter::RequestEmit(const DsVec3d& pos, const DsVec3d& dir)
{
	m_isRequest = true;
	m_reqPos = pos;
	m_reqDir = dir;
}

double DsBloodParticleEmitter::GetParticleMaxLifeTime()const
{
	return PARTICLE_LIFE_TIME;
}

bool DsBloodParticleEmitter::IsEmpty()const
{
	return (0.0 < m_lineTime);
}

