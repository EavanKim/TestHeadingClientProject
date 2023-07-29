#pragma once

struct ChatConnectionInfo : Heading::connectionInfo
{
	std::string nickName;
};

class CChatClientSession : public Heading::CClientSession
{
public:
	CChatClientSession( ChatConnectionInfo& info );
	CChatClientSession( SOCKET _sock );
	virtual ~CChatClientSession();

	// Inherited via CClientSession
	virtual void Update( ) override;

private:
	void ( *onPing )() = nullptr;
};

