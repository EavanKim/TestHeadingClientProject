// TestHeadingProject.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include "psudoPCH.h"


uint64_t m_resultsize = 0;
char RecvBuffer[ 1 << 13 ];
bool IsNeedReconnectWait = false;

//================================================================================================================================================================
// 지금은 받는 패킷이 하나니까 더 복잡하게 처리 안할 예정.
void ProcessTime( time_t& _start, uint64_t& count, time_t now = time( NULL ) )
{
	if( UINT64_MAX == count )
	{
		count = 0;
	}
	uint64_t Time = now - _start;
	uint64_t Timedevide = ( 0 == Time ? 1 : Time );
	uint64_t MPS = count / Timedevide;
	printf( "[%lld][count : %lld][MPS : %lld] ", Time, count, MPS );
}

uint64_t ReadData( char* _buffer, uint64_t& _totalrecvSize, uint64_t& _counter )
{
	uint64_t processSize = 0;
	uint64_t lastLength = 0;

	while( 0 != _totalrecvSize )
	{
		if( _totalrecvSize < sizeof( Header ) )
			return processSize;

		char* currPtr = _buffer + lastLength;
		Header getHeader = {};
		uint64_t type = 0;
		uint64_t packlength = 0;
		Util::ParseHeader( currPtr, getHeader );

		switch( getHeader.type )
		{
			case 2:
				IsNeedReconnectWait = true;
				break;
			case 100:
			{
				if( _totalrecvSize < sizeof( TestBuffer ) )
					return processSize;
				TestBuffer parseData = {};

				Util::ParseData( currPtr, parseData );
				packlength = sizeof( TestBuffer );
				//printf( parseData.buffer );
				//printf( "\n" );
			}
			break;
			default:
				printf( "!!! Packet Parsing Failure !!! \n" );
				return processSize;
		}

		processSize += getHeader.length;
		lastLength += getHeader.length;
		_totalrecvSize -= getHeader.length;
		//printf("%lld \n", _totalrecvSize);

		++_counter;
	}

	return processSize;
}

uint64_t ProcessPacket( char* _buffer, uint64_t& _bufferSize, uint64_t& _counter, uint64_t& _reserveSize, DWORD& _receiveSize, time_t _start )
{
	// 일단 상수로 먼저 짭니다.
	// 어차피 Sync는 테스트 용도에 가까우므로 이 이후에 제대로 작성해야합니다.
	_bufferSize += _receiveSize;
	ProcessTime( _start, _counter );
	printf( "[reserveSize : %lld][receiveSize : %lld][bufferSize : %ulld]TestBuffer[% s] \n", _reserveSize, _receiveSize, _bufferSize, _buffer + _reserveSize );
	//++counter0;


	if( _bufferSize < sizeof( Header ) )
	{
		return 0;
	}

	return ReadData( _buffer, _bufferSize, _counter );
}

int ConnectInfoCreate( sockaddr_in& _result, addrinfo*& m_info )
{
	addrinfo createData = {};

	SecureZeroMemory( ( PVOID )&createData, sizeof( addrinfo ) );

	createData.ai_family = AF_INET;
	createData.ai_socktype = SOCK_STREAM;
	createData.ai_protocol = IPPROTO_TCP;
	createData.ai_flags = AI_PASSIVE;

	int result = getaddrinfo( nullptr, "50000", &createData, &m_info );
	if( S_OK != result )
	{
		// 초기화 실패
		int sockerror = WSAGetLastError();
		int winerror = GetLastError();
		// exception 객체 생성되면 throw하면서 에러 정보 송신
		return 1;
	}
	sockaddr_storage storage;
	memset( &storage, 0, sizeof storage );
	// The socket address to be passed to bind
	_result.sin_family = AF_INET;
	_result.sin_addr.s_addr = htonl( INADDR_ANY );
	_result.sin_port = htons( 50000 );

	return 0;
}

