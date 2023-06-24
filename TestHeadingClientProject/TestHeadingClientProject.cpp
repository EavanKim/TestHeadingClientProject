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
		length = sizeof( Header ) + _buffersize;
	}
};

typedef SendStruct<1, 0> SessionKey;
typedef SendStruct<2, 0> Shutdown;
typedef SendStruct<100, 43> TestBuffer;
#pragma pack(pop)

uint64_t m_resultsize = 0;

int TryConnect(SOCKET& _sock, addrinfo* _info)
{
	int result = 0;
	int loopCounter = 0;
	do
	{
		if( 5 < loopCounter )
		{
			int winerror = GetLastError();

			if( INVALID_SOCKET != _sock )
			{
				closesocket( _sock );
				_sock = INVALID_SOCKET;
			}
			// exception 객체 생성되면 throw하면서 에러 정보 송신
			return 1;
		}

		_sock = socket( _info->ai_family, _info->ai_socktype, _info->ai_protocol );
		if( INVALID_SOCKET == _sock )
		{
			continue;
		}

		result = connect( _sock, _info->ai_addr, ( int )_info->ai_addrlen );
		if( result == SOCKET_ERROR )
		{
			int err = 0;
			if( WSAECONNREFUSED == ( err = WSAGetLastError() ) )
			{
				closesocket( _sock );
				_sock = INVALID_SOCKET;
			}
			wprintf( L"connect failed with error: %d\n", err );
			freeaddrinfo( _info );
			continue;
		}
	}
	while( S_OK != result );
}

int main()
{
	uint64_t m_packetMulti = 1;
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

	if( 0 != TryConnect( m_socket, m_info ) )
		return 1;

	unsigned long long Count = 0;
	DWORD receiveSize = 0;
	while( 100 > Count )
	{
		for( uint64_t count = 0; 100 > count; ++count )
		{
			receiveSize = send( m_socket, sendBuffer, sizeof( testbuffer ) * m_packetMulti, 0 );
			if( -1 == receiveSize )
			{
				int sockerror = WSAGetLastError();
				int winerror = GetLastError();
				// 그냥 끝냅니다.

				closesocket( m_socket );
				m_socket = INVALID_SOCKET;

				if( 0 != TryConnect( m_socket, m_info ) )
					return 1;
			}
		}

		Sleep(100);
		++Count;
	}
	//================================================================================================================================================================

	Shutdown endpacket;
	receiveSize = send( m_socket, (char*)&endpacket, sizeof(Shutdown), 0);
	closesocket(m_socket);
	WSACleanup();
}