#include "psudoPCH.h"

int main()
{
	WSAData Data;
	Heading::start( Data );

	//===================================================================================================================================================================

	try
	{
		bool IsClientLive = true;
		//================================================================================================================================================================

		Heading::connectionInfo mainConn;
		Heading::connectionInfo broadCastConn;

		ZeroMemory( &mainConn, sizeof( Heading::connectionInfo ) );
		ZeroMemory( &broadCastConn, sizeof( Heading::connectionInfo ) );

		Heading::createInfo( mainConn );
		Heading::createInfo( broadCastConn );

		Heading::connect( mainConn );
		Heading::connect( broadCastConn );

		while( IsClientLive )
		{

		}

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