int CreateSocket( SOCKET& _listenSock, addrinfo* _info, sockaddr_in& _service )
{
	int result = 0;
	int loopCounter = 0;
	do
	{
		if( 5 < loopCounter )
		{
			int winerror = GetLastError();

			if( INVALID_SOCKET != _listenSock )
			{
				closesocket( _listenSock );
				_listenSock = INVALID_SOCKET;
			}
			// exception 객체 생성되면 throw하면서 에러 정보 송신
			return 1;
		}

		_listenSock = socket( _info->ai_family, _info->ai_socktype, _info->ai_protocol );
		if( INVALID_SOCKET == _listenSock )
		{
			continue;
		}

		result = bind( _listenSock, ( SOCKADDR* )&_service, sizeof( _service ) );
		if( result == SOCKET_ERROR )
		{
			int err = 0;
			if( WSAECONNREFUSED == ( err = WSAGetLastError() ) )
			{
				closesocket( _listenSock );
				_listenSock = INVALID_SOCKET;
				continue;
			}
			wprintf( L"connect failed with error: %d\n", err );
			freeaddrinfo( _info );
			closesocket( _listenSock );
			return 1;
		}
	}
	while( S_OK != result );

	return 0;
}

int WaitClient( SOCKET& _listenSock, SOCKET& _session )
{
	int result = listen( _listenSock, SOMAXCONN );
	if( result == SOCKET_ERROR )
	{
		printf( "listen failed with error: %d\n", WSAGetLastError() );
		closesocket( _listenSock );
		WSACleanup();
		return 1;
	}

	_session = accept( _listenSock, NULL, NULL );
	if( _session == INVALID_SOCKET )
	{
		printf( "accept failed with error: %d\n", WSAGetLastError() );
		closesocket( _listenSock );
		WSACleanup();
		return 1;
	}


	// No longer need server socket
	closesocket( _listenSock );
	_listenSock = INVALID_SOCKET;
	return 0;
}

void DoRawCode()
{
	//sockaddr_in service;
		//if( 0 != ConnectInfoCreate( service, m_info ) )
		//	return 1;
		//
		//if( 0 != CreateSocket( socket, m_info, service ) )
		//	return 1;
		//
		//DWORD receiveSize = 0;
		//
		//freeaddrinfo( m_info );
		//
		//SOCKET sessionOpen = INVALID_SOCKET;
		//
		//if( 0 != WaitClient( socket, sessionOpen ) )
		//	return 1;
		//
		//uint64_t NoSignalCheck = 0;
		//uint64_t counter0 = 0;
		//time_t start = time(NULL);
		//const uint64_t BUFFER_SIZE = sizeof( RecvBuffer );
		//while( 1 )
		//{
		//	uint64_t reserveSize = ( BUFFER_SIZE - bufferSize );
		//	if( 0 == reserveSize )
		//	{
		//		closesocket(sessionOpen);
		//		sessionOpen = INVALID_SOCKET;
		//		WSACleanup();
		//		return 1;
		//	}
		//
		//	receiveSize = recv( sessionOpen, RecvBuffer + bufferSize, reserveSize, 0 );
		//	if( SOCKET_ERROR == receiveSize || IsNeedReconnectWait || 1000 < NoSignalCheck )
		//	{
		//		int sockerror = WSAGetLastError();
		//		int winerror = GetLastError();
		//
		//		closesocket( sessionOpen );
		//		sessionOpen = INVALID_SOCKET;
		//
		//		if( 0 != ConnectInfoCreate( service, m_info ) )
		//			return 1;
		//
		//		if( 0 != CreateSocket( socket, m_info, service ) )
		//			return 1;
		//
		//		freeaddrinfo( m_info );
		//
		//
		//		printf("Ready For New Connect");
		//		if( 0 != WaitClient( socket, sessionOpen ) )
		//			return 1;
		//
		//		counter0 = 0;
		//		start = time( NULL );
		//		IsNeedReconnectWait = false;
		//		NoSignalCheck = 0;
		//		continue;
		//	}
		//	if( 0 == receiveSize )
		//	{
		//		++NoSignalCheck;
		//		continue;
		//	}
		//
		//	uint64_t ProcessLength = ProcessPacket( RecvBuffer, bufferSize, counter0, reserveSize, receiveSize, start );
		//
		//	if( 0 != ProcessLength )
		//	{
		//		if( 0 != bufferSize )
		//		{
		//			memcpy( RecvBuffer, RecvBuffer + ProcessLength, bufferSize );
		//		}
		//	}
		//
		//	printf( "[Process Size : %lld][bufferSize : %lld] \n", ProcessLength, bufferSize );
		//
		//	if( UINT64_MAX - 10000 <= counter0 )
		//	{
		//		counter0 = 0;
		//		start = time( NULL );
		//	}
		//}
}

