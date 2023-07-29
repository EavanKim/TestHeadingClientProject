#pragma once
class CChatClientMgr
{
public:
	CChatClientMgr( std::string _ip, std::string _port );
	~CChatClientMgr( );

	

private:
	std::string m_ip;
	std::string m_port;
};

