#include "psudoPCH.h"
#include <conio.h>

int main()
{
	WSAData Data = {};
	Heading::start( Data );

	//===================================================================================================================================================================

	try
	{
		bool IsClientLive = true;
		//================================================================================================================================================================

		Heading::connectionInfo mainConn;
		Heading::connectionInfo broadCastConn;

		mainConn.ip = std::string( "127.0.0.1" );
		broadCastConn.ip = std::string( "127.0.0.1" );

		mainConn.port = std::string( "50000" );
		broadCastConn.port = std::string( "51000" );

		if( !Heading::createInfo( mainConn ) )
		{
			return 1;
		}

		if( !Heading::createInfo( broadCastConn ) )
		{
			return 1;
		}

		Heading::connect( mainConn );
		Heading::connect( broadCastConn );

		while( IsClientLive )
		{
			char buffer[ DEFAULT_SOCKET_BUFFER_LENGTH ];
			//std::streamsize size = {};
			//char* string = nullptr;
			//std::cin.getline( string, size );
			scanf_s(buffer);

			Heading::ChatBuffer* chatBuffer = new Heading::ChatBuffer( );
			memcpy_s( chatBuffer->buffer, 1000, buffer, 1000 );
			mainConn.sendBuff.push_back( chatBuffer );

			Heading::send( mainConn );
			Heading::recv( broadCastConn );

			for( Heading::Header* header : broadCastConn.sendBuff )
			{
				Heading::SendStruct<0, 1>* parseMessage = ( Heading::SendStruct<0, 1>* )header;
				printf("Message : %s", parseMessage->buffer);
				delete header;
			}

			broadCastConn.sendBuff.clear();
		}

		Heading::disconnect( mainConn );
		Heading::disconnect( broadCastConn );

		//================================================================================================================================================================
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

	//===================================================================================================================================================================

	Heading::end();
}