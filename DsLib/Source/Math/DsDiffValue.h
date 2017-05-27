#ifndef _DS_DIFF_VALUE_H_
#define _DS_DIFF_VALUE_H_

namespace DsLib
{
	template<typename TYPE>
	class DsDiffValue
	{
	private:
		enum SET_STATE
		{
			SET_STANDBY = 0,
			SET_END = 1,
			OLD_RESET = 2,
		};

	public:
		DsDiffValue()
			: m_value()
			, m_oldValue()
			, m_state(OLD_RESET)
		{}
		DsDiffValue(const TYPE& init )
			: m_value(init)
			, m_oldValue(init)
			, m_state(OLD_RESET)
		{}
		virtual ~DsDiffValue(){}

	public:
		void Set(const TYPE& val)
		{
			if (OLD_RESET == m_state)
			{
				//しばらくセットされなかったので差分が飛ばないように現在値セット
				m_oldValue = val;
				m_value = val;
			}
			else
			{
				m_oldValue = m_value;
				m_value = val;
			}
			m_state = SET_END;
		}

		void Update()
		{
			m_oldValue = m_value;
			m_state = (SET_END == m_state) ? SET_STANDBY : OLD_RESET;
		}

	public:
		const TYPE& Val() const { return m_value; }
		TYPE Diff() const{ return m_value - m_oldValue; }


	private:
		TYPE m_value;
		TYPE m_oldValue;
		char m_state;
	};
}

#endif