//================================================================================================================================================================

void DoClassCode_First()
{
	//TestSock_Server server(50000);
		//
		//if( 0 != server.CreateInitializeData() )
		//	return 1;
		//
		//server.Binding();
		//CSession* session = server.Wating();
		//
		//if( nullptr == session )
		//	return 1;
		//
		//if( !session->IsConnected() )
		//{
		//	delete session;
		//}
		//
		//while( 1 )
		//{
		//	if( NULL == session || !session->IsConnected() )
		//	{
		//		session = server.Wating();
		//		if( !session->IsConnected() )
		//		{
		//			server.CloseSession( session );
		//			return 1;
		//		}
		//	}
		//
		//	session->Process();
		//}
}

//================================================================================================================================================================

void DoClassCode_Second()
{
	//TestServer_Select selectServer( 50000 );

	//if( 0 != selectServer.CreateInitializeData() )
	//	return 1;

	//selectServer.ListenBind();

	//while( 1 )
	//{
	//	selectServer.Update();
	//}
}

//================================================================================================================================================================

struct SimpleSession
{
	SimpleSession()
	{
		ZeroMemory( this, sizeof( SimpleSession ) );
	}

	SimpleSession( uint64_t _sesssionKey )
		: m_buffer( _sesssionKey )
	{
		ZeroMemory( this, sizeof( SimpleSession ) );
	}

	SimpleSession( SimpleSession& _copy )
		: m_sesssionKey( _copy.m_sesssionKey )
		, m_buffer( _copy.m_buffer )
	{
	}

	SimpleSession( SimpleSession&& _move )
		: m_sesssionKey( _move.m_sesssionKey )
		, m_buffer( _move.m_buffer )
	{
	}

	~SimpleSession()
	{
		for( Header* pending : m_sendBuffer )
		{
			delete pending;
		}
		m_sendBuffer.clear();
	}

	uint64_t				m_sesssionKey = 0;
	CNet_Buffer				m_buffer = {};
	std::vector<Header*>	m_sendBuffer = {};
};

struct SimpleSocket
{
	SOCKET _sock;
};

SOCKET				sock_bind = INVALID_SOCKET;
SOCKET				sock_broadCast = INVALID_SOCKET;
uint16_t			port_bind = 50000;
uint16_t			port_broadCast = 51000;
uint64_t			sessionNum = 0;
sockaddr_in			listenIn_bind = {};
sockaddr_in			listenIn_broadCast = {};
fd_set				fd_bind = {};
fd_set				fd_broadCast = {};
fd_set				fd_temp = {};
fd_set				fd_temp_R = {};
fd_set				fd_temp_W = {};
std::vector<SOCKET> clients_bind = {};
std::vector<SOCKET> clients_broadCast = {};

void fn_printTime()
{
	static time_t	startTime = time( NULL );
	time_t			currentTime = time( NULL );

	printf( "%lld", currentTime - startTime );
}

int fn_message_send( SOCKET& _socket, Header* _message )
{
	return send( _socket, ( char* )_message, _message->length, 0 );
}

void Setaddr( uint16_t _port, sockaddr_in& _listenIn )
{
	sockaddr_storage storage;
	memset( &storage, 0, sizeof storage );
	// The socket address to be passed to bind
	_listenIn.sin_family = AF_INET;
	_listenIn.sin_addr.s_addr = htonl( INADDR_ANY );
	_listenIn.sin_port = htons( _port );
}

SOCKET fn_bind( sockaddr_in& _listenIn )
{
	SOCKET result = INVALID_SOCKET;
	int returnValue = 0;
	int loopCounter = 0;

	// 새로 바인딩하면 초기화해버리기
	if( INVALID_SOCKET != result )
	{
		closesocket( result );
		result = INVALID_SOCKET;
	}

	do
	{
		if( 5 < loopCounter )
		{
			int winerror = GetLastError();
			// exception 객체 생성되면 throw하면서 에러 정보 송신
			return INVALID_SOCKET;
		}

		result = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
		if( INVALID_SOCKET == result )
		{
			continue;
		}

		returnValue = bind( result, ( SOCKADDR* )&_listenIn, sizeof( _listenIn ) );
		if( returnValue == SOCKET_ERROR )
		{
			int err = 0;
			if( WSAECONNREFUSED == ( err = WSAGetLastError() ) )
			{
				closesocket( result );
				result = INVALID_SOCKET;
				continue;
			}
			printf( "connect failed with error: %d\n", err );
			return INVALID_SOCKET;
		}
	}
	while( S_OK != returnValue );


	return result;
}

