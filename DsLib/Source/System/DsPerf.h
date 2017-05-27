#ifndef _DS_PERF_H_
#define _DS_PERF_H_

#include <chrono>

//これを使ったスコープ内の処理までを計測するマクロ
#define DS_PERF_BEGIN_SCOPE() DsLib::DsPerfScope ____local_scope_pef_begin____;

//ログにパフォーマンス出力
#define DS_LOG_PEF() DS_LOG(" __Perf log [%.3f]ms (%.1f)%% ", DsLib::DsPerf::RefPerfDefault().GetMsec(), DsLib::DsPerf::RefPerfDefault().GetMsec()/0.16 );

namespace DsLib
{
}

namespace DsLib
{
	class DsPerf
	{
	public:
		static DsPerf& RefPerfDefault();
		static DsPerf& RefPerfTotalFps();

	private:
		DsPerf()
			: m_begin()
			, m_fps(0)
			, m_ms(0)
			, m_isDbgPrint(false)
		{};

	public:
		void Begin()
		{
			m_begin = std::chrono::system_clock::now();
			m_isDbgPrint = true;
		}
		
		void End()
		{
			auto end = std::chrono::system_clock::now();
			auto dur = end - m_begin;
			auto msec = std::chrono::duration_cast<std::chrono::milliseconds>(dur).count();
			m_ms = static_cast<double>(msec);
			if (0 < msec){
				m_fps = 1.0 / (m_ms* 0.001);
			}
		}
		
		void DbgPrintPerf()
		{
			if (m_isDbgPrint){
				DS_LOG_PEF();
				m_isDbgPrint=false;
			}
		}

		void DbgPrintFps()
		{
			if (m_isDbgPrint){
				DS_LOG(" ___FPS [%.1f]", m_fps);
			}
		}

		double GetMsec() { return m_ms; }
		double GetFps() { return m_fps; }

	private:
		std::chrono::system_clock::time_point m_begin;
		double m_fps;
		double m_ms;
		bool m_isDbgPrint;
	};

	class DsPerfScope
	{
	public:
		DsPerfScope(){
			DsPerf::RefPerfDefault().Begin();
		}
		~DsPerfScope(){
			DsPerf::RefPerfDefault().DsPerf::End();
		}
	};
}

#endif