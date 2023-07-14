﻿#include "psudoPCH.h"
#include <conio.h>

int main()
{
	WSAData Data = {};
	Heading::start( Data );

	try
	{
		bool IsClientLive = true;

		Heading::connectionInfo mainConn;
		mainConn.ip = std::string( "127.0.0.1" );
		mainConn.port = std::string( "50000" );

		if( !Heading::createInfo( mainConn ) )
		{
			return 1;
		}

		Heading::connect( mainConn );

		while( IsClientLive )
		{
			char buffer[ DEFAULT_SOCKET_BUFFER_LENGTH ];
			scanf_s(buffer);

			Heading::ChatBuffer* chatBuffer = new Heading::ChatBuffer( );
			memcpy_s( chatBuffer->buffer, 1000, buffer, 1000 );
			mainConn.sendBuff.push_back( chatBuffer );

			Heading::send( mainConn );
		}

		Heading::disconnect( mainConn );
	}
	catch( ... )
	{
		// 크래시하면 죽어버리고, 바로 죽는게 억지로 살리는 것 보단 낫겠지만... 적어도 뭔 일이 벌어졌는지는 볼 수 있도록 합니다.

		TCHAR* message = nullptr;
		FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER,
					   nullptr,
					   GetLastError(),
					   MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),
					   ( TCHAR* )&message,
					   0,
					   nullptr );

		wprintf( L" LastError String : %s", message );
		LocalFree( message );
	}

	Heading::end();
}