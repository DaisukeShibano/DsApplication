#include "DsPch.h"

#include "Config/DsConf.h"
#include <fstream>

using namespace DsLib;
using namespace std;

static string s_keys[] = 
{
	"VertexShaderPath",
	"FragmetShaderPath",
};

DsConf::DsConf()
:m_conf()
{
}

DsConf::~DsConf()
{
}

void DsConf::Initialize( const char* path )
{
	DsFile ifs(path);
    if (ifs.IsFail())
    {
		printf("DsLibê›íËÉtÉ@ÉCÉãì«Ç›çûÇ›é∏îs %s\n", path);
        return;
    }
	
	for ( string str; ifs.GetLine(str); )
	{
		auto pos = str.find(" ");
		if( string::npos != pos )
		{
			string key = str.substr(0, pos );
			string val = str.substr(pos+1, string::npos);
			m_conf[key] = val;
		}
	}
}

const char* DsConf::Str( DsConfKey key )
{
	if( m_conf.find(s_keys[key]) != m_conf.end() )
	{
		return m_conf[s_keys[key]].c_str();
	}
	else
	{
		return NULL;
	}
}

int	DsConf::Int( DsConfKey key )
{
	if( m_conf.find(s_keys[key]) != m_conf.end() )
	{
		return std::stoi(m_conf[s_keys[key]]);
	}
	else
	{
		return 0;
	}
}
