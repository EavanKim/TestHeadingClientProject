#ifndef UTIL_H__
#define UTIL_H__

class Util
{
public:
	static void PrintMem( char* _ptr, uint64_t _length )
	{
		for( uint64_t seek = 0; _length > seek; ++seek )
		{
			printf( "%02X", _ptr[ seek ] );
		}
	}

	static void ParseHeader( char* _buffer, Header& _parse )
	{
		memcpy( &_parse, _buffer, sizeof( Header ) );
	}

	static void ParseData( char* _buffer, TestBuffer& _parse )
	{
		memcpy( &_parse, _buffer, sizeof( TestBuffer ) );
	}

	static char* GetBuffer( Header*& _header )
	{
		char* result = nullptr;
		switch( _header->type )
		{
			case 1:
				result = ( ( SessionKey* )_header )->buffer;
				break;
			case 2:
				result = ( ( Shutdown* )_header )->buffer;
				break;
			case 3:
				result = ( ( Ping* )_header )->buffer;
				break;
			case 100:
				result = ( ( TestBuffer* )_header )->buffer;
				break;
			case 1000:
				result = ( ( ChatBuffer* )_header )->buffer;
				break;
		}

		return result;
	}
};

#endif