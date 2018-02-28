#include "DsPch.h"
#include <windows.h>
#include <gl/GL.h>
#ifndef _DS_DRAW_COMMAND_H_
#include "Graphics/Command/DsDrawCommand.h"
#endif
#ifndef __DS_MATH_UTIL__
#include "Math/DsMathUtil.h"
#endif

#ifndef _DS_RENDER_H_
#include "Graphics/Render/DsRender.h"
#endif
#ifndef _DS_CAMERA_H_
#include "Graphics/Camera/DsCamera.h"
#endif

using namespace DsLib;


namespace
{
	//DsDrawCommand����DsDrawComBase�֓n�����B�O���Ɍ��J�������Ȃ�����
	struct _InnerCommandInfo
	{
		_InnerCommandInfo()
			: hFont(NULL)
			, pTexIdMap(NULL)
		{}
		HFONT hFont;
		std::map< const unsigned char*, unsigned int>* pTexIdMap;
	};


}


namespace
{
	/****************************
	@brief �L���[�u
	*****************************/
	class _ComBox : public DsDrawComBase
	{
	public:
		_ComBox(const DsVec3f& pos, const float size )
			:m_pos(pos)
			,m_size(size)
		{}
		virtual void Exe(DsDrawCommand& owner) override
		{
			const float len = m_size*0.5f;
			const float vertex[8][3]=
			{
				//+Z�͎�O
				{ len + m_pos.x, len + m_pos.y, -len + m_pos.z },//�E��
				{ len + m_pos.x, len + m_pos.y, len + m_pos.z },//�E��O
				{ -len + m_pos.x, len + m_pos.y, len + m_pos.z },//����O
				{ -len + m_pos.x, len + m_pos.y, -len + m_pos.z },//����
				{ len + m_pos.x, -len + m_pos.y, -len + m_pos.z },
				{ len + m_pos.x, -len + m_pos.y, len + m_pos.z },
				{ -len + m_pos.x, -len + m_pos.y, len + m_pos.z },
				{ -len + m_pos.x, -len + m_pos.y, -len + m_pos.z },
			};
			glBegin(GL_QUADS); glNormal3f(0, 1, 0);
			glVertex3fv(vertex[3]); glVertex3fv(vertex[2]); glVertex3fv(vertex[1]); glVertex3fv(vertex[0]);
			glEnd();
			glBegin(GL_QUADS); glNormal3f(0, -1, 0);
			glVertex3fv(vertex[4]); glVertex3fv(vertex[5]); glVertex3fv(vertex[6]); glVertex3fv(vertex[7]);
			glEnd();
			glBegin(GL_QUADS); glNormal3f(0, 0, 1);
			glVertex3fv(vertex[4]); glVertex3fv(vertex[7]); glVertex3fv(vertex[3]); glVertex3fv(vertex[0]);
			glEnd();
			glBegin(GL_QUADS); glNormal3f(0, 0, -1);
			glVertex3fv(vertex[2]); glVertex3fv(vertex[6]); glVertex3fv(vertex[5]); glVertex3fv(vertex[1]);
			glEnd();
			glBegin(GL_QUADS); glNormal3f(1, 0, 0);
			glVertex3fv(vertex[1]); glVertex3fv(vertex[5]); glVertex3fv(vertex[4]); glVertex3fv(vertex[0]);
			glEnd();
			glBegin(GL_QUADS); glNormal3f(-1, 0, 0);
			glVertex3fv(vertex[7]); glVertex3fv(vertex[6]); glVertex3fv(vertex[2]); glVertex3fv(vertex[3]);
			glEnd();
		}

	private:
		const DsVec3f m_pos;
		const float m_size;
	};

	/****************************
	@brief �X�t�B�A
	*****************************/
	class _ComSphere : public DsDrawComBase
	{
	public:
		_ComSphere(const DsVec3f& pos, const float radius)
			:m_pos(pos)
			,m_radius(radius)
		{}
		virtual void Exe(DsDrawCommand& owner) override
		{
			const int meshNum = 16;
			const float dRad = (2.0f*static_cast<float>(M_PI)) / static_cast<float>(meshNum);
			
			for (int si = 1; si <= meshNum; ++si)
			{
				for (int ci = 1; ci <= meshNum+1; ++ci)
				{
					const float rUp = m_radius*sin(dRad*static_cast<float>(si)*0.5f);
					const float rDown = m_radius*sin(dRad*static_cast<float>(si - 1)*0.5f);
					const float hUp = -m_radius*cos(dRad*static_cast<float>(si)*0.5f);
					const float hDown = -m_radius*cos(dRad*static_cast<float>(si - 1)*0.5f);

					//�E��
					const DsVec3f c0( m_pos.x + rDown*cos(static_cast<float>(ci - 1)*dRad),
									  m_pos.y + hDown,
									  m_pos.z + rDown*sin(static_cast<float>(ci - 1)*dRad));
					//����
					const DsVec3f c1( m_pos.x + rDown*cos(static_cast<float>(ci)*dRad),
									  m_pos.y + hDown,
									  m_pos.z + rDown*sin(static_cast<float>(ci)*dRad));
					//����
					const DsVec3f c2( m_pos.x + rUp*cos(static_cast<float>(ci)*dRad),
									  m_pos.y + hUp,
									  m_pos.z + rUp*sin(static_cast<float>(ci)*dRad));
					//�E��
					const DsVec3f c3( m_pos.x + rUp*cos(static_cast<float>(ci - 1)*dRad),
									  m_pos.y + hUp,
									  m_pos.z + rUp*sin(static_cast<float>(ci - 1)*dRad));

					const DsVec3f normal = DsVec3f::CalcNormal(c0, c1, c2);
					
					glNormal3fv(normal.v);
					glBegin(GL_QUADS);
					glVertex3fv(c3.v);
					glVertex3fv(c2.v);
					glVertex3fv(c1.v);
					glVertex3fv(c0.v);
					glEnd();
					
				}
			}
		}

