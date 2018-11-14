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
			if (1 < scale) {//�����_���傫�������̂Ŗ߂�
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
		const double drDir = M_PI * 2.0 / static_cast<double>(createNum);//createNum�ň��
		for (int i = 0; i < createNum; ++i) {
			DsSquareParticle p;
			
			const double yAng = _Rand(0.0, RAND_Y_ANG);
			const DsMat33d drY = DsMat33d::RotateAxis(xAxis, -yAng);
			DsVec3d ver = drY * vertical;
			DsVec3d dir = drY * m_reqDir;

			//m_reqDir�ɐ��������ɏo��
			p.pos[0] = m_reqPos;
			const double w = _Rand(BASE_WIDTH, RAND_WIDTH_RATE);
			p.pos[1] = m_reqPos + (ver * w);

			//m_reqDir�����֐L�΂�
			const double l = _Rand(BASE_LEN, RAND_LEN_RATE);
			p.pos[2] = p.pos[1] + (dir*l);
			p.pos[3] = dir * l;

			const double v = _Rand(BASE_VEL, RAND_VEL_RATE);
			p.speed = dir * v;

			p.lifeTime = PARTICLE_LIFE_TIME;

			//������������]
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

