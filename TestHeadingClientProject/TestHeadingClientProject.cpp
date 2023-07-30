#include "psudoPCH.h"
#include <conio.h>

#if _WIN32
	#ifdef _DEBUG
		#include "crtdbg.h"
		#define new new( _CLIENT_BLOCK, __FILE__, __LINE__)
	#endif // _DEBUG
#endif

bool processChatting(IN OUT std::string& buffer)
{
	if (_kbhit() == 0)
		return false;

	// typing
	int input = _getch();
	if (input != 0x0D)
	{
		buffer.append(1, (char)input);
		return false;
	}

	// enter
	return true;
}

int main()
{
	WSAData Data = {};
	Heading::start( Data );

	try
	{
		bool IsClientLive = true;

		ChatConnectionInfo mainConn;
		mainConn.ip = std::string( "127.0.0.1" );
		mainConn.port = std::string( "50000" );
		mainConn.nickName = "testUser0\0";

		Heading::CConnector conn;
		conn.newConnect( mainConn );
		CChatClientSession session( mainConn );
		session.CreateAndSetEvent(FD_READ);

		std::string chattingBuffer;
		chattingBuffer.reserve(1 << 10);

		while( IsClientLive )
		{
			// non-blocking chatting
			if (processChatting(IN OUT chattingBuffer) == true)
			{
				Heading::PCK_CS_Chatting* pck = new Heading::PCK_CS_Chatting();
				{
					size_t bufferSize = sizeof(pck->buffer);
					memcpy_s(pck->buffer, bufferSize, chattingBuffer.c_str(), bufferSize - 1);
					pck->buffer[bufferSize - 1] = '\0';
				}

				session.enqueueSend(pck);
				chattingBuffer.clear();
			}

			session.Update();

			Sleep(1);
		}

		session.Release();
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
#if _WIN32
	#ifdef _DEBUG
		_CrtDumpMemoryLeaks();
	#endif
#endif
}