	private:
		const DsVec3f m_pos;
		const float m_radius;
	};

	/****************************
	@brief ���C��
	*****************************/
	class _ComLine : public DsDrawComBase
	{
	public:
		_ComLine( const DsVec3f& p1, const DsVec3f& p2 )
		:m_p1(p1)
		,m_p2(p2)
		{}
	virtual void Exe(DsDrawCommand& owner) override
	{
		glBegin(GL_LINES);
		glVertex3fv(m_p1.v);
		glVertex3fv(m_p2.v);
		glEnd();
	}

	private:
		const DsVec3f m_p1;
		const DsVec3f m_p2;
	};

	/****************************
	@brief �J�v�Z��
	*****************************/
	class _ComCapsule : public DsDrawComBase
	{
	public:
		_ComCapsule(const DsVec3f& p1, const DsVec3f& p2, const float radius)
			: m_p1(p1)
			, m_p2(p2)
			, m_radius(radius)
		{}
		virtual void Exe(DsDrawCommand& owner) override
		{

			const int meshNum = 16;
			const float dRad = (2.0f*static_cast<float>(M_PI)) / static_cast<float>(meshNum);

			//������オ�J�v�Z���̕���
			const DsVec3f dir = m_p2 - m_p1;
			const DsVec3f rotAxis = DsVec3f::Normalize( DsVec3f::Cross(DsVec3f::Up(), dir) );
			const float rotAng = acosf( Clamp( DsVec3f::Dot(DsVec3f::Up(), DsVec3f::Normalize(dir)), -1.0f, 1.0f) );
			const DsMat33f rot = DsMat33f::RotateVec(rotAxis*rotAng);
			
			//������
			for (int si = 1; si <= meshNum/2; ++si)
			{
				for (int ci = 1; ci <= meshNum + 1; ++ci)
				{
					const float rUp = m_radius*sin(dRad*static_cast<float>(si)*0.5f);
					const float rDown = m_radius*sin(dRad*static_cast<float>(si - 1)*0.5f);
					const float hUp = -m_radius*cos(dRad*static_cast<float>(si)*0.5f);
					const float hDown = -m_radius*cos(dRad*static_cast<float>(si - 1)*0.5f);

					//�E��
					const DsVec3f c0(rDown*cos(static_cast<float>(ci - 1)*dRad),
						hDown,
						rDown*sin(static_cast<float>(ci - 1)*dRad));
					//����
					const DsVec3f c1(rDown*cos(static_cast<float>(ci)*dRad),
						hDown,
						rDown*sin(static_cast<float>(ci)*dRad));
					//����
					const DsVec3f c2(rUp*cos(static_cast<float>(ci)*dRad),
						hUp,
						rUp*sin(static_cast<float>(ci)*dRad));
					//�E��
					const DsVec3f c3(rUp*cos(static_cast<float>(ci - 1)*dRad),
						hUp,
						rUp*sin(static_cast<float>(ci - 1)*dRad));

					const DsVec3f face[4] =
					{
						(rot*c0) + m_p1,
						(rot*c1) + m_p1,
						(rot*c2) + m_p1,
						(rot*c3) + m_p1,
					};

					const DsVec3f normal = DsVec3f::CalcNormal(face[0], face[1], face[2]);
					glNormal3fv(normal.v);
					glBegin(GL_QUADS);
					glVertex3fv(face[3].v);
					glVertex3fv(face[2].v);
					glVertex3fv(face[1].v);
					glVertex3fv(face[0].v);
					glEnd();
				}
			}
			//�㔼��
			for (int si = meshNum/2+1; si <= meshNum; ++si)
			{
				for (int ci = 1; ci <= meshNum + 1; ++ci)
				{
					const float rUp = m_radius*sin(dRad*static_cast<float>(si)*0.5f);
					const float rDown = m_radius*sin(dRad*static_cast<float>(si - 1)*0.5f);
					const float hUp = -m_radius*cos(dRad*static_cast<float>(si)*0.5f);
					const float hDown = -m_radius*cos(dRad*static_cast<float>(si - 1)*0.5f);

					//�E��
					const DsVec3f c0(rDown*cos(static_cast<float>(ci - 1)*dRad),
						hDown,
						rDown*sin(static_cast<float>(ci - 1)*dRad));
					//����
					const DsVec3f c1(rDown*cos(static_cast<float>(ci)*dRad),
						hDown,
						rDown*sin(static_cast<float>(ci)*dRad));
					//����
					const DsVec3f c2(rUp*cos(static_cast<float>(ci)*dRad),
						hUp,
						rUp*sin(static_cast<float>(ci)*dRad));
					//�E��
					const DsVec3f c3(rUp*cos(static_cast<float>(ci - 1)*dRad),
						hUp,
						rUp*sin(static_cast<float>(ci - 1)*dRad));
					
					const DsVec3f face[4] =
					{
						(rot*c0) + m_p2,
						(rot*c1) + m_p2,
						(rot*c2) + m_p2,
						(rot*c3) + m_p2,
					};

					const DsVec3f normal = DsVec3f::CalcNormal(face[0], face[1], face[2]);
					glNormal3fv(normal.v);
					glBegin(GL_QUADS);
					glVertex3fv(face[3].v);
					glVertex3fv(face[2].v);
					glVertex3fv(face[1].v);
					glVertex3fv(face[0].v);
					glEnd();
				}
			}
			
			//����
			const float halfHeight = dir.Length()*0.5f;
			const DsVec3f center = (m_p1 + m_p2)*0.5f;
			for (int i = 1; i <= meshNum; ++i)
			{
				//�E��
				const DsVec3f c0(m_radius*cos(static_cast<float>(i)*dRad),
					-halfHeight,
					m_radius*sin(static_cast<float>(i)*dRad));

				//����
				const DsVec3f c1(m_radius*cos(static_cast<float>(i-1)*dRad),
					-halfHeight,
					m_radius*sin(static_cast<float>(i-1)*dRad));

				//����
				const DsVec3f c2(m_radius*cos(static_cast<float>(i - 1)*dRad),
					halfHeight,
					m_radius*sin(static_cast<float>(i - 1)*dRad));

				//�E��
				const DsVec3f c3(m_radius*cos(static_cast<float>(i)*dRad),
					halfHeight,
					m_radius*sin(static_cast<float>(i)*dRad));

				const DsVec3f face[4] =
				{
					(rot*c0) + center,
					(rot*c1) + center,
					(rot*c2) + center,
					(rot*c3) + center,
				};

				const DsVec3f normal = DsVec3f::CalcNormal(face[0], face[1], face[2]);
				glNormal3fv(normal.v);
				glBegin(GL_QUADS);
				glVertex3fv(face[0].v);
				glVertex3fv(face[1].v);
				glVertex3fv(face[2].v);
				glVertex3fv(face[3].v);
				glEnd();
			}

		}