void fn_ready( SOCKET _bind, fd_set& _inSet )
{
	FD_ZERO( &_inSet );

	if( SOCKET_ERROR == listen( _bind, 5 ) )
		return;

	FD_SET( _bind, &_inSet );
}

uint64_t fn_select( const fd_set& _readfds, fd_set& _temp, SOCKET& _sock )
{
	_temp = _readfds;
	return select( _sock + 1, &_temp, NULL, NULL, NULL );
}

uint64_t fn_select_passThrough( const fd_set& _readfds, fd_set& _temp, SOCKET& _sock, uint32_t _wait = 5 )
{
	struct timeval tv;
	tv.tv_sec = _wait;
	tv.tv_usec = 0;
	_temp = _readfds;
	return  select( _sock + 1, &_temp, NULL, NULL, &tv );
}

uint64_t fn_select_send_passThrough( const fd_set& _write, fd_set& _temp, SOCKET& _sock, uint32_t _wait = 5 )
{
	struct timeval tv;
	tv.tv_sec = _wait;
	tv.tv_usec = 0;
	_temp = _write;
	return select( _sock + 1, NULL, &_temp, NULL, &tv );
}

uint64_t fn_select_complex_passThrough( fd_set& _temp_R, fd_set& _temp_W, SOCKET& _sock, uint32_t _wait = 5 )
{
	struct timeval tv;
	tv.tv_sec = _wait;
	tv.tv_usec = 0;
	return select( _sock + 1, &_temp_R, &_temp_W, NULL, &tv );
}

void fn_create( const SOCKET _newConn, fd_set& _set, std::unordered_map<SOCKET, SimpleSession>& _sockList )
{
	SOCKET newThing = accept( _newConn, NULL, NULL );
	if( INVALID_SOCKET != newThing )
	{
		++sessionNum;
		_sockList.insert( std::make_pair( newThing, SimpleSession( sessionNum ) ) );
		FD_SET( newThing, &_set );
	}
}

void fn_parse( const SOCKET _newConn, SimpleSession& _data, std::vector<Header*>& _recvMessage, std::vector<SOCKET> _disconnectList )
{
	uint64_t seek = 0;
	uint64_t currentSize = 0;
	char* data = nullptr;
	uint64_t length = 0;
	if( _data.m_buffer.get_buffer( &data, &length ) )
	{
		int readcount = recv( _newConn, data, length, 0 );
		if( -1 == readcount )
		{
			printf( "Disconnect : %i", _data.m_sesssionKey );
			_disconnectList.push_back( _newConn );
			return;
		}
		if( 0 == readcount )
		{
			_data.m_buffer.get_data( &_recvMessage );
			return;
		}

		_data.m_buffer.commit( readcount );
		_data.m_buffer.get_data( &_recvMessage );
	}
}

void fn_process_recv( fd_set& _set, std::vector<Header*>& _recvMessage, std::unordered_map<SOCKET, SimpleSession>& _sockList, fd_set& _originset, const SOCKET _CreateTarget )
{
	std::vector<SOCKET> removeList;
	for( uint64_t count = 0; _set.fd_count > count; ++count )
	{
		SOCKET currSock = _set.fd_array[ count ];

		if( FD_ISSET( currSock, &_set ) )
		{
			if( currSock == _CreateTarget )
			{
				fn_create( currSock, _originset, _sockList );
			}
			else
			{
				fn_parse( currSock, _sockList[ currSock ], _recvMessage, removeList );
			}
		}
	}

	for( SOCKET del : removeList )
	{
		FD_CLR( del, &_set );
		closesocket( del );
		_sockList.erase( del );
	}
}

