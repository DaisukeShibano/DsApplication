#ifndef _DS_CONF_H_
#define _DS_CONF_H_


namespace DsLib
{
	enum DsConfKey
	{
		CONF_VX_SHADER_PATH = 0,
		CONF_FR_SHADER_PATH,
	
		CONF_KEY_NUM
	};

	class DsConf
	{
	public:
		DsConf();
		virtual ~DsConf();

	public:
		static DsConf& GetIns()
		{
			static DsConf ret;
			return ret;
		}

	public:
		void Initialize( const char* path );

	public:
		const char* Str( DsConfKey key );
		int	Int( DsConfKey key ); 

	private:
		std::map<std::string, std::string> m_conf;
	};

}


#endif