	private:
		const DsVec3f m_p1;
		const DsVec3f m_p2;
		const float m_radius;
	};
	
	/****************************
	@brief �|���S��
	*****************************/
	class _ComPolygon : public DsDrawComBase
	{
	public:
		_ComPolygon( const DsVec3f& p1, const DsVec3f& p2, const DsVec3f& p3 )
		:m_p1(p1)
		,m_p2(p2)
		,m_p3(p3)
		{}
	virtual void Exe(DsDrawCommand& owner) override
	{
		const DsVec3f normal = DsVec3f::CalcNormal(m_p1, m_p2, m_p3);
		glNormal3fv(normal.v);
		glBegin( GL_TRIANGLE_FAN );
		glVertex3fv(m_p1.v);
		glVertex3fv(m_p2.v);
		glVertex3fv(m_p3.v);
		glEnd();

	}

	private:
		const DsVec3f m_p1;
		const DsVec3f m_p2;
		const DsVec3f m_p3;
	};

	//N�p�`�|���S��
	class _ComPolygonNf : public DsDrawComBase
	{
	public:
		_ComPolygonNf(const DsVec3f* p, const int pn)
			: m_p()
			, m_pn(pn)
		{
			DS_ASSERT(pn <= 32, "���p�`�̕`��R�}���h�Ńo�b�t�@�C�T�C�Y�I�[�o�[");
			for (int i = 0; i < pn; ++i)
			{
				m_p[i] = p[i];
			}
		}
		

		virtual void Exe(DsDrawCommand& owner) override
		{
			DS_ASSERT(m_pn >= 3, "���p�`�|���S���̒��_�����R����");

			const DsVec3f normal = DsVec3f::CalcNormal(m_p[0], m_p[1], m_p[2]);
			glNormal3fv(normal.v);
			glBegin(GL_POLYGON);
			for (int i = 0; i < m_pn; ++i)
			{
				glVertex3fv(m_p[i].v);
			}
			glEnd();
		}

