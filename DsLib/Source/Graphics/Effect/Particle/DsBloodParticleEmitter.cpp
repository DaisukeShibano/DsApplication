#include "DsPch.h"
#include "Graphics/Effect/Particle/DsBloodParticleEmitter.h"
//���̃w�b�_


using namespace DsLib;

namespace
{
	//�p�����[�^�ł��������������Ǎ��͂Ƃ肠�������ߑł�
	static const double PARTICLE_LIFE_TIME = 1.5;//�p�[�e�B�N������
	static const int BASE_CREATE_NUM = 8;
	static const double GRAVITY = -9.8;
	static const double BASE_WIDTH = 0.1;
	static const double RAND_WIDTH_RATE = 0.3;//�������_���l
	static const double BASE_LEN = 1.0;
	static const double RAND_LEN_RATE = 0.5;//�����������������_���l
	static const double BASE_VEL = 1.0;
	static const double RAND_VEL_RATE = 0.3;//���x�����_���l
	static const double RAND_Y_ANG = DegToRad(20.0);//�}�p�����_���l
	static const double RAND_Y_ANG_RATE = 0.3;


	double _Rand(double val, double rate)
	{
		double d = fabs(val) * fabs(rate);

		//�����_�ȉ��̃����_���l���o�����߂̈ꎞ��グ
		int scale = 100;

		int rmax = static_cast<int>( (fabs(val) + fabs(d)) * static_cast<double>(scale) );
		int rmin = max(0, static_cast<int>( (fabs(val) - fabs(d)) * static_cast<double>(scale)));

		if (rmax != rmin) {
			int randVal = (rand() % (rmax - rmin)) + rmin;
			double ret = static_cast<double>(randVal);
			//�����_���傫�������̂Ŗ߂�
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
	, m_lineTime(-1)
	, m_isRequest(false)
{
	m_texPath = "TestParticle.tga";
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
		const double drDir = M_PI * 2.0 / static_cast<double>(createNum);//createNum�ň��
		for (int i = 0; i < createNum; ++i) {
			DsSquareParticle p;
			
			const double yAng = _Rand(RAND_Y_ANG, RAND_Y_ANG_RATE);
			const DsMat33d drY = DsMat33d::RotateAxis(xAxis, -yAng);
			DsVec3d ver = drY * vertical;
			DsVec3d dir = drY * m_reqDir;

			//m_reqDir�ɐ��������ɏo��
			p.pos[0] = m_reqPos;
			const double h = _Rand(BASE_WIDTH, RAND_WIDTH_RATE);
			p.pos[1] = m_reqPos + (ver * h);

			//m_reqDir�����֐L�΂�
			const double w = _Rand(BASE_LEN, RAND_LEN_RATE);
			p.pos[2] = p.pos[1] + (dir*w);
			p.pos[3] = p.pos[2] - (ver * h);

			const double v = _Rand(BASE_VEL, RAND_VEL_RATE);
			p.speed = dir * v;

			p.lifeTime = PARTICLE_LIFE_TIME;

			//������������]
			vertical = DsMat33d::RotateAxis(m_reqDir, static_cast<double>(i)*drDir) * vertical;

			m_particle.push_back(p);
		}

		m_lineTime = GetParticleMaxLifeTime();
	}
	else {

		m_lineTime -= dt;
		if (0.0 >= m_lineTime) {
			m_particle.clear();
			m_lineTime = -1;
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
double DsBloodParticleEmitter::GetParticleMaxLifeTime()const//override
{
	return PARTICLE_LIFE_TIME;
}

bool DsBloodParticleEmitter::IsEmpty()const
{
	return (m_lineTime < 0.0);
}

//virtual 
void DsBloodParticleEmitter::EnumParticle(const EnumType& func) const //override
{
	for (const DsSquareParticle& p : m_particle) {
		func(p);
	}
}