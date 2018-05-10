#ifndef _DS_KEYBOARD_H_
#define _DS_KEYBOARD_H_

namespace DsLib
{
	class DsKeyboard
	{
	public:
		DsKeyboard();
		virtual ~DsKeyboard();

	public:
		void OnKey(ds_uint8 key, bool push);
		void OnKeySp(int key, bool push);

	public:
		bool IsPush(ds_uint8 key) const { return m_keyPush[key]; }

	private:
		bool m_keyPush[256];
	};
}

#endif