	private:
		DsVec3f m_p[32];
		const int m_pn;
	};
	class _ComPolygonNd : public DsDrawComBase
	{
	public:
		_ComPolygonNd(const DsVec3d* p, const int pn)
			: m_p()
			, m_pn(pn)
		{
			DS_ASSERT(pn <= 32, "���p�`�̕`��R�}���h�Ńo�b�t�@�C�T�C�Y�I�[�o�[");
			for (int i = 0; i < pn; ++i)
			{
				m_p[i] = ToVec3f(p[i]);
			}
		}
		virtual void Exe(DsDrawCommand& owner) override
		{
			DS_ASSERT(m_pn >= 3, "���p�`�|���S���̒��_�����R����");

			const DsVec3f normal = DsVec3f::CalcNormal(m_p[0], m_p[1], m_p[2]);
			glNormal3fv(normal.v);
			glBegin(GL_TRIANGLE_FAN);
			for (int i = 0; i < m_pn; ++i)
			{
				glVertex3fv(m_p[i].v);
			}
			glEnd();
		}

	private:
		DsVec3f m_p[32];
		const int m_pn;
	};

	/****************************
	@brief �S�p�|���S��
	*****************************/
	class _ComQuad : public DsDrawComBase
	{
	public:
		_ComQuad(const DsVec3f& p1, const DsVec3f& p2, const DsVec3f& p3, const DsVec3f& p4)
			: m_p1(p1)
			, m_p2(p2)
			, m_p3(p3)
			, m_p4(p4)
		{}
		virtual void Exe(DsDrawCommand& owner) override
		{
			const DsVec3f normal = DsVec3f::CalcNormal(m_p1, m_p2, m_p3);
			glNormal3fv(normal.v);
			glBegin(GL_QUADS);
			glVertex3fv(m_p1.v);
			glVertex3fv(m_p2.v);
			glVertex3fv(m_p3.v);
			glVertex3fv(m_p4.v);
			glEnd();
		}

	private:
		const DsVec3f m_p1;
		const DsVec3f m_p2;
		const DsVec3f m_p3;
		const DsVec3f m_p4;
	};

	/****************************
	@brief �e�N�X�`��
	*****************************/
	class _ComTexQuad : public DsDrawComBase
	{
	public:
		_ComTexQuad(const DsVec3d& p1, const DsVec3d& p2, const DsVec3d& p3, const DsVec3d& p4
			, const DsVec2d& t1, const DsVec2d& t2, const DsVec2d& t3, const DsVec2d& t4
			, const unsigned char* pImage, int w, int h,
			_InnerCommandInfo& innerInfo)
			: m_p1(p1)
			, m_p2(p2)
			, m_p3(p3)
			, m_p4(p4)
			, m_t1(t1)
			, m_t2(t2)
			, m_t3(t3)
			, m_t4(t4)
			, m_pImage(pImage)
			, m_w(w)
			, m_h(h)
			, m_innerInfo(innerInfo)
		{}
		virtual void Exe(DsDrawCommand& owner) override
		{
			GLuint texId;
			auto itr = m_innerInfo.pTexIdMap->find(m_pImage);
			if (itr == m_innerInfo.pTexIdMap->end()) {
				//������Ȃ������̂Ńe�N�X�`���o�^
				glGenTextures(1, &texId);
				(*m_innerInfo.pTexIdMap)[m_pImage] = texId;
			}
			else {
				//���������̂�ID���擾
				texId = itr->second;
			}

			owner.RefRender().SetEnableTexture(true);
			glBindTexture(GL_TEXTURE_2D, texId);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);//�e�N�X�N�`���J��Ԃ�
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);//�e�N�X�N�`���J��Ԃ�
			glTexImage2D(
				GL_TEXTURE_2D, 0, GL_RGB, m_w, m_h,
				0, GL_RGB, GL_UNSIGNED_BYTE, m_pImage);
			
			//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);//���n�𓧖��ɂ���̂ɕK�v
			//glEnable(GL_BLEND);
			//glEnable(GL_TEXTURE_2D);//SetEnableTexture�ŃV�F�[�_�[�Ő؂�ւ��Ă���̂ł���Ȃ�

			const DsVec3d normal = DsVec3d::CalcNormal(m_p1, m_p2, m_p3);
			glColor3d(1, 1, 1);
			glNormal3dv(normal.v);
			glBegin(GL_QUADS);
			glTexCoord2dv(m_t1.v);  glVertex3dv(m_p1.v);
			glTexCoord2dv(m_t2.v);  glVertex3dv(m_p2.v);
			glTexCoord2dv(m_t3.v);  glVertex3dv(m_p3.v);
			glTexCoord2dv(m_t4.v);  glVertex3dv(m_p4.v);
			glEnd();