void fn_process_send( const fd_set& _set, std::unordered_map<SOCKET, SimpleSession>& _sockMap, fd_set& _originset, const SOCKET _CreateTarget )
{
	Ping temp_ping;
	std::vector<SOCKET> removeList;
	for( uint64_t count = 0; _set.fd_count > count; ++count )
	{
		SOCKET currSock = _set.fd_array[ count ];

		if( FD_ISSET( currSock, &_set ) )
		{
			std::unordered_map<SOCKET, SimpleSession>::iterator session = _sockMap.find( currSock );
			if( 0 != session->second.m_sendBuffer.size() )
			{
				for( Header* header : session->second.m_sendBuffer )
				{
					int Result = send( currSock, ( char* )header, header->length, 0 );
					if( -1 == Result )
					{
						removeList.push_back( currSock );
						continue;
					}

					delete header;
				}

				session->second.m_sendBuffer.clear();
			}
			else
			{
				int Result = send( currSock, ( char* )&temp_ping, temp_ping.length, 0 );
				if( -1 == Result )
				{
					removeList.push_back( currSock );
					continue;
				}
			}
		}
	}

	for( SOCKET del : removeList )
	{
		FD_CLR( del, &_set );
		closesocket( del );
		_sockMap.erase( del );
	}
}

void fn_broadCast( std::unordered_map<SOCKET, SimpleSession>& _sockList, std::vector<Header*>& _recvMessage )
{
	std::unordered_map<SOCKET, SimpleSession>::iterator end = _sockList.end();
	for( std::unordered_map<SOCKET, SimpleSession>::iterator iter = _sockList.begin(); end != iter; ++iter )
	{
		SimpleSession& session = iter->second;
		for( Header*& sendData : _recvMessage )
		{
			Header* newData = nullptr;

			switch( sendData->type )
			{
				case 1000:
					newData = new ChatBuffer();
					memcpy( newData, sendData, sendData->length );
					break;
			}

			if( nullptr != newData )
				session.m_sendBuffer.push_back( newData );
		}
	}

	for( Header*& sendData : _recvMessage )
		delete sendData;

	_recvMessage.clear();
}

//================================================================================================================================================================

typedef std::unordered_map<SOCKET, SimpleSession> socketMap;

int main()
{
	WSADATA m_data = {};
	int result = WSAStartup( MAKEWORD( 2, 2 ), &m_data );
	if( S_OK != result )
	{
		// 초기화 실패
		int sockerror = WSAGetLastError();
		int winerror = GetLastError();
		// exception 객체 생성되면 throw하면서 에러 정보 송신
		return 1;
	}

	//================================================================================================================================================================
	// DoRawCode();
	//================================================================================================================================================================

	CSocket proxySocket("127.0.0.1", "50000");
	CSocket broadCastSocket("127.0.0.1", "51000");

	proxySocket.C_Connect();
	broadCastSocket.C_Connect();

	proxySocket.CreateSessionKey();
	broadCastSocket.CreateSessionKey();

	char staticbuffer[999];
	std::string buffer;
	bool ProgramAlive = true;
	while( ProgramAlive )
	{
		ZeroMemory( staticbuffer, 999 );
		ChatBuffer sendbuffer;
		// 단순한 구조인 지금 상황에서 매번 데이터를 갱신받으려면 빈 데이터라도 보내야합니다.
		// 업데이트 요청을 기본으로 발송합니다.
		memcpy( sendbuffer.buffer, "Request Update", 15 );
		std::cin.getline( staticbuffer, 999 );

		//uint64_t length = buffer.length();
		uint64_t length = strlen( staticbuffer );
		if(1 == length)
		{
			if( '0' == staticbuffer[ 0 ] )
			{
				// 여기서 바로 cleanup시키고 리턴해도 되지만 그냥 빠져나가기 처리로 합니다.
				proxySocket.C_Disconnect();
				broadCastSocket.C_Disconnect();
				ProgramAlive = false;
				continue;
			}
		}
		
		if( 0 != length )
		{
			ZeroMemory( &sendbuffer.buffer, sendbuffer.length - sizeof( Header ) );
			// 작으면 글 버퍼 사이즈만큼,
			// 크다면 오버한만큼 자릅니다.
			memcpy( sendbuffer.buffer, staticbuffer, length > sendbuffer.length ? sendbuffer.length : length );
		}
		proxySocket.C_Send( &sendbuffer );
		int result = broadCastSocket.C_Recv();
		if( 0 < result )
		{
			broadCastSocket.PrintAllData();
		}
		else if( -1 == result )
		{
			proxySocket.C_Disconnect();
			broadCastSocket.C_Disconnect();

			break;
		}

		buffer = "";
	}

	WSACleanup();
}