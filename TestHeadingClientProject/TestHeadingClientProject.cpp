// TestHeadingProject.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include <iostream>

#include <winsock2.h>
#include <ws2tcpip.h>
#include <ws2ipdef.h>

#pragma comment(lib, "ws2_32.lib")
#include <Windows.h>

#pragma pack(push, 1)
struct Header
{
	uint64_t type = 0;
	uint64_t length = 0;
};

template<uint64_t _type, uint64_t _buffersize>
struct SendStruct : public Header
{
	char buffer[ _buffersize ];

	SendStruct()
	{
		type = _type;
		length = _buffersize;
	}
};

typedef SendStruct<1, 43> TestBuffer;
#pragma pack(pop)

uint64_t m_resultsize = 0;
char Buffer[ 1 << 13 ];
char RecvBuffer[ 1 << 13 ];

void WriteResultBuffer( char* _buffer, uint64_t _length )
{
	char* destPtr = ( char* )( Buffer + m_resultsize );
	uint64_t length = ( uint64_t )( 1 << 13 ) - m_resultsize;
	memcpy_s( destPtr, length, _buffer, _length );

	m_resultsize += _length;
}

int main()
{
	uint64_t m_packetMulti = 5;
	uint64_t m_bufferSize = 0;

	uint64_t m_currentpacketSize = 0;
	uint64_t m_totalRecv = 0;

	WSADATA m_data = {};
	addrinfo* m_info = nullptr;
	SOCKET m_socket = INVALID_SOCKET;
	TestBuffer testbuffer;

	uint64_t testdata = 0;
	uint64_t testdata1 = 43;

	memcpy( &testbuffer.buffer, "!!! this is test Text this is test Text !!", 43 );

	char* sendBuffer = ( char* )malloc( sizeof( testbuffer ) * m_packetMulti );

	for( unsigned long count = 0; m_packetMulti > count; ++count )
		memcpy( sendBuffer + ( count * sizeof( testbuffer ) ), &testbuffer, sizeof( testbuffer ) );

	printf("Created Packet Info \n");
	printf("TestBuffer \n");
	printf("type : %lld \n", testbuffer.type);
	printf("length : %lld \n", testbuffer.length);
	printf("Buffer : %s \n", testbuffer.buffer);

	int result = WSAStartup( MAKEWORD( 2, 2 ), &m_data );
	if( S_OK != result )
	{
		// 초기화 실패
		int sockerror = WSAGetLastError();
		int winerror = GetLastError();
		// exception 객체 생성되면 throw하면서 에러 정보 송신
		return 1;
	}

	addrinfo createData = {};

	SecureZeroMemory( ( PVOID )&createData, sizeof( addrinfo ) );

	createData.ai_family = AF_UNSPEC;
	createData.ai_socktype = SOCK_STREAM;
	createData.ai_protocol = IPPROTO_TCP;

	result = getaddrinfo( "127.0.0.1", "50000", &createData, &m_info );
	if( S_OK != result )
	{
		// 초기화 실패
		int sockerror = WSAGetLastError();
		int winerror = GetLastError();
		// exception 객체 생성되면 throw하면서 에러 정보 송신
		return 1;
	}


	int loopCounter = 0;
	do
	{
		if( 5 < loopCounter )
		{
			int winerror = GetLastError();

			if( INVALID_SOCKET != m_socket )
			{
				closesocket( m_socket );
				m_socket = INVALID_SOCKET;
			}
			// exception 객체 생성되면 throw하면서 에러 정보 송신
			return 1;
		}

		m_socket = socket( m_info->ai_family, m_info->ai_socktype, m_info->ai_protocol );
		if( INVALID_SOCKET == m_socket )
		{
			continue;
		}

		result = connect( m_socket, m_info->ai_addr, ( int )m_info->ai_addrlen );
		if( result == SOCKET_ERROR )
		{
			int err = 0;
			if( WSAECONNREFUSED == ( err = WSAGetLastError() ) )
			{
				closesocket( m_socket );
				m_socket = INVALID_SOCKET;
			}
			wprintf( L"connect failed with error: %d\n", err );
			freeaddrinfo( m_info );
			continue;
		}
	}
	while( S_OK != result );

	DWORD receiveSize = 0;
	while( 1 )
	{
		//receiveSize = send(m_socket, ( char* )&testdata, 8, 0);
		//if( -1 == receiveSize )
		//{
		//	int sockerror = WSAGetLastError();
		//	int winerror = GetLastError();
		//	// 창이 안보일때가 있으니 Yield 시킵니다.
		//	Sleep( 1 );
		//	// 에러복구
		//	continue;
		//}
		//Sleep( 10 );
		//receiveSize = send(m_socket, ( char* )&testdata1, 8, 0);
		//if( -1 == receiveSize )
		//{
		//	int sockerror = WSAGetLastError();
		//	int winerror = GetLastError();
		//	// 창이 안보일때가 있으니 Yield 시킵니다.
		//	Sleep( 1 );
		//	// 에러복구
		//	continue;
		//}
		//Sleep( 10 );
		printf("!!! TRY Send !!! \n");
		receiveSize = send(m_socket, sendBuffer, sizeof( testbuffer ) * m_packetMulti, 0);
		if( -1 == receiveSize )
		{
			int sockerror = WSAGetLastError();
			int winerror = GetLastError();
			// 창이 안보일때가 있으니 Yield 시킵니다.
			Sleep( 1 );
			// 에러복구
			continue;
		}
		printf( "!!! End Send !!! \n" );

		Sleep(1000);
	}
	//================================================================================================================================================================
}