			glBindTexture(GL_TEXTURE_2D, 0);
			owner.RefRender().SetEnableTexture(false);
			//glDisable(GL_TEXTURE_2D);//SetEnableTexture�ŃV�F�[�_�[�Ő؂�ւ��Ă���̂ł���Ȃ�
			//glDisable(GL_BLEND);
		}

	private:
		const DsVec3d m_p1;
		const DsVec3d m_p2;
		const DsVec3d m_p3;
		const DsVec3d m_p4;
		const DsVec2d m_t1;
		const DsVec2d m_t2;
		const DsVec2d m_t3;
		const DsVec2d m_t4;
		const unsigned char* m_pImage;
		const int m_w;
		const int m_h;
		_InnerCommandInfo m_innerInfo;
	};

	/****************************
	@brief ��
	*****************************/
	class _ComAxis : public DsDrawComBase
	{
	public:
		_ComAxis(const DsMat33f& m, const DsVec3f& pos, const float len, const float width)
			: m_m(m)
			, m_pos(pos)
			, m_len(len)
			, m_width(width)
		{}
		virtual void Exe(DsDrawCommand& owner) override
		{
			float r[] = { 1, 0, 0, 1 };
			float g[] = { 0, 1, 0, 1 };
			float b[] = { 0, 0, 1, 1 };

			float srcWidth;
			glGetFloatv(GL_LINE_WIDTH, &srcWidth);
			glLineWidth(m_width);

			glMaterialfv(GL_FRONT, GL_DIFFUSE, r);
			glColor3fv(r);
			glNormal3fv(m_m.GetAxisX().v);
			glBegin(GL_LINES);
			glVertex3fv(m_pos.v);
			glVertex3fv((m_pos + m_m.GetAxisX()*m_len).v);
			glEnd();

			glMaterialfv(GL_FRONT, GL_DIFFUSE, g);
			glColor3fv(g);
			glNormal3fv(m_m.GetAxisY().v);
			glBegin(GL_LINES);
			glVertex3fv(m_pos.v);
			glVertex3fv((m_pos + m_m.GetAxisY()*m_len).v);
			glEnd();

			glMaterialfv(GL_FRONT, GL_DIFFUSE, b);
			glColor3fv(b);
			glNormal3fv(m_m.GetAxisZ().v);
			glBegin(GL_LINES);
			glVertex3fv(m_pos.v);
			glVertex3fv((m_pos + m_m.GetAxisZ()*m_len).v);
			glEnd();

			glLineWidth(srcWidth);
		}

	private:
		const DsMat33f m_m;
		const DsVec3f m_pos;
		const float m_len;
		const float m_width;
	};
	
	/****************************
	@brief �J���[
	*****************************/
	class _ComColor : public DsDrawComBase
	{
	public:
		_ComColor( const DsVec3f& color )
		:m_color(color)
		{}
	virtual void Exe(DsDrawCommand& owner) override
	{
		const GLfloat col[] = { m_color.x, m_color.y, m_color.z, 1.0f };
		const GLfloat spec[] = { 0.4f, 0.4f, 0.4f, 1.0f };	// ���ʔ��ːF
		const GLfloat ambi[] = { 0.1f, 0.1f, 0.1f, 1.0f };	// ����
		glMaterialfv(GL_FRONT, GL_SPECULAR, spec);
		glMaterialfv(GL_FRONT, GL_AMBIENT, ambi);
		glMaterialf(GL_FRONT, GL_SHININESS, 30);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, col);
		glColor3fv(col);
	}

	private:
		const DsVec3f m_color;
	};

	/****************************
	@brief ����
	*****************************/
	class _ComText : public DsDrawComBase
	{
	private:
		enum 
		{
			FONT_W_MAX = 512,
			FONT_H_MAX = 64,
		};

	public:
		_ComText(const DsVec3d& pos, const wchar_t* pText, _InnerCommandInfo& innerInfo)
			: m_pos(pos)
			, m_text(pText)
			, m_innerInfo(innerInfo)
		{
		}
		virtual void Exe(DsDrawCommand& owner) override
		{
			char buf[FONT_W_MAX * FONT_H_MAX];

			HDC hDC = wglGetCurrentDC();
			HFONT hFont = m_innerInfo.hFont;

			//�Â��t�H���g���o�b�N�A�b�v
			HFONT  hFont_old = (HFONT)SelectObject(hDC, hFont);

			//�����̃T�C�Y���擾
			SIZE sizeText;
			{
				GetTextExtentPoint32(hDC, m_text.c_str(), static_cast<int>(m_text.length()), &sizeText);
				sizeText.cx = min(sizeText.cx, FONT_W_MAX);//�o�b�t�@�z���Ȃ��悤��
				sizeText.cy = min(sizeText.cy, FONT_H_MAX);
			}

			//�����T�C�Y����bitmap�p��
			BITMAP bitmap;
			HBITMAP hBitmap = NULL;
			{
				FillMemory(&bitmap, sizeof(bitmap), 0);
				bitmap.bmWidth = sizeText.cx;
				bitmap.bmHeight = sizeText.cy;
				bitmap.bmWidthBytes = ((sizeText.cx + 7) / 8 + 1) & (~1);
				bitmap.bmPlanes = 1;
				bitmap.bmBitsPixel = 1;
				bitmap.bmBits = buf;//�T�C�Y=bitmap.bmWidthBytes * lHeight
				hBitmap = CreateBitmapIndirect(&bitmap);
				DS_ASSERT(hBitmap, "�t�H���g�pbitmap�������s");
			}

			//��������ɕ������o�͂��邽�߂̐ݒ�
			HDC hMemDC = CreateCompatibleDC(hDC);
			HBITMAP hBitmap_memdc_old = (HBITMAP)SelectObject(hMemDC, hBitmap);//�Â��̃o�b�N�A�b�v
			HFONT hFont_memdc_old = (HFONT)SelectObject(hMemDC, hFont);
			{
				DS_ASSERT(hMemDC, "�t�H���g�p�������f�o�C�X�R���e�L�X�g�쐬���s");
				SetTextColor(hMemDC, RGB(255, 255, 255));
				SetBkColor(hMemDC, RGB(0, 0, 0));
				SetBkMode(hMemDC, OPAQUE);
				// MemDC��Ƀe�L�X�g���o��
				TextOut(hMemDC, 0, 0, m_text.c_str(), static_cast<int>(m_text.length()));
			}

			// MonoBits�f�[�^�̍쐬�A�\�z
			GLubyte* pStringMonoBits = NULL;
			SIZE rsizeBitmap;
			{
				rsizeBitmap.cx = ((bitmap.bmWidth + 31) & (~31));	// 32�̔{���ɂ���
				rsizeBitmap.cy = bitmap.bmHeight;
				// �r�b�g��̗̈�̊m��
				const int uiBitsSize = rsizeBitmap.cy * (rsizeBitmap.cx / 8);
				GLubyte* pMonoBits = (GLubyte*)buf;//�T�C�Y=uiBitsSize ;
				FillMemory(pMonoBits, uiBitsSize, 0);
				// �r�b�g�}�b�v���r�b�g��ɃR�s�[
				struct
				{
					BITMAPINFOHEADER	bmiHeader;
					RGBQUAD		bmiColors[2];	// �p���b�g�i�F�̐�(��2^biBitCount)����RGBQUAD��p�ӂ���K�v������j
				}monobitmapinfo;
				BITMAPINFO* pBitmapinfo = (BITMAPINFO*)&monobitmapinfo;
				pBitmapinfo->bmiHeader.biSize = sizeof(pBitmapinfo->bmiHeader);
				pBitmapinfo->bmiHeader.biWidth = bitmap.bmWidth;
				pBitmapinfo->bmiHeader.biHeight = bitmap.bmHeight;
				pBitmapinfo->bmiHeader.biPlanes = 1;
				pBitmapinfo->bmiHeader.biBitCount = 1;	// 1�s�N�Z��������̃r�b�g���i1�s�N�Z���������1�r�b�g���Q�F�r�b�g�}�b�v�j
				pBitmapinfo->bmiHeader.biCompression = BI_RGB;
				pBitmapinfo->bmiHeader.biSizeImage = uiBitsSize;
				pBitmapinfo->bmiHeader.biXPelsPerMeter = 1;
				pBitmapinfo->bmiHeader.biYPelsPerMeter = 1;
				pBitmapinfo->bmiHeader.biClrUsed = 0;
				pBitmapinfo->bmiHeader.biClrImportant = 0;
				GetDIBits(hDC, hBitmap, 0, bitmap.bmHeight, pMonoBits, pBitmapinfo, DIB_RGB_COLORS);
				pStringMonoBits = pMonoBits;
			}

			//�Â��̕��A
			SelectObject(hMemDC, hFont_memdc_old);
			SelectObject(hMemDC, hBitmap_memdc_old);

			//�������f�o�C�X�R���e�L�X�g�폜
			DeleteDC(hMemDC);

			//�t�H���g�pbitmap�폜
			DeleteObject(hBitmap);

			//�Â��t�H���g���A
			SelectObject(hDC, hFont_old);


			//GL�ŕ`��BpStringMonoBits��rsizeBitmap���������OK
			{
				glPushMatrix();
				glLoadIdentity();
				glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
				glRasterPos3dv(m_pos.v);
				glBitmap(rsizeBitmap.cx, rsizeBitmap.cy,
					0.0,		// �r�b�g�}�b�v���_���ǂ��ɂ��邩�i�r�b�g�}�b�v�̍�����B���́A�w���͉E�����A�x���͏�����j
					0.0,		// �r�b�g�}�b�v���_���ǂ��ɂ��邩�i�r�b�g�}�b�v�̍�����B���́A�w���͉E�����A�x���͏�����j
					0.0,
					0.0,
					pStringMonoBits);
				glPopMatrix();
			}

		}

	private:
		const DsVec3d m_pos;
		std::wstring m_text;
		_InnerCommandInfo m_innerInfo;
	};
}

