#include "DsPch.h"

#ifndef _DS_ANIM_SET_H_
#include "Animation/Res/DsAnimSet.h"
#endif

#ifndef _DS_ANIM_RES_
#include "Animation/Res/DsAnimRes.h"
#endif

using namespace DsLib;
using namespace std;

DsAnimSet::DsAnimSet()
	:m_res()
{

}

DsAnimSet::~DsAnimSet()
{
	for each(DsAnimRes* res in m_res)
	{
		delete res;
	}
	m_res.clear();
}

void DsAnimSet::Initialize(const InitInfo& initInfo)
{
	if (NULL == initInfo.dataPath) return;

	vector<string> loadList;
	ifstream ifs;
	ifs.open(initInfo.dataPath, ios::in);
	if (ifs.fail())
	{
		return;
	}

	while (!ifs.eof())
	{
		string str;
		getline(ifs, str);
		loadList.push_back(str);
	}
	ifs.close();


	for each(string path in loadList)
	{
		DsAnimRes* pRes = new DsAnimRes();
		pRes->Initialize(path.c_str());
		m_res.push_back(pRes);
	}

	//ƒtƒ@ƒCƒ‹–¼‚Ì”Žš‚ªŽá‚¢‡‚É•À‚Ô‚æ‚¤‚É‚·‚é 
	class Cmp {
	public:
		bool operator()(const DsAnimRes* riLeft, const DsAnimRes* riRight) const 
		{
			regex reg("[^0-9]");
			
			string cmp1 = riLeft->RefName();
			regex_replace(cmp1, reg, "");
			const int val1 = atoi(cmp1.c_str());

			string cmp2 = riRight->RefName();
			regex_replace(cmp2, reg, "");
			const int val2 = atoi(cmp2.c_str());

			return val1 < val2;
		}
	};
	sort(m_res.begin(), m_res.end(), Cmp());
}

const DsAnimRes* DsAnimSet::GetAnimRes(const char* name) const
{
	for each(DsAnimRes* res in m_res)
	{
		if (res->RefName() == name)
		{
			return res;
		}
	}
	return NULL;
}