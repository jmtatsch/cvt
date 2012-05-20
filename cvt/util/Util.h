/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef CVT_UTIL_H
#define CVT_UTIL_H

#include <stdint.h>
#include <stdlib.h>
#include <cvt/util/String.h>
#include <cvt/util/Exception.h>

#define DEBUG_PRINT( VAR ) std::cout << #VAR << " = " << VAR << std::endl;

namespace cvt {

	class Util {
		public:
			static inline uint16_t bswap16( uint16_t x );
			static inline uint32_t bswap32( uint32_t x );
			static inline uint64_t bswap64( uint64_t x );
			static inline uint32_t UTF8toUTF32( const char* data, char** endptr );
			static inline bool	   getEnv( String& value, const String& name );
			static inline String   getDirectoryFromPath( const String& path );
			static inline String   getBasenameFromPath( const String& path );
			template< typename T>
			static inline T*	   alignPtr( T* ptr, size_t alignment );

	};

	inline uint16_t Util::bswap16( uint16_t x )
	{
		return ( x << 8 ) | ( x >> 8 );
	}

	inline uint32_t Util::bswap32( uint32_t x )
	{
		return ( x << 24) | ( x >> 24 ) | ( ( x << 8 ) & 0xff0000 ) | ( ( x >> 8 ) & 0xff00 );
	}

	inline uint64_t Util::bswap64( uint64_t x )
	{
		return ( ( uint64_t ) bswap32( x & 0xffffffff ) ) << 32 | ( uint64_t ) bswap32( x >> 32 );
	}

	inline uint32_t Util::UTF8toUTF32( const char* _data, char** _endptr )
	{
		static const uint8_t _UTF8_LENGTH[256] = {
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
			1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
			2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
			3,3,3,3,3,3,3,3,4,4,4,4,5,5,0,0
		};
		uint8_t* data = ( uint8_t* ) _data;

		if( *( ( uint8_t* ) data ) < 0x80 )
			return *data;

		uint8_t len = _UTF8_LENGTH[ *data ];

		if( len > 4 )
			throw CVTException("Invalid UTF-8 sequence");

		uint32_t ret = ( *data++ ) & ( 0x3F >> len );
		while( len-- ) {
			if( ( *data & 0xC0 ) != 0x80 )
				throw CVTException("Invalid UTF-8 sequence");
			ret = ( ( ret << 6 ) + ( *data++ & 0x3F ) );
		}
		if( _endptr )
			*_endptr = ( char* ) ( data );
		return ret;
	}


	inline bool Util::getEnv( String& value, const String& name )
	{
		char* val;
		if( ( val = getenv( name.c_str()) ) ) {
			value = val;
			return true;
		}
		return false;
	}

	inline String Util::getDirectoryFromPath( const String& path )
	{
		ssize_t pos = path.rfind( '/' );
		if( pos < 0 )
			return String( "./" );
		return path.substring( 0, pos + 1 );
	}

	inline String Util::getBasenameFromPath( const String& path )
	{
		ssize_t pos = path.rfind( '/' );
		if( pos < 0 )
			return String( path );
		return path.substring( pos + 1 );
	}


	template<typename T>
	inline T* Util::alignPtr( T* ptr, size_t alignment )
	{
		size_t rem = ( ( size_t ) ptr ) % alignment;
		if( !rem )
			return ptr;
		return ( T* ) ( ( ( uint8_t*) ptr ) + alignment - rem );
	}

}

#endif