//static
DsDrawCommand& DsDrawCommand::Create(DsAnimModelRender& animRender, DsRender& render)
{
	DsDrawCommand* ret = new DsDrawCommand(animRender, render);
	DS_ASSERT(ret, "�������m�ێ��s");
	return *ret;
}

DsDrawCommand::DsDrawCommand(DsAnimModelRender& animRender, DsRender& render)
: m_coms()
, m_pBuffer(NULL)
, m_useMemory(0)
, m_animRender(animRender)
, m_render(render)
, m_dummyTexId(0)
, m_texIdMap()
, m_hFont(0)
{
	m_coms.clear();
	m_pBuffer = new char[BUFFER_SIZE];
	if(m_pBuffer)
	{
		m_useMemory=0;
	}else
	{
		//�m�ۂł��Ȃ�������S�Ďg�p�ς݂ɂ���
		m_useMemory=BUFFER_SIZE;
	}

	//�_�~�[�̐^�����e�N�X�`��
	//unsigned char dummyTex[4][3] =
	//{
	//	{ 255, 255, 255 },{ 255, 255, 255 },
	//	{ 255, 255, 255 },{ 255, 255, 255 },
	//};
	//
	//GLuint texName;
	//glGenTextures(1, &texName);
	//glBindTexture(GL_TEXTURE_2D, texName);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);//�e�N�X�N�`���J��Ԃ�
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);//�e�N�X�N�`���J��Ԃ�
	//glTexImage2D(
	//	GL_TEXTURE_2D, 0, GL_RGB, 2, 2,
	//	0, GL_RGB, GL_UNSIGNED_BYTE, dummyTex);
	//glBindTexture(GL_TEXTURE_2D, 0);
	//m_dummyTexId = texName;

	HDC hDC = wglGetCurrentDC();


	LOGFONT logfont;
	FillMemory(&logfont, sizeof(LOGFONT), 0);
	logfont.lfHeight = -MulDiv((int)24, GetDeviceCaps(hDC, LOGPIXELSY), 72);	// �����̍���
	logfont.lfCharSet = SHIFTJIS_CHARSET;	// �L�����N�^�Z�b�g�i���{��L�����N�^�Z�b�g�j
	logfont.lfWeight = FW_NORMAL;		// �����i�W���̑����j
	wcscpy_s(logfont.lfFaceName, LF_FACESIZE, L"�l�r �S�V�b�N");
	HFONT hFont = CreateFontIndirect(&logfont);
	DS_ASSERT(hFont, "�t�H���g�̏������Ɏ��s");
	m_hFont = (ds_uint64)hFont;

}

