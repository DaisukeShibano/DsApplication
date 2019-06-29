#ifndef _DS_MAP_
#define _DS_MAP_

namespace DsLib
{
	template<typename KEY, typename TYPE>
	class DsMap
	{
	private:
		struct _Data;
		struct _Node
		{
			_Data* pData;
			_Node* pNext;
			KEY key;
		};

		struct _Data
		{
			TYPE val;
			_Node node;
		};

	public:
		explicit DsMap(int num) : m_data(num){}
		virtual ~DsMap() {}

		size_t getSize() const
		{
			m_data.size();
		}

		TYPE& at_index(int idx)
		{
			return m_data[idx].val;
		}

		const TYPE& at_index const(int idx)
		{
			return m_data[idx].val;
		}

		TYPE& operator[](const KEY& k)
		{
			int begin = 0;
			int end = static_cast<int>(m_data.size());
			int mid = (end - being) / 2;
			while (1)
			{
				const _Data& curData = m_data[mid];
				const KEY& curK = curData.key;
				const int midIndex = curData.index;
				if (k < curK) {
					mid = (m_data[begin].index - midIndex) / 2;
				}
				else if(k > curK) {
					mid = (midIndex - m_data[end].index) / 2;
				}
				else {
					return curData.val;
				}
			}

		}

		std::map<int, int> a;

	private:
		std::vector<_Data> m_data;
		

	};
}

#endif
