#ifndef _DS_KEYBOARD_H_
#define _DS_KEYBOARD_H_

namespace DsLib
{
	class DsKeyboard
	{
	public:
		enum KEY
		{
			FORWARD,
			BACK,
			LEFT,
			RIGHT,
			KEY_NUM,
		};
		

	public:
		DsKeyboard();
		virtual ~DsKeyboard();

	public:
		void OnKey(int key, bool push);

	public:
		bool IsPush(KEY key) const { return m_keyPush[key]; }

	private:
		bool m_keyPush[KEY_NUM];
	};
}

#endif