DsDrawCommand::~DsDrawCommand()
{
	delete m_pBuffer;m_pBuffer=NULL;

	for (const auto& texId : m_texIdMap) {
		glDeleteTextures(1, &texId.second);
	}

	DeleteObject((HFONT)m_hFont);
}

void DsDrawCommand::Exe()
{
	for each(DsDrawComBase* com in m_coms)
	{
		com->Exe(*this);
	}
}

void DsDrawCommand::Clear()
{
	m_coms.clear();
	m_useMemory = 0;
}

DsDrawCommand& DsDrawCommand::DrawBox(const DsVec3f& pos, const float size)
{
	if (BUFFER_SIZE > (m_useMemory + sizeof(_ComBox)))
	{
		DsDrawComBase* com = new(m_pBuffer + m_useMemory) _ComBox(pos, size);
		if (com)
		{
			m_coms.push_back(com);
			m_useMemory += sizeof(_ComBox);
		}
	}
	return (*this);
}

DsDrawCommand& DsDrawCommand::DrawSphere(const DsVec3f& pos, const float radius)
{
	if (BUFFER_SIZE > (m_useMemory + sizeof(_ComSphere)))
	{
		DsDrawComBase* com = new(m_pBuffer + m_useMemory) _ComSphere(pos, radius);
		if (com)
		{
			m_coms.push_back(com);
			m_useMemory += sizeof(_ComSphere);
		}
	}
	return (*this);
}


DsDrawCommand& DsDrawCommand::DrawLine(const DsVec3f& p1, const DsVec3f& p2)
{
	if( BUFFER_SIZE > (m_useMemory+sizeof(_ComLine)) )
	{
		DsDrawComBase* com = new(m_pBuffer+m_useMemory) _ComLine(p1,p2);
		if( com )
		{
			m_coms.push_back( com );
			m_useMemory += sizeof(_ComLine);
		}
	}
	return (*this);
}

DsDrawCommand& DsDrawCommand::DrawCapsule(const DsVec3f& p1, const DsVec3f& p2, const float radius)
{
	if (BUFFER_SIZE > (m_useMemory + sizeof(_ComCapsule)))
	{
		DsDrawComBase* com = new(m_pBuffer + m_useMemory) _ComCapsule(p1, p2, radius);
		if (com)
		{
			m_coms.push_back(com);
			m_useMemory += sizeof(_ComCapsule);
		}
	}
	return (*this);
}

DsDrawCommand& DsDrawCommand::DrawPolygon(const DsVec3f& p1, const DsVec3f& p2, const DsVec3f& p3)
{
	if( BUFFER_SIZE > (m_useMemory+sizeof(_ComPolygon)) )
	{
		DsDrawComBase* com = new(m_pBuffer+m_useMemory) _ComPolygon(p1,p2, p3);
		if( com )
		{
			m_coms.push_back( com );
			m_useMemory += sizeof(_ComPolygon);
		}
	}
	return (*this);
}

