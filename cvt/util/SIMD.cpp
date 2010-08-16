#include "util/SIMD.h"
#include "math/Math.h"

namespace cvt {
    SIMD* SIMD::_simd = 0;

    SIMD* SIMD::get()
    {
	if( !_simd ) {
	    _simd = new SIMD();
	}
	return _simd;
    }

    void SIMD::set_value_u8( uint8_t* dst, const size_t n, const uint8_t value ) const
    {
	size_t i = n & 0x2;
	uint32_t v = ( value << 24 ) | ( value << 16 ) | ( value << 8 ) | value;

	set_value_u32( ( uint32_t* ) dst, n >> 2, v );
	dst += n & ( ~ 0x02 );
	while( i-- )
	    *dst++ = value;
    }

    void SIMD::set_value_u16( uint16_t* dst, const size_t n, const uint16_t value ) const
    {
	uint32_t v = ( value << 16 ) | value;

	set_value_u32( ( uint32_t* ) dst, n >> 1, v );
	if( n & 1 ) {
	    dst += n & ( ~ 0x01 );
	    *dst++ = value;
	}
    }

    void SIMD::set_value_u32( uint32_t* dst, const size_t n, const uint32_t value ) const
    {
	size_t i = n;
	while( i-- )
	    *dst++ = value;
    }

    void SIMD::set_value_f( float* dst, const size_t n, const float value ) const
    {
	size_t i = n;
	while( i-- )
	    *dst++ = value;
    }

    void SIMD::set_value_4f( float* dst, const size_t n, const float (&value)[ 4 ] ) const
    {
	size_t i = n;
	while( i-- ) {
	    *dst++ = value[ 0 ];
	    *dst++ = value[ 1 ];
	    *dst++ = value[ 2 ];
	    *dst++ = value[ 3 ];
	}
    }

    void SIMD::add( float* dst, float const* src1, float const* src2, const size_t n ) const
    {
	size_t i = n >> 2;
	while( i-- ) {
	    *dst++ = *src1++ + *src2++;
	    *dst++ = *src1++ + *src2++;
	    *dst++ = *src1++ + *src2++;
	    *dst++ = *src1++ + *src2++;
	}
	i = n & 0x02;
	while( i-- )
	    *dst++ = *src1++ + *src2++;
    }

    void SIMD::sub( float* dst, float const* src1, float const* src2, const size_t n ) const
    {
	size_t i = n >> 2;
	while( i-- ) {
	    *dst++ = *src1++ - *src2++;
	    *dst++ = *src1++ - *src2++;
	    *dst++ = *src1++ - *src2++;
	    *dst++ = *src1++ - *src2++;
	}
	i = n & 0x02;
	while( i-- )
	    *dst++ = *src1++ - *src2++;
    }

    void SIMD::mul( float* dst, float const* src1, float const* src2, const size_t n ) const
    {
	size_t i = n >> 2;
	while( i-- ) {
	    *dst++ = *src1++ * *src2++;
	    *dst++ = *src1++ * *src2++;
	    *dst++ = *src1++ * *src2++;
	    *dst++ = *src1++ * *src2++;
	}
	i = n & 0x02;
	while( i-- )
	    *dst++ = *src1++ * *src2++;
    }

    void SIMD::div( float* dst, float const* src1, float const* src2, const size_t n ) const
    {
	size_t i = n >> 2;
	while( i-- ) {
	    *dst++ = *src1++ / *src2++;
	    *dst++ = *src1++ / *src2++;
	    *dst++ = *src1++ / *src2++;
	    *dst++ = *src1++ / *src2++;
	}
	i = n & 0x02;
	while( i-- )
	    *dst++ = *src1++ / *src2++;
    }

    void SIMD::mul_valuef_add( float* dst, float const* src1, const size_t n, float value ) const
    {
	size_t i = n >> 2;
	while( i-- ) {
	    *dst++ += *src1++ * value;
	    *dst++ += *src1++ * value;
	    *dst++ += *src1++ * value;
	    *dst++ += *src1++ * value;
	}
	i = n & 0x02;
	while( i-- )
	    *dst++ += *src1++ * value;
    }

    void SIMD::mul_valuef( float* dst, float* src, const size_t n, float value ) const
    {
	size_t i = n >> 2;
	while( i-- ) {
	    *dst++ = *src++ * value;
	    *dst++ = *src++ * value;
	    *dst++ = *src++ * value;
	    *dst++ = *src++ * value;
	}
	i = n & 0x02;
	while( i-- )
	    *dst++ = *src++ * value;

    }

    void SIMD::mul_valuef_sub( float* dst, float const* src1, const size_t n, float value ) const
    {
	size_t i = n >> 2;
	while( i-- ) {
	    *dst++ -= *src1++ * value;
	    *dst++ -= *src1++ * value;
	    *dst++ -= *src1++ * value;
	    *dst++ -= *src1++ * value;
	}
	i = n & 0x02;
	while( i-- )
	    *dst++ -= *src1++ * value;
    }

    void SIMD::mul_value4f_add( float* dst, float const* src1, const size_t n, float (&value)[ 4 ] ) const
    {
	size_t i = n >> 2;
	size_t x = 0;
	while( i-- ) {
	    *dst++ += *src1++ * value[ 0 ];
	    *dst++ += *src1++ * value[ 1 ];
	    *dst++ += *src1++ * value[ 2 ];
	    *dst++ += *src1++ * value[ 3 ];
	}
	i = n & 0x02;
	while( i-- ) {
	    *dst++ += *src1++ * value[ x++ ];
	    x &= 0x02;
	}
    }

    void SIMD::mul_value4f_sub( float* dst, float const* src1, const size_t n, float (&value)[ 4 ] ) const
    {
	size_t i = n >> 2;
	size_t x = 0;
	while( i-- ) {
	    *dst++ -= *src1++ * value[ 0 ];
	    *dst++ -= *src1++ * value[ 1 ];
	    *dst++ -= *src1++ * value[ 2 ];
	    *dst++ -= *src1++ * value[ 3 ];
	}
	i = n & 0x02;
	while( i-- ) {
	    *dst++ -= *src1++ * value[ x++ ];
	    x &= 0x02;
	}
    }

    void SIMD::conv_f_to_u8( uint8_t* dst, float* src, const size_t n )
    {
	size_t i = n >> 2;
	while( i-- ) {
	    *dst++ = ( uint8_t ) Math::clamp( *src++ * 255.0f, 0.0f, 255.0f );
	    *dst++ = ( uint8_t ) Math::clamp( *src++ * 255.0f, 0.0f, 255.0f );
	    *dst++ = ( uint8_t ) Math::clamp( *src++ * 255.0f, 0.0f, 255.0f );
	    *dst++ = ( uint8_t ) Math::clamp( *src++ * 255.0f, 0.0f, 255.0f );
	}
	i = n & 0x02;
	while( i-- )
	    *dst++ = ( uint8_t ) Math::clamp( *src++ * 255.0f, 0.0f, 255.0f );

    }

    void SIMD::conv_u8_to_f( float* dst, uint8_t* src, const size_t n )
    {
	size_t i = n >> 2;
	while( i-- ) {
	    *dst++ = ( float ) *src++ / 255.0f;
	    *dst++ = ( float ) *src++ / 255.0f;
	    *dst++ = ( float ) *src++ / 255.0f;
	    *dst++ = ( float ) *src++ / 255.0f;
	}
	i = n & 0x02;
	while( i-- )
	    *dst++ = ( float ) *src++ / 255.0f;
    }

}