#include "DsPch.h"
#include "Graphics/Effect/Particle/DsBloodParticleEmitter.h"
//他のヘッダ


using namespace DsLib;

namespace
{
	//パラメータでもいいかもだけど今はとりあえず決め打ち
	static const double PARTICLE_LIFE_TIME = 0.4;//パーティクル寿命
	static const int BASE_CREATE_NUM = 16;
	static const double GRAVITY = -28.0;
	static const double BASE_WIDTH = 0.36;
	static const double RAND_WIDTH_RATE = 0.55;//幅ランダム値
	static const double START_WIDTH_RATE = 0.2;//始点幅サイズ
	static const double BASE_LEN = 0.1;
	static const double RAND_LEN_RATE = 2.5;//発生方向長さランダム値
	static const double BASE_VEL = 14.0;
	static const double RAND_VEL_RATE = 0.45;//速度ランダム値
	static const double VEL_RESIST = 7.0;//速度減衰値
	static const double RAND_Y_ANG = DegToRad(40.0);//迎角ランダム値
	static const double RAND_Y_ANG_RATE = 0.9;


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
			//小数点分大きくしたので戻す
			ret /= static_cast<double>(scale);
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
	, m_lifeTime(-1)
	, m_isRequest(false)
{
	m_texPath = "blood.tga";
	m_particle.reserve(BASE_CREATE_NUM);
}

//virtual
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
			
			const double yAng = _Rand(RAND_Y_ANG, RAND_Y_ANG_RATE);
			const DsMat33d drY = DsMat33d::RotateAxis(xAxis, -yAng);
			DsVec3d ver = drY * vertical;
			DsVec3d dir = drY * m_reqDir;

			//m_reqDirに垂直方向に出す
			p.pos[0] = m_reqPos;
			const double h = _Rand(BASE_WIDTH, RAND_WIDTH_RATE);
			p.pos[1] = m_reqPos + (ver * h * START_WIDTH_RATE);//始点は細く

			//m_reqDir方向へ伸ばす
			const double w = _Rand(BASE_LEN, RAND_LEN_RATE);
			p.pos[2] = p.pos[1] + (dir*w);
			p.pos[3] = p.pos[2] - (ver * h);

			const double v = _Rand(BASE_VEL, RAND_VEL_RATE);
			p.speed = dir * v;

			p.lifeTime = PARTICLE_LIFE_TIME;

			//垂直方向を回転
			vertical = DsMat33d::RotateAxis(m_reqDir, static_cast<double>(i)*drDir) * vertical;

			m_particle.push_back(p);
		}

		m_lifeTime = PARTICLE_LIFE_TIME;
	}
	else {


		for (DsSquareParticle& p : m_particle) {
			p.speed -= p.speed*VEL_RESIST*dt;
			p.speed += DsVec3d(0, GRAVITY, 0)*dt;
			p.pos[2] += (p.speed*dt);
			p.pos[3] += (p.speed*dt);
		}


		m_lifeTime -= dt;
		if (0.0 >= m_lifeTime) {
			m_particle.clear();
			m_lifeTime = -1;
		}
	}
}

void DsBloodParticleEmitter::RequestEmit(const DsVec3d& pos, const DsVec3d& dir)
{
	m_isRequest = true;
	m_reqPos = pos;
	m_reqDir = dir;
}

//virtual
double DsBloodParticleEmitter::GetAlpha(const double lifeTime)const //override
{
	//http://d.hatena.ne.jp/nakamura001/20111117/1321539246
	double t = m_lifeTime / PARTICLE_LIFE_TIME;//時間(進行度)
	double b = 0.0;//開始の値(開始時の座標やスケールなど)
	double c = 1.0;//開始と終了の値の差分
	double d = 1.0;//Tween(トゥイーン)の合計時間

	//ease_out
	t /= d;
	t = t - 1.0;
	double val = c * (t*t*t*t*t + 1.0) + b;
	return m_lifeTime/PARTICLE_LIFE_TIME;
}

bool DsBloodParticleEmitter::IsEmpty()const
{
	return (m_lifeTime < 0.0);
}

//virtual 
void DsBloodParticleEmitter::EnumParticle(const EnumType& func) const //override
{
	for (const DsSquareParticle& p : m_particle) {
		func(p);
	}
}