DsDrawCommand& DsDrawCommand::DrawPolygon(const DsVec3f* p, const int pn)
{
	if (BUFFER_SIZE > (m_useMemory + sizeof(_ComPolygonNf)))
	{
		DsDrawComBase* com = new(m_pBuffer + m_useMemory) _ComPolygonNf(p, pn);
		if (com)
		{
			m_coms.push_back(com);
			m_useMemory += sizeof(_ComPolygonNf);
		}
	}
	return (*this);
}
DsDrawCommand& DsDrawCommand::DrawPolygon(const DsVec3d* p, const int pn)
{
	if (BUFFER_SIZE > (m_useMemory + sizeof(_ComPolygonNd)))
	{
		DsDrawComBase* com = new(m_pBuffer + m_useMemory) _ComPolygonNd(p, pn);
		if (com)
		{
			m_coms.push_back(com);
			m_useMemory += sizeof(_ComPolygonNd);
		}
	}
	return (*this);
}


DsDrawCommand& DsDrawCommand::DrawQuad(const DsVec3f& p1, const DsVec3f& p2, const DsVec3f& p3, const DsVec3f& p4)
{
	if (BUFFER_SIZE > (m_useMemory + sizeof(_ComQuad)))
	{
		DsDrawComBase* com = new(m_pBuffer + m_useMemory) _ComQuad(p1, p2, p3, p4);
		if (com)
		{
			m_coms.push_back(com);
			m_useMemory += sizeof(_ComQuad);
		}
	}
	return (*this);
}

DsDrawCommand& DsDrawCommand::DrawTexQuad(const DsVec3d& p1, const DsVec3d& p2, const DsVec3d& p3, const DsVec3d& p4,
	const DsVec2d& t1, const DsVec2d& t2, const DsVec2d& t3, const DsVec2d& t4,
	const unsigned char* pImage, int w, int h)
{
	if (BUFFER_SIZE > (m_useMemory + sizeof(_ComTexQuad)))
	{
		_InnerCommandInfo innerInfo;
		innerInfo.pTexIdMap = &m_texIdMap;
		DsDrawComBase* com = new(m_pBuffer + m_useMemory) _ComTexQuad(p1, p2, p3, p4, t1, t2, t3, t4, pImage, w, h, innerInfo);
		if (com)
		{
			m_coms.push_back(com);
			m_useMemory += sizeof(_ComTexQuad);
		}
	}
	return (*this);
}

DsDrawCommand& DsDrawCommand::DrawAxis(const DsMat33f& m, const DsVec3f& pos, const float len, const float width)
{
	if (BUFFER_SIZE > (m_useMemory + sizeof(_ComAxis)))
	{
		DsDrawComBase* com = new(m_pBuffer + m_useMemory) _ComAxis(m, pos, len, width);
		if (com)
		{
			m_coms.push_back(com);
			m_useMemory += sizeof(_ComAxis);
		}
	}
	return (*this);
}

DsDrawCommand& DsDrawCommand::SetColor(const DsVec3f& color)
{
	if( BUFFER_SIZE > (m_useMemory+sizeof(_ComColor)) )
	{
		DsDrawComBase* com = new(m_pBuffer+m_useMemory) _ComColor(color);
		if( com )
		{
			m_coms.push_back( com );
			m_useMemory += sizeof(_ComColor);
		}
	}
	return (*this);
}


DsDrawCommand& DsDrawCommand::DrawText(const DsVec3d& pos, const wchar_t* pText, ...)
{
	wchar_t buf[1024];
	va_list  valist;
	va_start(valist, pText);
	//swprintf_s(buf, 1024, pText, valist);
	vswprintf_s(buf, 1024, pText, valist);
	va_end(valist);

	if (BUFFER_SIZE > (m_useMemory + sizeof(_ComText)))
	{
		_InnerCommandInfo innnerInfo;
		innnerInfo.hFont = (HFONT)m_hFont;
		DsDrawComBase* com = new(m_pBuffer + m_useMemory) _ComText(pos, buf, innnerInfo);
		if (com)
		{
			m_coms.push_back(com);
			m_useMemory += sizeof(_ComText);
		}
	}
	return (*this);
}

DsDrawCommand& DsDrawCommand::DrawTextScreen(const DsVec2d& pos, const wchar_t* pText, ...)
{
	wchar_t buf[1024];
	va_list  valist;
	va_start(valist, pText);
	//swprintf_s(buf, 1024, pText, valist);
	vswprintf_s(buf, 1024, pText, valist);
	va_end(valist);

	if (BUFFER_SIZE > (m_useMemory + sizeof(_ComText)))
	{
		_InnerCommandInfo innnerInfo;
		innnerInfo.hFont = (HFONT)m_hFont;
		DsVec3d drawPos = DsVec3d(pos.x*m_render.RefCam().GetAspect() , pos.y, -(m_render.RefCam().GetNear()+0.00001));
		DsDrawComBase* com = new(m_pBuffer + m_useMemory) _ComText(drawPos, buf, innnerInfo);
		if (com)
		{
			m_coms.push_back(com);
			m_useMemory += sizeof(_ComText);
		}
	}
	return (*this);
}