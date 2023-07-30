#include "psudoPCH.h"

CChatClientSession::CChatClientSession( ChatConnectionInfo& _info )
	: Heading::CClientSession( _info.sock )
{
	if( SOCKET_ERROR ==  connect( m_sock, _info.info->ai_addr, _info.info->ai_addrlen ) )
	{
		closesocket( m_sock );
		return;
	}

	Heading::PCK_CS_Enter* enter = new Heading::PCK_CS_Enter();

	memcpy_s(enter->buffer, 12, _info.nickName.c_str(), 12 );

	m_sendBuff.push_back(enter);

	SendData();
}

CChatClientSession::CChatClientSession( SOCKET _sock )
	: Heading::CClientSession( _sock )
{

}

CChatClientSession::~CChatClientSession( )
{

}

void CChatClientSession::Update( )
{
	//RecvData();
	SendData();
}
