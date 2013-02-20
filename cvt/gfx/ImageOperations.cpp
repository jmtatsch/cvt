/*
            CVT - Computer Vision Tools Library

     Copyright (c) 2012, Philipp Heise, Sebastian Klose

    THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
    KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
    PARTICULAR PURPOSE.
 */
#include <cvt/gfx/Image.h>
#include <cvt/math/Math.h>
#include <cvt/util/SIMD.h>
#include <cvt/util/Exception.h>
#include <cvt/util/ScopedBuffer.h>
#include <cvt/gfx/IMapScoped.h>

#include <iomanip>

namespace cvt {

	void Image::convert( Image& dst, IConvertFlags flags ) const
	{
		IConvert::convert( dst, *this, flags );
	}

	void Image::convert( Image & dst, const IFormat & dstFormat, IConvertFlags flags  ) const
	{
		dst.reallocate( _mem->_width, _mem->_height, dstFormat, dst.memType() );
		IConvert::convert( dst, *this, flags );
	}

	void Image::convert( Image& dst, const IFormat & dstformat, IAllocatorType memtype, IConvertFlags flags ) const
	{
		dst.reallocate( _mem->_width, _mem->_height, dstformat, memtype );
		IConvert::convert( dst, *this, flags );
	}

	void Image::fill( const Color& c )
	{
		size_t h, stride;
		uint8_t* dst;
		uint8_t* dbase;
		SIMD* simd = SIMD::instance();

		switch ( _mem->_format.formatID ) {
			case IFORMAT_GRAY_UINT8:
				{
					uint8_t v = ( uint8_t ) ( 255.0f * c.gray() );
					h = _mem->_height;
					dbase = dst = map( &stride );
					while( h-- ) {
						simd->SetValueU8( dst, v, _mem->_width );
						dst += stride;
					}
					unmap( dbase );
				}
				break;
			case IFORMAT_GRAY_UINT16:
				throw CVTException("Unimplemented");
				break;
			case IFORMAT_GRAY_INT16:
				throw CVTException("Unimplemented");
				break;
			case IFORMAT_GRAY_FLOAT:
				{
					float v = c.gray();

					h = _mem->_height;
					dbase = dst = map( &stride );
					while( h-- ) {
						simd->SetValue1f( ( float* ) dst, v, _mem->_width );
						dst += stride;
					}
					unmap( dbase );
				}
				break;

			case IFORMAT_GRAYALPHA_UINT8:
				{
					uint16_t v = ( uint16_t ) ( ( ( uint16_t ) ( 255.0f * c.alpha() ) ) << 8 );
					v |= ( ( uint16_t ) ( 255.0f * c.gray() ));
					h = _mem->_height;
					dbase = dst = map( &stride );
					while( h-- ) {
						simd->SetValueU16( ( uint16_t* ) dst, v, _mem->_width );
						dst += stride;
					}
					unmap( dbase );
				}
				break;
			case IFORMAT_GRAYALPHA_UINT16:
				throw CVTException("Unimplemented");
				break;
			case IFORMAT_GRAYALPHA_INT16:
				throw CVTException("Unimplemented");
				break;
			case IFORMAT_GRAYALPHA_FLOAT:
				{
					float v[ 2 ] = { c.gray(), c.alpha() };

					h = _mem->_height;
					dbase = dst = map( &stride );
					while( h-- ) {
						simd->SetValue2f( ( float* ) dst, v, _mem->_width );
						dst += stride;
					}
					unmap( dbase );
				}
				break;
			case IFORMAT_RGBA_UINT8:
				{
					uint32_t v = ( ( uint32_t ) ( 255.0f * c.alpha() ) ) << 24;
					v |= ( ( uint32_t ) ( 255.0f * c.blue() ) ) << 16;
					v |= ( ( uint32_t ) ( 255.0f * c.green() ) ) << 8;
					v |= ( ( uint32_t ) ( 255.0f * c.red() ) );

					h = _mem->_height;
					dbase = dst = map( &stride );
					while( h-- ) {
						simd->SetValueU32( ( uint32_t* ) dst, v, _mem->_width );
						dst += stride;
					}
					unmap( dbase );
				}
				break;
			case IFORMAT_RGBA_UINT16:
				throw CVTException("Unimplemented");
				break;
			case IFORMAT_RGBA_INT16:
				throw CVTException("Unimplemented");
				break;
			case IFORMAT_RGBA_FLOAT:
				{
					float v[ 4 ] = { c.red(), c.green(), c.blue(), c.alpha() };

					h = _mem->_height;
					dbase = dst = map( &stride );
					while( h-- ) {
						simd->SetValue4f( ( float* ) dst, v, _mem->_width );
						dst += stride;
					}
					unmap( dbase );
				}
				break;
			case IFORMAT_BGRA_UINT8:
				{
					uint32_t v = ( ( uint32_t ) ( 255.0f * c.alpha() ) ) << 24;
					v |= ( ( uint32_t ) ( 255.0f * c.red() ) ) << 16;
					v |= ( ( uint32_t ) ( 255.0f * c.green() ) ) << 8;
					v |= ( ( uint32_t ) ( 255.0f * c.blue() ) );

					h = _mem->_height;
					dbase = dst = map( &stride );
					while( h-- ) {
						simd->SetValueU32( ( uint32_t* ) dst, v, _mem->_width );
						dst += stride;
					}
					unmap( dbase );
				}
				break;
			case IFORMAT_BGRA_UINT16:
				throw CVTException("Unimplemented");
				break;
			case IFORMAT_BGRA_INT16:
				throw CVTException("Unimplemented");
				break;
			case IFORMAT_BGRA_FLOAT:
				{
					float v[ 4 ] = { c.blue(), c.green(), c.red(), c.alpha() };

					h = _mem->_height;
					dbase = dst = map( &stride );
					while( h-- ) {
						simd->SetValue4f( ( float* ) dst, v, _mem->_width );
						dst += stride;
					}
					unmap( dbase );
				}
				break;

			case IFORMAT_BAYER_RGGB_UINT8:
				throw CVTException("Unimplemented");
				break;
			default:
				throw CVTException("Unimplemented");
				break;
		}
	}

	void Image::add( float alpha )
	{
		SIMD* simd = SIMD::instance();
		switch( _mem->_format.type ) {
			case IFORMAT_TYPE_FLOAT:
				{
					size_t stride;
					uint8_t* dst = map( &stride );
					uint8_t* dbase = dst;
					size_t h = _mem->_height;
					while( h-- ) {
						simd->AddValue1f( ( float* ) dst, ( float* ) dst, alpha, _mem->_width * _mem->_format.channels );
						dst += stride;
					}
					unmap( dbase );
				}
				break;
			default:
				throw CVTException("Unimplemented");

		}
	}


	void Image::add( const Color& c )
	{
		SIMD* simd = SIMD::instance();
		switch( _mem->_format.formatID ) {
			case IFORMAT_GRAY_FLOAT:
				{
					size_t stride;
					uint8_t* dst = map( & stride );
					uint8_t* dbase = dst;
					size_t h = _mem->_height;

					while( h-- ) {
						simd->AddValue1f( ( float* ) dst, ( float* ) dst, c.gray(), _mem->_width );
						dst += stride;
					}
					unmap( dbase );
				}
				break;
			case IFORMAT_RGBA_FLOAT:
				{
					float v[ 4 ] = { c.red(), c.green(), c.blue(), c.alpha() };
					size_t stride;
					uint8_t* dst = map( &stride );
					uint8_t* dbase = dst;
					size_t h = _mem->_height;

					while( h-- ) {
						simd->AddValue4f( ( float* ) dst, ( float* ) dst, v, _mem->_width * _mem->_format.channels );
						dst += stride;
					}
					unmap( dbase );
				}
				break;
			case IFORMAT_BGRA_FLOAT:
				{
					float v[ 4 ] = { c.blue(), c.green(), c.red(), c.alpha() };
					size_t stride;
					uint8_t* dst = map( &stride );
					uint8_t* dbase = dst;
					size_t h = _mem->_height;

					while( h-- ) {
						simd->AddValue4f( ( float* ) dst, ( float* ) dst, v, _mem->_width * _mem->_format.channels );
						dst += stride;
					}
					unmap( dbase );
				}
				break;
			default:
				throw CVTException("Unimplemented");

		}
	}

	void Image::sub( float alpha )
	{
		SIMD* simd = SIMD::instance();
		switch( _mem->_format.type ) {
			case IFORMAT_TYPE_FLOAT:
				{
					size_t stride;
					uint8_t* dst = map( &stride );
					uint8_t* dbase = dst;
					size_t h = _mem->_height;
					while( h-- ) {
						simd->SubValue1f( ( float* ) dst, ( float* ) dst, alpha, _mem->_width * _mem->_format.channels );
						dst += stride;
					}
					unmap( dbase );
				}
				break;
			default:
				throw CVTException("Unimplemented");

		}
	}

	void Image::sub( const Color& c )
	{
		SIMD* simd = SIMD::instance();
		switch( _mem->_format.formatID ) {
			case IFORMAT_GRAY_FLOAT:
				{
					size_t stride;
					uint8_t* dst = map( &stride );
					uint8_t* dbase = dst;
					size_t h = _mem->_height;

					while( h-- ) {
						simd->SubValue1f( ( float* ) dst, ( float* ) dst, c.gray(), _mem->_width );
						dst += stride;
					}
					unmap( dbase );
				}
				break;
			case IFORMAT_RGBA_FLOAT:
				{
					float v[ 4 ] = { c.red(), c.green(), c.blue(), c.alpha() };
					size_t stride;
					uint8_t* dst = map( &stride );
					uint8_t* dbase = dst;
					size_t h = _mem->_height;

					while( h-- ) {
						simd->SubValue4f( ( float* ) dst, ( float* ) dst, v, _mem->_width * _mem->_format.channels );
						dst += stride;
					}
					unmap( dbase );
				}
				break;
			case IFORMAT_BGRA_FLOAT:
				{
					float v[ 4 ] = { c.blue(), c.green(), c.red(), c.alpha() };
					size_t stride;
					uint8_t* dst = map( &stride );
					uint8_t* dbase = dst;
					size_t h = _mem->_height;

					while( h-- ) {
						simd->SubValue4f( ( float* ) dst, ( float* ) dst, v, _mem->_width * _mem->_format.channels );
						dst += stride;
					}
					unmap( dbase );
				}
				break;


			default:
				throw CVTException("Unimplemented");

		}
	}

	void Image::mul( float alpha )
	{
		SIMD* simd = SIMD::instance();

		switch( _mem->_format.type ) {
			case IFORMAT_TYPE_FLOAT:
				{
					size_t stride;
					uint8_t* dst = map( &stride );
					uint8_t* dbase = dst;
					size_t h = _mem->_height;

					while( h-- ) {
						simd->MulValue1f( ( float* ) dst, ( float* ) dst, alpha, _mem->_width * _mem->_format.channels );
						dst += stride;
					}
					unmap( dbase );
				}
				break;
			case IFORMAT_TYPE_UINT16:
				{
					size_t stride;
					uint16_t* dst = map<uint16_t>( &stride );
					uint16_t* dbase = dst;
					size_t h = _mem->_height;

					while( h-- ) {
						simd->MulValue1ui16( dst, dst, alpha, _mem->_width * _mem->_format.channels );
						dst += stride;
					}
					unmap( dbase );
				}
				break;
			default:
				throw CVTException("Unimplemented");

		}
	}

	void Image::mul( const Color& c )
	{
		SIMD* simd = SIMD::instance();
		switch( _mem->_format.formatID ) {
			case IFORMAT_GRAY_FLOAT:
				{
					size_t stride;
					uint8_t* dst = map( &stride );
					uint8_t* dbase = dst;
					size_t h = _mem->_height;

					while( h-- ) {
						simd->MulValue1f( ( float* ) dst, ( float* ) dst, c.gray(), _mem->_width );
						dst += stride;
					}
					unmap( dbase );
				}
				break;
			case IFORMAT_RGBA_FLOAT:
				{
					float v[ 4 ] = { c.red(), c.green(), c.blue(), c.alpha() };
					size_t stride;
					uint8_t* dst = map( &stride );
					uint8_t* dbase = dst;
					size_t h = _mem->_height;

					while( h-- ) {
						simd->MulValue4f( ( float* ) dst, ( float* ) dst, v, _mem->_width * _mem->_format.channels );
						dst += stride;
					}
					unmap( dbase );
				}
				break;
			case IFORMAT_BGRA_FLOAT:
				{
					float v[ 4 ] = { c.blue(), c.green(), c.red(), c.alpha() };
					size_t stride;
					uint8_t* dst = map( &stride );
					uint8_t* dbase = dst;
					size_t h = _mem->_height;

					while( h-- ) {
						simd->MulValue4f( ( float* ) dst, ( float* ) dst, v, _mem->_width * _mem->_format.channels );
						dst += stride;
					}
					unmap( dbase );
				}
				break;
			default:
				throw CVTException("Unimplemented");

		}
	}

	void Image::add( const Image& i )
	{
		if( _mem->_width != i._mem->_width ||
			_mem->_height != i._mem->_height ||
			_mem->_format != i._mem->_format )
			throw CVTException("Image mismatch");

		SIMD* simd = SIMD::instance();
		switch( _mem->_format.type ) {
			case IFORMAT_TYPE_FLOAT:
				{
					size_t sstride, dstride;
					const uint8_t* src = i.map( &sstride );
					const uint8_t* sbase = src;
					uint8_t* dst = map( &dstride );
					uint8_t* dbase = dst;

					size_t h = _mem->_height;
					while( h-- ) {
						simd->Add( ( float* ) dst, ( float* ) dst, ( float* ) src, _mem->_width * _mem->_format.channels );
						src += sstride;
						dst += dstride;
					}
					unmap( dbase );
					i.unmap( sbase );
				}
				break;
			default:
				throw CVTException("Unimplemented");

		}
	}

	void Image::sub( const Image& i )
	{
		if( _mem->_width != i._mem->_width ||
			_mem->_height != i._mem->_height ||
			_mem->_format != i._mem->_format )
			throw CVTException("Image mismatch");

		SIMD* simd = SIMD::instance();
		switch( _mem->_format.type ) {
			case IFORMAT_TYPE_FLOAT:
				{
					size_t sstride, dstride;
					const uint8_t* src = i.map( &sstride );
					const uint8_t* sbase = src;
					uint8_t* dst = map( &dstride );
					uint8_t* dbase = dst;

					size_t h = _mem->_height;
					while( h-- ) {
						simd->Sub( ( float* ) dst, ( float* ) dst, ( float* ) src, _mem->_width * _mem->_format.channels );
						src += sstride;
						dst += dstride;
					}
					unmap( dbase );
					i.unmap( sbase );
				}
				break;
			default:
				throw CVTException("Unimplemented");

		}
	}

	void Image::mul( const Image& i )
	{
		if( _mem->_width != i._mem->_width ||
			_mem->_height != i._mem->_height ||
			_mem->_format != i._mem->_format )
			throw CVTException("Image mismatch");

		SIMD* simd = SIMD::instance();
		switch( _mem->_format.type ) {
			case IFORMAT_TYPE_FLOAT:
				{
					size_t sstride, dstride;
					const uint8_t* src = i.map( &sstride );
					const uint8_t* sbase = src;
					uint8_t* dst = map( &dstride );
					uint8_t* dbase = dst;

					size_t h = _mem->_height;
					while( h-- ) {
						simd->Mul(( float* ) dst, ( float* ) dst, ( float* ) src, _mem->_width * _mem->_format.channels );
						src += sstride;
						dst += dstride;
					}
					unmap( dbase);
					i.unmap( sbase );
				}
				break;
			default:
				throw CVTException("Unimplemented");

		}
	}

	void Image::mad( const Image& i, float alpha )
	{
		if( _mem->_width != i._mem->_width ||
			_mem->_height != i._mem->_height ||
			_mem->_format != i._mem->_format )
			throw CVTException("Image mismatch");

		SIMD* simd = SIMD::instance();
		switch( _mem->_format.type ) {
			case IFORMAT_TYPE_FLOAT:
				{
					IMapScoped<const float> srcmap( i );
					IMapScoped<float> dstmap( *this );

					size_t h = height();
					while( h-- ) {
						simd->MulAddValue1f( dstmap.ptr(), srcmap.ptr(), alpha, width() * _mem->_format.channels );
						srcmap++;
						dstmap++;
					}
				}
				break;
			default:
				throw CVTException("Unimplemented");

		}
	}

	float Image::ssd( const Image& i ) const
	{
		if( _mem->_width != i._mem->_width ||
			_mem->_height != i._mem->_height ||
			_mem->_format != i._mem->_format )
			throw CVTException("Image mismatch");

		SIMD* simd = SIMD::instance();

		float ssd = 0.0f;

		switch( _mem->_format.type ) {
			case IFORMAT_TYPE_FLOAT:
			{
				size_t astride, bstride;
				const uint8_t* srcA = i.map( &astride );
				const uint8_t* baseA = srcA;
				const uint8_t* srcB = map( &bstride );
				const uint8_t* baseB = srcB;

				size_t h = _mem->_height;
				while( h-- ) {
					ssd += simd->SSD( ( float* ) srcA, ( float* )srcB, _mem->_width * _mem->_format.channels );
					srcA += astride;
					srcB += bstride;
				}
				unmap( baseB );
				i.unmap( baseA );
			}
				break;
			case IFORMAT_TYPE_UINT8:
			{
				size_t astride, bstride;
				const uint8_t* srcA = i.map( &astride );
				const uint8_t* baseA = srcA;
				const uint8_t* srcB = map( &bstride );
				const uint8_t* baseB = srcB;

				size_t h = _mem->_height;
				while( h-- ) {
					ssd += simd->SSD( srcA, srcB, _mem->_width * _mem->_format.channels );
					srcA += astride;
					srcB += bstride;
				}
				unmap( baseB );
				i.unmap( baseA );
			}
				break;
			default:
				throw CVTException("Unimplemented");

		}
		return ssd;
	}

	float Image::sad( const Image& i ) const
	{
		if( _mem->_width != i._mem->_width ||
			_mem->_height != i._mem->_height ||
			_mem->_format != i._mem->_format )
			throw CVTException("Image mismatch");

		SIMD* simd = SIMD::instance();

		float sad = 0.0f;

		switch( _mem->_format.type ) {
			case IFORMAT_TYPE_FLOAT:
				{
					size_t astride, bstride;
					const uint8_t* srcA = i.map( &astride );
					const uint8_t* baseA = srcA;
					const uint8_t* srcB = map( &bstride );
					const uint8_t* baseB = srcB;

					size_t h = _mem->_height;
					while( h-- ) {
						sad += simd->SAD( ( float* ) srcA, ( float* )srcB, _mem->_width * _mem->_format.channels );
						srcA += astride;
						srcB += bstride;
					}
					unmap( baseB );
					i.unmap( baseA );
				}
				break;
			case IFORMAT_TYPE_UINT8:
			{
				size_t astride, bstride;
				const uint8_t* srcA = i.map( &astride );
				const uint8_t* baseA = srcA;
				const uint8_t* srcB = map( &bstride );
				const uint8_t* baseB = srcB;

				size_t h = _mem->_height;
				while( h-- ) {
					sad += simd->SAD( srcA, srcB, _mem->_width * _mem->_format.channels );
					srcA += astride;
					srcB += bstride;
				}
				unmap( baseB );
				i.unmap( baseA );
			}
				break;
			default:
				throw CVTException("Unimplemented");

		}

		return sad ;
	}

	void Image::convolve( Image& idst, const IKernel& kernel ) const
	{
		if( _mem->_format.type == IFORMAT_TYPE_FLOAT && idst._mem->_format.type == IFORMAT_TYPE_FLOAT )
			convolveFloat( idst, kernel );
		else if( _mem->_format.type == IFORMAT_TYPE_UINT8 && idst._mem->_format.type == IFORMAT_TYPE_UINT8 )
			convolveU8( idst, kernel );
		else if( _mem->_format.type == IFORMAT_TYPE_UINT8 && idst._mem->_format.type == IFORMAT_TYPE_INT16 )
			convolveU8_to_S16( idst, kernel );
		else {
			throw CVTException("Unimplemented");
		}
	}

	void Image::convolve( Image& idst, const IKernel& hkernel, const IKernel& vkernel ) const
	{
		if( _mem->_format.type == IFORMAT_TYPE_FLOAT && idst._mem->_format.type == IFORMAT_TYPE_FLOAT )
			convolveSeperableFloat( idst, hkernel, vkernel );
		else if( _mem->_format.type == IFORMAT_TYPE_UINT8 && idst._mem->_format.type == IFORMAT_TYPE_UINT8 )
			convolveSeperableU8( idst, hkernel, vkernel );
		else if( _mem->_format.type == IFORMAT_TYPE_UINT8 && idst._mem->_format.type == IFORMAT_TYPE_INT16 )
			convolveSeperableU8_to_S16( idst, hkernel, vkernel );
		else {
			throw CVTException("Unimplemented");
		}
	}

	void Image::convolveFloat( Image& idst, const IKernel& kernel ) const
	{
		const float* weights;
		const float* pweights;
		size_t kwidth, kheight;
		const uint8_t* src;
		const uint8_t* osrc;
		const uint8_t* psrc;
		uint8_t* dst;
		uint8_t* odst;
		size_t i, k, b1, b2;
		size_t sstride, dstride;
		void (SIMD::*convfunc)( float* _dst, float const* _src, const size_t width, float const* weights, const size_t wn ) const;
		void (SIMD::*convaddfunc)( float* _dst, float const* _src, const size_t width, float const* weights, const size_t wn ) const;
		SIMD* simd = SIMD::instance();

		if( _mem->_format.channels == 1 ) {
			convfunc = &SIMD::ConvolveClampSet1f;
			convaddfunc = &SIMD::ConvolveClampAdd1f;
		} else if( _mem->_format.channels == 2 ) {
			convfunc = &SIMD::ConvolveClampSet2f;
			convaddfunc = &SIMD::ConvolveClampAdd2f;
		} else {
			convfunc = &SIMD::ConvolveClampSet4f;
			convaddfunc = &SIMD::ConvolveClampAdd4f;
		}

		/* kernel should at least fit once into the image */
		if( _mem->_width < kernel.width() || _mem->_height < kernel.height() ) {
			throw CVTException( "Image smaller than convolution kernel");
		}

		checkFormatAndSize( idst, __PRETTY_FUNCTION__, __LINE__ );

		osrc = src = map( &sstride );
		odst = dst = idst.map( &dstride );

		/* flip and normalize kernel image */
		weights = &kernel( 0, 0 );
		kwidth = kernel.width();
		kheight = kernel.height();

		b1 = ( kernel.height() - ( 1 - ( kernel.height() & 1 ) ) ) / 2;
		b2 = ( kernel.height() + ( 1 - ( kernel.height() & 1 ) ) ) / 2;

		/* upper border */
		i = b1;
		while( i-- ) {
			psrc = src;
			pweights = weights;
			( simd->*convfunc )( ( float* ) dst, ( float* ) src, _mem->_width, pweights, kwidth );
			pweights += kwidth;
			k = i;
			while( k-- ) {
				( simd->*convaddfunc )( ( float* ) dst, ( float* ) psrc, _mem->_width, pweights, kwidth );
				pweights += kwidth;
			}
			k = kheight - ( i + 1 );
			while( k-- ) {
				( simd->*convaddfunc )( ( float* ) dst, ( float* ) psrc, _mem->_width, pweights, kwidth );
				psrc += sstride;
				pweights += kwidth;
			}
			dst += dstride;
		}

		/* center */
		i = _mem->_height - kheight + 1;
		while( i-- ) {
			psrc = src;
			pweights = weights;
			( simd->*convfunc )( ( float* ) dst, ( float* ) psrc, _mem->_width, pweights, kwidth );
			k = kheight - 1;
			while( k-- ) {
				psrc += sstride;
				pweights += kwidth;
				( simd->*convaddfunc )( ( float* ) dst, ( float* ) psrc, _mem->_width, pweights, kwidth );
			}
			dst += dstride;
			src += sstride;
		}

		/* lower border */
		i = b2;
		while( i-- ) {
			psrc = src;
			pweights = weights;
			( simd->*convfunc )( ( float* ) dst, ( float* ) psrc, _mem->_width, pweights, kwidth );
			k = b1 + i;
			while( k-- ) {
				psrc += sstride;
				pweights += kwidth;
				( simd->*convaddfunc )( ( float* ) dst, ( float* ) psrc, _mem->_width, pweights, kwidth );
			}
			k = b2 - i;
			while( k-- ) {
				pweights += kwidth;
				( simd->*convaddfunc )( ( float* ) dst, ( float* ) psrc, _mem->_width, pweights, kwidth );
			}
			dst += dstride;
			src += sstride;
		}

		unmap( osrc );
		idst.unmap( odst );
	}

	void Image::convolveU8( Image& idst, const IKernel& kernel ) const
	{
		Fixed* weights;
		const Fixed* pweights;
		size_t kwidth, kheight;
		const uint8_t* src;
		const uint8_t* osrc;
		const uint8_t* psrc;
		Fixed* buf;
		uint8_t* dst;
		uint8_t* odst;
		size_t i, k, b1, b2, widthchannels;
		size_t sstride, dstride;
		void (SIMD::*convfunc)( Fixed* _dst, const uint8_t* _src, const size_t width, const Fixed* weights, const size_t wn ) const;
		void (SIMD::*convaddfunc)( Fixed* _dst, const uint8_t* _src, const size_t width, const Fixed* weights, const size_t wn ) const;
		SIMD* simd = SIMD::instance();

		if( _mem->_format.channels == 1 ) {
			convfunc = &SIMD::ConvolveClampSet1fx;
			convaddfunc = &SIMD::ConvolveClampAdd1fx;
		} else if( _mem->_format.channels == 2 ) {
			convfunc = &SIMD::ConvolveClampSet2fx;
			convaddfunc = &SIMD::ConvolveClampAdd2fx;
		} else {
			convfunc = &SIMD::ConvolveClampSet4fx;
			convaddfunc = &SIMD::ConvolveClampAdd4fx;
		}

		/* kernel should at least fit once into the image */
		if( _mem->_width < kernel.width() || _mem->_height < kernel.height() ) {
			throw CVTException( "Image smaller than convolution kernel");
		}

		checkFormatAndSize( idst, __PRETTY_FUNCTION__, __LINE__ );

		widthchannels = _mem->_width * _mem->_format.channels;

		// get aligned space for one line in buf
		if( posix_memalign( ( void** ) &buf, 16, sizeof( Fixed ) * widthchannels ) )
			throw CVTException("Out of memory");

		osrc = src = map( &sstride );
		odst = dst = idst.map( &dstride );

		kwidth = kernel.width();
		kheight = kernel.height();
		weights = new Fixed[ kwidth * kheight ];

		for( size_t ky = 0; ky < kheight; ky++ ) {
			for( size_t kx = 0; kx < kwidth; kx++ ) {
				weights[ ky * kwidth + kx ] = kernel( kx, ky );
			}
		}

		b1 = ( kernel.height() - ( 1 - ( kernel.height() & 1 ) ) ) / 2;
		b2 = ( kernel.height() + ( 1 - ( kernel.height() & 1 ) ) ) / 2;

		/* upper border */
		i = b1;
		while( i-- ) {
			psrc = src;
			pweights = weights;
			( simd->*convfunc )( buf, ( uint8_t* ) src, _mem->_width, pweights, kwidth );
			pweights += kwidth;
			k = i;
			while( k-- ) {
				( simd->*convaddfunc )( buf, ( uint8_t* ) psrc, _mem->_width, pweights, kwidth );
				pweights += kwidth;
			}
			k = kheight - ( i + 1 );
			while( k-- ) {
				( simd->*convaddfunc )( buf, ( uint8_t* ) psrc, _mem->_width, pweights, kwidth );
				psrc += sstride;
				pweights += kwidth;
			}
			simd->Conv_fx_to_u8( dst, buf, widthchannels );
			dst += dstride;
		}

		/* center */
		i = _mem->_height - kheight + 1;
		while( i-- ) {
			psrc = src;
			pweights = weights;
			( simd->*convfunc )( buf, ( uint8_t* ) psrc, _mem->_width, pweights, kwidth );
			k = kheight - 1;
			while( k-- ) {
				psrc += sstride;
				pweights += kwidth;
				( simd->*convaddfunc )( buf, ( uint8_t* ) psrc, _mem->_width, pweights, kwidth );
			}
			simd->Conv_fx_to_u8( dst, buf, widthchannels );
			dst += dstride;
			src += sstride;
		}

		/* lower border */
		i = b2;
		while( i-- ) {
			psrc = src;
			pweights = weights;
			( simd->*convfunc )( buf, ( uint8_t* ) psrc, _mem->_width, pweights, kwidth );
			k = b1 + i;
			while( k-- ) {
				psrc += sstride;
				pweights += kwidth;
				( simd->*convaddfunc )( buf, ( uint8_t* ) psrc, _mem->_width, pweights, kwidth );
			}
			k = b2 - i;
			while( k-- ) {
				pweights += kwidth;
				( simd->*convaddfunc )( buf, ( uint8_t* ) psrc, _mem->_width, pweights, kwidth );
			}
			simd->Conv_fx_to_u8( dst, buf, widthchannels );
			dst += dstride;
			src += sstride;
		}

		unmap( osrc );
		idst.unmap( odst );
		free( buf );
		delete[] weights;
	}

	void Image::convolveU8_to_S16( Image& idst, const IKernel& kernel ) const
	{
		Fixed* weights;
		const Fixed* pweights;
		size_t kwidth, kheight;
		const uint8_t* src;
		const uint8_t* osrc;
		const uint8_t* psrc;
		Fixed* buf;
		uint8_t* dst;
		uint8_t* odst;
		size_t i, k, b1, b2, widthchannels;
		size_t sstride, dstride;
		void (SIMD::*convfunc)( Fixed* _dst, const uint8_t* _src, const size_t width, const Fixed* weights, const size_t wn ) const;
		void (SIMD::*convaddfunc)( Fixed* _dst, const uint8_t* _src, const size_t width, const Fixed* weights, const size_t wn ) const;
		SIMD* simd = SIMD::instance();

		if( _mem->_format.channels == 1 ) {
			convfunc = &SIMD::ConvolveClampSet1fx;
			convaddfunc = &SIMD::ConvolveClampAdd1fx;
		} else if( _mem->_format.channels == 2 ) {
			convfunc = &SIMD::ConvolveClampSet2fx;
			convaddfunc = &SIMD::ConvolveClampAdd2fx;
		} else {
			convfunc = &SIMD::ConvolveClampSet4fx;
			convaddfunc = &SIMD::ConvolveClampAdd4fx;
		}

		/* kernel should at least fit once into the image */
		if( _mem->_width < kernel.width() || _mem->_height < kernel.height() ) {
			throw CVTException( "Image smaller than convolution kernel");
		}

		checkFormat( idst, __PRETTY_FUNCTION__, __LINE__, IFormat::int16Equivalent( _mem->_format ) );
		checkSize( idst, __PRETTY_FUNCTION__, __LINE__, _mem->_width, _mem->_height );

		widthchannels = _mem->_width * _mem->_format.channels;
		if( posix_memalign( ( void** ) &buf, 16, sizeof( Fixed ) * widthchannels ) )
			throw CVTException("Out of memory");


		osrc = src = map( &sstride );
		odst = dst = idst.map( &dstride );

		kwidth = kernel.width();
		kheight = kernel.height();
		weights = new Fixed[ kwidth * kheight ];

		for( size_t ky = 0; ky < kheight; ky++ ) {
			for( size_t kx = 0; kx < kwidth; kx++ ) {
				weights[ ky * kwidth + kx ] = kernel( kx, ky );
			}
		}

		b1 = ( kernel.height() - ( 1 - ( kernel.height() & 1 ) ) ) / 2;
		b2 = ( kernel.height() + ( 1 - ( kernel.height() & 1 ) ) ) / 2;

		/* upper border */
		i = b1;
		while( i-- ) {
			psrc = src;
			pweights = weights;
			( simd->*convfunc )( buf, ( uint8_t* ) src, _mem->_width, pweights, kwidth );
			pweights += kwidth;
			k = i;
			while( k-- ) {
				( simd->*convaddfunc )( buf, ( uint8_t* ) psrc, _mem->_width, pweights, kwidth );
				pweights += kwidth;
			}
			k = kheight - ( i + 1 );
			while( k-- ) {
				( simd->*convaddfunc )( buf, ( uint8_t* ) psrc, _mem->_width, pweights, kwidth );
				psrc += sstride;
				pweights += kwidth;
			}
			simd->Conv_fx_to_s16( ( int16_t* ) dst, buf, widthchannels );
			dst += dstride;
		}

		/* center */
		i = _mem->_height - kheight + 1;
		while( i-- ) {
			psrc = src;
			pweights = weights;
			( simd->*convfunc )( buf, ( uint8_t* ) psrc, _mem->_width, pweights, kwidth );
			k = kheight - 1;
			while( k-- ) {
				psrc += sstride;
				pweights += kwidth;
				( simd->*convaddfunc )( buf, ( uint8_t* ) psrc, _mem->_width, pweights, kwidth );
			}
			simd->Conv_fx_to_s16( ( int16_t* ) dst, buf, widthchannels );
			dst += dstride;
			src += sstride;
		}

		/* lower border */
		i = b2;
		while( i-- ) {
			psrc = src;
			pweights = weights;
			( simd->*convfunc )( buf, ( uint8_t* ) psrc, _mem->_width, pweights, kwidth );
			k = b1 + i;
			while( k-- ) {
				psrc += sstride;
				pweights += kwidth;
				( simd->*convaddfunc )( buf, ( uint8_t* ) psrc, _mem->_width, pweights, kwidth );
			}
			k = b2 - i;
			while( k-- ) {
				pweights += kwidth;
				( simd->*convaddfunc )( buf, ( uint8_t* ) psrc, _mem->_width, pweights, kwidth );
			}
			simd->Conv_fx_to_s16( ( int16_t* ) dst, buf, widthchannels );
			dst += dstride;
			src += sstride;
		}

		unmap( osrc );
		idst.unmap( odst );
		delete[] buf;
		delete[] weights;
	}

	void Image::convolveSeperableU8( Image& idst, const IKernel& hkernel, const IKernel& vkernel ) const
	{
		Fixed* hweights;
		Fixed* vweights;
		Fixed* pweights;
		size_t kwidth, kheight;
		const uint8_t* src;
		const uint8_t* osrc;
		Fixed** buf;
		Fixed* bufmem;
		Fixed* accumBuf;
		size_t curbuf;
		uint8_t* dst;
		uint8_t* odst;
		size_t i, k, b1, b2, widthchannels;
		size_t sstride, dstride;
		void (SIMD::*convfunc)( Fixed* _dst, const uint8_t* _src, const size_t width, const Fixed* weights, const size_t wn ) const;
		SIMD* simd = SIMD::instance();

		if( _mem->_format.channels == 1 ) {
			convfunc = &SIMD::ConvolveClampSet1fx;
		} else if( _mem->_format.channels == 2 ) {
			convfunc = &SIMD::ConvolveClampSet2fx;
		} else {
			convfunc = &SIMD::ConvolveClampSet4fx;
		}

		/* kernel should at least fit once into the image */
		if( _mem->_width < hkernel.width() || _mem->_height < vkernel.height() ) {
			throw CVTException( "Image smaller than convolution kernel");
		}

		checkFormatAndSize( idst, __PRETTY_FUNCTION__, __LINE__ );


		osrc = src = map( &sstride );
		odst = dst = idst.map( &dstride );

		kwidth = hkernel.width();
		kheight = vkernel.height();
		hweights = new Fixed[ kwidth ];
		vweights = new Fixed[ kheight ];

		for( size_t kx = 0; kx < kwidth; kx++ ) {
			hweights[ kx ] = hkernel( kx, 0 );
		}

		for( size_t ky = 0; ky < kheight; ky++ ) {
			vweights[ ky ] = vkernel( 0, ky );
		}

		widthchannels = _mem->_width * _mem->_format.channels;
		buf = new Fixed*[ kheight ];

		// allocate and fill buffer
		size_t bstride = Math::pad16( sizeof( Fixed ) * widthchannels );
		if( posix_memalign( ( void** ) &bufmem, 16, bstride * kheight ) ) {
			delete[] hweights;
			delete[] vweights;
			delete[] buf;
			throw CVTException("Out of memory");
		}

		buf[ 0 ] = bufmem;
		for( i = 0; i < kheight; i++ ) {
			if( i != 0 )
				buf[ i ] = ( Fixed* ) ( ( uint8_t* )buf[ i - 1 ] + bstride );
			( simd->*convfunc )( buf[ i ], ( uint8_t* ) src, _mem->_width, hweights, kwidth );
			src += sstride;
		}
		if( posix_memalign( ( void** ) &accumBuf, 16, sizeof( Fixed ) * ( widthchannels ) ) ) {
			delete[] hweights;
			delete[] vweights;
			delete[] buf;
			free( bufmem );
			throw CVTException("Out of memory");
		}


		b1 = ( kheight - ( 1 - ( kheight & 1 ) ) ) / 2;
		b2 = ( kheight + ( 1 - ( kheight & 1 ) ) ) / 2;

		/* upper border */
		i = b1;
		while( i-- ) {
			pweights = vweights;
			simd->MulValue1fx( accumBuf, buf[ 0 ], *pweights++, widthchannels );
			k = i;
			while( k-- ) {
				simd->MulAddValue1fx( accumBuf, buf[ 0 ], *pweights++, widthchannels );
			}
			k = kheight - ( i + 1 );
			curbuf = 0;
			while( k-- ) {
				simd->MulAddValue1fx( accumBuf, buf[ curbuf++ ], *pweights++, widthchannels );
			}
			simd->Conv_fx_to_u8( dst, accumBuf, widthchannels );
			dst += dstride;
		}

		/* center */
		i = _mem->_height - kheight + 1;
		while( i-- ) {

			simd->ConvolveClampVert_fx_to_u8( dst, ( const Fixed** ) buf, vweights, kheight, widthchannels );

			if( i != 0 ) {
				Fixed* tmp = buf[ 0 ];
				for( size_t k = 0; k < kheight - 1; k++ )
					buf[ k ] = buf[ k + 1 ];
				buf[ kheight - 1 ] = tmp;
				( simd->*convfunc )( tmp, ( uint8_t* ) src, _mem->_width, hweights, kwidth );
			}

			dst += dstride;
			src += sstride;
		}

		/* lower border */
		curbuf = 0;
		i = b2;
		while( i-- ) {
			pweights = vweights;
			simd->MulValue1fx( accumBuf, buf[ curbuf ], *pweights++, widthchannels );
			for( k = 1; k < b1 + i + 1; k++ ) {
				simd->MulAddValue1fx( accumBuf, buf[ ( curbuf + k ) % kheight ], *pweights++, widthchannels );
			}
			k = b2 - i;
			while( k-- ) {
				simd->MulAddValue1fx( accumBuf, buf[ ( curbuf + b1 + i ) % kheight ], *pweights++, widthchannels );
			}
			simd->Conv_fx_to_u8( dst, accumBuf, widthchannels );
			curbuf = ( curbuf + 1 ) % kheight;
			dst += dstride;
		}

		unmap( osrc );
		idst.unmap( odst );
		free( accumBuf );
		free( bufmem );
		delete[] buf;
		delete[] vweights;
		delete[] hweights;
	}


	void Image::convolveSeperableU8_to_S16( Image& idst, const IKernel& hkernel, const IKernel& vkernel ) const
	{
		Fixed* hweights;
		Fixed* vweights;
		Fixed* pweights;
		size_t kwidth, kheight;
		const uint8_t* src;
		const uint8_t* osrc;
		Fixed** buf;
		Fixed* bufmem;
		Fixed* accumBuf;
		size_t curbuf;
		uint8_t* dst;
		uint8_t* odst;
		size_t i, k, b1, b2, widthchannels;
		size_t sstride, dstride;
		void (SIMD::*convfunc)( Fixed* _dst, const uint8_t* _src, const size_t width, const Fixed* weights, const size_t wn ) const;
		SIMD* simd = SIMD::instance();

		if( _mem->_format.channels == 1 ) {
			convfunc = &SIMD::ConvolveClampSet1fx;
		} else if( _mem->_format.channels == 2 ) {
			convfunc = &SIMD::ConvolveClampSet2fx;
		} else {
			convfunc = &SIMD::ConvolveClampSet4fx;
		}

		// kernel should at least fit once into the image
		if( _mem->_width < hkernel.width() || _mem->_height < vkernel.height() ) {
			throw CVTException( "Image smaller than convolution kernel");
		}

		if( _mem->_format.channels != idst.format().channels ){
			throw CVTException( "src and dst image number of channels differ" );
		}

		osrc = src = map( &sstride );
		odst = dst = idst.map( &dstride );

		kwidth = hkernel.width();
		kheight = vkernel.height();
		hweights = new Fixed[ kwidth ];
		vweights = new Fixed[ kheight ];

		for( size_t kx = 0; kx < kwidth; kx++ ) {
			hweights[ kx ] = hkernel( kx, 0 );
		}

		for( size_t ky = 0; ky < kheight; ky++ ) {
			vweights[ ky ] = vkernel( 0, ky );
		}

		// number of data elements per line
		widthchannels = _mem->_width * _mem->_format.channels;
		buf = new Fixed*[ kheight ];

		// allocate and fill buffer
		size_t bstride = Math::pad16( sizeof( Fixed ) * widthchannels );
		if( posix_memalign( ( void** ) &bufmem, 16, bstride * kheight ) ) {
			delete[] hweights;
			delete[] vweights;
			delete[] buf;
			throw CVTException("Out of memory");
		}

		buf[ 0 ] = bufmem;
		for( i = 0; i < kheight; i++ ) {
			if( i != 0 )
				buf[ i ] = ( Fixed* ) ( ( uint8_t* )buf[ i - 1 ] + bstride );
			( simd->*convfunc )( buf[ i ], ( uint8_t* ) src, _mem->_width, hweights, kwidth );
			src += sstride;
		}
		if( posix_memalign( ( void** ) &accumBuf, 16, sizeof( Fixed ) * ( widthchannels ) ) ) {
			delete[] hweights;
			delete[] vweights;
			delete[] buf;
			free( bufmem );
			throw CVTException("Out of memory");
		}


		b1 = ( kheight - ( 1 - ( kheight & 1 ) ) ) / 2;
		b2 = ( kheight + ( 1 - ( kheight & 1 ) ) ) / 2;

		// upper border
		i = b1;
		while( i-- ) {
			pweights = vweights;
			simd->MulValue1fx( accumBuf, buf[ 0 ], *pweights++, widthchannels );
			k = i;
			while( k-- ) {
				simd->MulAddValue1fx( accumBuf, buf[ 0 ], *pweights++, widthchannels );
			}
			k = kheight - ( i + 1 );
			curbuf = 0;
			while( k-- ) {
				simd->MulAddValue1fx( accumBuf, buf[ curbuf++ ], *pweights++, widthchannels );
			}
			simd->Conv_fx_to_s16( ( int16_t* )dst, accumBuf, widthchannels );
			dst += dstride;
		}

		// center
		i = _mem->_height - kheight + 1;
		while( i-- ) {
			simd->ConvolveClampVert_fx_to_s16( ( int16_t* ) dst, ( const Fixed** ) buf, vweights, kheight, widthchannels );

			if( i != 0 ) {
				Fixed* tmp = buf[ 0 ];
				for( size_t k = 0; k < kheight - 1; k++ )
					buf[ k ] = buf[ k + 1 ];
				buf[ kheight - 1 ] = tmp;
				( simd->*convfunc )( tmp, ( uint8_t* ) src, _mem->_width, hweights, kwidth );
			}

			dst += dstride;
			src += sstride;
		}

		// lower border
		curbuf = 0;
		i = b2;
		while( i-- ) {
			pweights = vweights;
			simd->MulValue1fx( accumBuf, buf[ curbuf ], *pweights++, widthchannels );
			for( k = 1; k < b1 + i + 1; k++ ) {
				simd->MulAddValue1fx( accumBuf, buf[ ( curbuf + k ) % kheight ], *pweights++, widthchannels );
			}
			k = b2 - i;
			while( k-- ) {
				simd->MulAddValue1fx( accumBuf, buf[ ( curbuf + b1 + i ) % kheight ], *pweights++, widthchannels );
			}
			simd->Conv_fx_to_s16( ( int16_t* )dst, accumBuf, widthchannels );
			curbuf = ( curbuf + 1 ) % kheight;
			dst += dstride;
		}

		unmap( osrc );
		idst.unmap( odst );
		free( accumBuf );
		free( bufmem );
		delete[] buf;
		delete[] vweights;
		delete[] hweights;
	}

	void Image::convolveSeperableFloat( Image& idst, const IKernel& hkernel, const IKernel& vkernel ) const
	{
		float* hweights;
		float* vweights;
		float* pweights;
		size_t kwidth, kheight;
		const uint8_t* src;
		const uint8_t* osrc;
		float** buf;
		float* bufmem;
		size_t curbuf;
		uint8_t* dst;
		uint8_t* odst;
		size_t i, k, b1, b2, widthchannels;
		size_t sstride, dstride;
		void (SIMD::*convfunc)( float* _dst, const float* _src, const size_t width, const float* weights, const size_t wn ) const;
		SIMD* simd = SIMD::instance();

		if( _mem->_format.channels == 1 ) {
			convfunc = &SIMD::ConvolveClampSet1f;
		} else if( _mem->_format.channels == 2 ) {
			convfunc = &SIMD::ConvolveClampSet2f;
		} else {
			convfunc = &SIMD::ConvolveClampSet4f;
		}

		/* kernel should at least fit once into the image */
		if( _mem->_width < hkernel.width() || _mem->_height < vkernel.height() ) {
			throw CVTException( "Image smaller than convolution kernel");
		}

		checkFormatAndSize( idst, __PRETTY_FUNCTION__, __LINE__ );


		osrc = src = map( &sstride );
		odst = dst = idst.map( &dstride );

		kwidth = hkernel.width();
		kheight = vkernel.height();
		hweights = new float[ kwidth ];
		vweights = new float[ kheight ];

		for( size_t kx = 0; kx < kwidth; kx++ ) {
			hweights[ kx ] = hkernel( kx, 0 );
		}

		for( size_t ky = 0; ky < kheight; ky++ ) {
			vweights[ ky ] = vkernel( 0, ky );
		}

		widthchannels = _mem->_width * _mem->_format.channels;
		buf = new float*[ kheight ];

		// allocate and fill buffer
		size_t bstride = Math::pad16( sizeof( float ) * widthchannels );
		if( posix_memalign( ( void** ) &bufmem, 16, bstride * kheight ) )
			throw CVTException("Out of memory");

		buf[ 0 ] = bufmem;
		for( i = 0; i < kheight; i++ ) {
			if( i != 0 )
				buf[ i ] = ( float* ) ( ( uint8_t* )buf[ i - 1 ] + bstride );
			( simd->*convfunc )( buf[ i ], ( float* ) src, _mem->_width, hweights, kwidth );
			src += sstride;
		}


		b1 = ( kheight - ( 1 - ( kheight & 1 ) ) ) / 2;
		b2 = ( kheight + ( 1 - ( kheight & 1 ) ) ) / 2;

		/* upper border */
		i = b1;
		while( i-- ) {
			pweights = vweights;
			simd->MulValue1f( ( float* ) dst, buf[ 0 ], *pweights++, widthchannels );
			k = i;
			while( k-- ) {
				simd->MulAddValue1f( ( float* ) dst, buf[ 0 ], *pweights++, widthchannels );
			}
			k = kheight - ( i + 1 );
			curbuf = 0;
			while( k-- ) {
				simd->MulAddValue1f(( float* ) dst, buf[ curbuf++ ], *pweights++, widthchannels );
			}
			dst += dstride;
		}

		/* center */
		i = _mem->_height - kheight + 1;
		while( i-- ) {

			simd->ConvolveClampVert_f(( float* ) dst, ( const float** ) buf, vweights, kheight, widthchannels );

			if( i != 0 ) {
				float* tmp = buf[ 0 ];
				for( size_t k = 0; k < kheight - 1; k++ )
					buf[ k ] = buf[ k + 1 ];
				buf[ kheight - 1 ] = tmp;
				( simd->*convfunc )( tmp, ( float* ) src, _mem->_width, hweights, kwidth );
			}

			dst += dstride;
			src += sstride;
		}

		/* lower border */
		curbuf = 0;
		i = b2;
		while( i-- ) {
			pweights = vweights;
			simd->MulValue1f( ( float* ) dst, buf[ curbuf ], *pweights++, widthchannels );
			for( k = 1; k < b1 + i + 1; k++ ) {
				simd->MulAddValue1f( ( float * ) dst, buf[ ( curbuf + k ) % kheight ], *pweights++, widthchannels );
			}
			k = b2 - i;
			while( k-- ) {
				simd->MulAddValue1f( ( float * ) dst, buf[ ( curbuf + b1 + i ) % kheight ], *pweights++, widthchannels );
			}
			curbuf = ( curbuf + 1 ) % kheight;
			dst += dstride;
		}

		unmap( osrc );
		idst.unmap( odst );
		free( bufmem );
		delete[] buf;
		delete[] vweights;
		delete[] hweights;
	}



	void Image::scale( Image& idst, size_t width, size_t height, const IScaleFilter& filter ) const
	{
		switch ( _mem->_format.type ) {
			case IFORMAT_TYPE_FLOAT:
				scaleFloat( idst, width, height, filter );
				break;
			case IFORMAT_TYPE_UINT8:
				scaleU8( idst, width, height, filter );
				break;
			default:
				throw CVTException("Unimplemented");
				break;
		}
	}

	void Image::scaleFloat( Image& idst, size_t width, size_t height, const IScaleFilter& filter ) const
	{
		IConvolveAdaptivef scalerx;
		IConvolveAdaptivef scalery;
		IConvolveAdaptiveSize* pysw;
		float* pyw;
		const uint8_t* src;
		const uint8_t* osrc;
		const uint8_t* send;
		uint8_t* dst;
		uint8_t* odst;
		size_t sstride, dstride;
		size_t i, l;
		int32_t k;
		float** buf;
		size_t bufsize;
		size_t curbuf;
		void (SIMD::*scalex_func)( float* _dst, float const* _src, const size_t width, IConvolveAdaptivef* conva ) const;
		SIMD* simd = SIMD::instance();


		if( _mem->_format.channels == 1 ) {
			scalex_func = &SIMD::ConvolveAdaptiveClamp1f;
		} else if( _mem->_format.channels == 2 ) {
			scalex_func = &SIMD::ConvolveAdaptiveClamp2f;
		} else {
			scalex_func = &SIMD::ConvolveAdaptiveClamp4f;
		}

		//checkFormat( idst, __PRETTY_FUNCTION__, __LINE__, _mem->_format );
		//checkSize( idst, __PRETTY_FUNCTION__, __LINE__, width, height );
		idst.reallocate( width, height, this->format() );

		osrc = src = map( &sstride );
		odst = dst = idst.map( &dstride );
		send = src + sstride * _mem->_height;

		bufsize = filter.getAdaptiveConvolutionWeights( height, _mem->_height, scalery, true );
		filter.getAdaptiveConvolutionWeights( width, _mem->_width, scalerx, false );

		buf = new float*[ bufsize ];
		/* allocate and fill buffer */
		for( i = 0; i < bufsize; i++ ) {
			if( posix_memalign( ( void** ) &buf[ i ], 16, sizeof( float ) * width * _mem->_format.channels) )
				throw CVTException("Out of memory");
			( simd->*scalex_func )( ( float* ) buf[ i ], ( float* ) src, width, &scalerx );
			src += sstride;
		}
		curbuf = 0;

		pysw = scalery.size;
		pyw = scalery.weights;

		while( height-- ) {
			if( pysw->incr ) {
				for( k = 0; k < pysw->incr && src < send ; k++ ) {
					( simd->*scalex_func )( ( float* ) buf[ ( curbuf + k ) % bufsize ], ( float* ) src, width, &scalerx );
					src += sstride;
				}
				curbuf = ( curbuf + pysw->incr ) % bufsize;
			}

			l = 0;
			while( Math::abs( *pyw ) < Math::EPSILONF ) {
				l++;
				pyw++;
			}
			simd->MulValue1f( ( float* ) dst, buf[ ( curbuf + l ) % bufsize ], *pyw++, width * _mem->_format.channels );
			l++;
			for( ; l < pysw->numw; l++ ) {
				if( Math::abs( *pyw ) > Math::EPSILONF )
					simd->MulAddValue1f( ( float* ) dst, buf[ ( curbuf + l ) % bufsize ], *pyw, width * _mem->_format.channels );
				pyw++;
			}
			pysw++;
			dst += dstride;
		}

		idst.unmap( odst );
		unmap( osrc );

		for( i = 0; i < bufsize; i++ )
			free( buf[ i ] );
		delete[] buf;
		delete[] scalerx.size;
		delete[] scalerx.weights;
		delete[] scalery.size;
		delete[] scalery.weights;
	}

	void Image::scaleU8( Image& idst, size_t width, size_t height, const IScaleFilter& filter ) const
	{
		IConvolveAdaptiveFixed scalerx;
		IConvolveAdaptiveFixed scalery;

		IConvolveAdaptiveSize* pysw;
		Fixed* pyw;

		const uint8_t* src;
		const uint8_t* osrc;
		const uint8_t* send;
		uint8_t* dst;
		uint8_t* odst;
		size_t sstride, dstride;
		size_t i, l;
		int32_t k;
		Fixed** buf;
		Fixed* accumBuf;
		size_t bufsize;
		size_t curbuf;
		void (SIMD::*scalex_func)( Fixed* _dst, uint8_t const* _src, const size_t width, IConvolveAdaptiveFixed* conva ) const;
		SIMD* simd = SIMD::instance();

		if( _mem->_format.channels == 1 ) {
			scalex_func = &SIMD::ConvolveAdaptive1Fixed;
		} else if( _mem->_format.channels == 2 ) {
			scalex_func = &SIMD::ConvolveAdaptive2Fixed;
		} else {
			scalex_func = &SIMD::ConvolveAdaptive4Fixed;
		}

		idst.reallocate( width, height, this->format() );

		osrc = src = map( &sstride );
		odst = dst = idst.map( &dstride );
		send = src + sstride * _mem->_height;

		bufsize = filter.getAdaptiveConvolutionWeights( height, _mem->_height, scalery, true );
		filter.getAdaptiveConvolutionWeights( width, _mem->_width, scalerx, false );

		buf = new Fixed*[ bufsize ];
		// allocate and fill buffer
		for( i = 0; i < bufsize; i++ ) {
			if( posix_memalign( ( void** ) &buf[ i ], 16, sizeof( Fixed ) * width * _mem->_format.channels) )
				throw CVTException("Out of memory");
			( simd->*scalex_func )( buf[ i ], src, width, &scalerx );
			src += sstride;
		}
		curbuf = 0;

		pysw = scalery.size;
		pyw = scalery.weights;

		if( posix_memalign( ( void** ) &accumBuf, 16, sizeof( Fixed ) * width * _mem->_format.channels ) )
			throw CVTException("Out of memory");

		while( height-- ) {
			if( pysw->incr ) {
				for( k = 0; k < pysw->incr && src < send ; k++ ) {
					( simd->*scalex_func )( buf[ ( curbuf + k ) % bufsize ], src, width, &scalerx );
					src += sstride;
				}
				curbuf = ( curbuf + pysw->incr ) % bufsize;
			}

			l = 0;
			while( *pyw == ( Fixed )0.0f ) {
				l++;
				pyw++;
			}
			simd->MulValue1fx( accumBuf, buf[ ( curbuf + l ) % bufsize ], *pyw++, width * _mem->_format.channels );
			l++;
			for( ; l < pysw->numw; l++ ) {
				if( *pyw != ( Fixed )0.0f )
					simd->MulAddValue1fx( accumBuf, buf[ ( curbuf + l ) % bufsize ], *pyw, width * _mem->_format.channels );
				pyw++;
			}

			for( size_t w = 0;  w < width * _mem->_format.channels; w++ ){
				dst[ w ] = Math::clamp( accumBuf[ w ].round(), 0, 255 );
			}

			pysw++;
			dst += dstride;
		}

		idst.unmap( odst );
		unmap( osrc );

		free( accumBuf );
		for( i = 0; i < bufsize; i++ )
			free( buf[ i ] );
		delete[] buf;
		delete[] scalerx.size;
		delete[] scalerx.weights;
		delete[] scalery.size;
		delete[] scalery.weights;
	}

	void Image::warpBilinear( Image& idst, const Image& warp ) const
	{
		size_t m, n, k, K;
		size_t sstride, dstride, wstride;

		if( _mem->_format.type == IFORMAT_TYPE_FLOAT &&
			warp._mem->_format == IFormat::GRAYALPHA_FLOAT ) {
			const uint8_t* src;
			const uint8_t* osrc;
			uint8_t* dst;
			uint8_t* odst;
			const uint8_t* wrp;
			const uint8_t* owrp;
			float* pdst;
			const float* pwrp;
			float data[ 4 ];

			checkFormatAndSize( idst, __PRETTY_FUNCTION__, __LINE__ );

			osrc = src = map( &sstride );
			odst = dst = idst.map( &dstride );
			owrp = wrp = warp.map( &wstride );
			K = channels();

			for( n = 0; n < warp._mem->_height; n++ ) {
				pdst = ( float* ) dst;
				pwrp = ( float* ) wrp;
				for( m = 0; m < warp._mem->_width; m++ ) {
					float x, y, alpha, beta;
					size_t ix[ 2 ], iy[ 2 ];
					x = *pwrp++;
					y = *pwrp++;
					alpha = x - Math::floor( x );
					beta  = y - Math::floor( y );
					ix[ 0 ] = ( size_t ) Math::clamp( ( float ) m + x, 0.0f, ( float ) ( _mem->_width - 1 ) );
					iy[ 0 ] = ( size_t ) Math::clamp( ( float ) n + y, 0.0f, ( float ) ( _mem->_height - 1 ) );
					ix[ 1 ] = Math::min( ix[ 0 ] + 1, _mem->_width - 1 );
					iy[ 1 ] = Math::min( iy[ 0 ] + 1, _mem->_height - 1 );
					for( k = 0; k < K; k++ ) {
						data[ 0 ] = *( ( float* ) ( src + sstride * iy[ 0 ] + ( ix[ 0 ] * K + k ) * sizeof( float ) ) );
						data[ 1 ] = *( ( float* ) ( src + sstride * iy[ 0 ] + ( ix[ 1 ] * K + k ) * sizeof( float ) ) );
						data[ 2 ] = *( ( float* ) ( src + sstride * iy[ 1 ] + ( ix[ 0 ] * K + k ) * sizeof( float ) ) );
						data[ 3 ] = *( ( float* ) ( src + sstride * iy[ 1 ] + ( ix[ 1 ] * K + k ) * sizeof( float ) ) );
						data[ 0 ] = Math::mix( data[ 0 ], data[ 1 ], alpha );
						data[ 2 ] = Math::mix( data[ 2 ], data[ 3 ], alpha );
						*pdst++ = Math::mix( data[ 0 ], data[ 2 ], beta );
					}
				}
				dst += dstride;
				wrp += wstride;
			}
			warp.unmap( owrp );
			idst.unmap( odst );
			unmap( osrc );
		} else
			throw CVTException("Unimplemented");
	}

    void Image::integralImage( Image & dst ) const
    {
        dst.reallocate( this->width(), this->height(), IFormat::floatEquivalent( this->format() ), _mem->type() );

        size_t inStride;
        size_t dstStride;

        float* out = dst.map<float>( &dstStride );
        SIMD* simd = SIMD::instance();

        IFormatID fId = this->format().formatID;

        switch ( fId ) {
            case IFORMAT_GRAY_UINT8:
            {
                const uint8_t* in = this->map<uint8_t>( &inStride );
                simd->prefixSum1_u8_to_f( out, dstStride, in, inStride, width(), height() );
                this->unmap( in );
            }
            break;
            case IFORMAT_GRAY_FLOAT:
            {
                const float* in = this->map<float>( &inStride );
                simd->prefixSum1_f_to_f( out, dstStride, in, inStride, width(), height() );
                this->unmap( in );
            }
            break;
            case IFORMAT_BGRA_UINT8:
            case IFORMAT_RGBA_UINT8:
            {
                const uint8_t* in = this->map<uint8_t > ( &inStride );
                simd->prefixSum1_xxxxu8_to_f( out, dstStride, in, inStride, width( ), height( ) );
                this->unmap( in );
            }
                break;
            default:
                this->unmap( out );
                String msg;
                msg.sprintf( "IntegralImage not implemented for type: %s", fId );
                throw CVTException( msg.c_str() );
        }
        dst.unmap( out );
    }

    void Image::squaredIntegralImage( Image & dst ) const
    {
        dst.reallocate( this->width(), this->height(), IFormat::floatEquivalent( this->format() ), _mem->type() );

        size_t inStride;
        size_t dstStride;

        float * out = dst.map<float>( &dstStride );
        SIMD * simd = SIMD::instance();

        IFormatID fId = this->format().formatID;

        switch ( fId ) {
            case IFORMAT_GRAY_UINT8:
            {
                const uint8_t * in = this->map<uint8_t>( &inStride );
                simd->prefixSumSqr1_u8_to_f( out, dstStride, in, inStride, width(), height() );
                this->unmap( in );
            }
            break;
            case IFORMAT_GRAY_FLOAT:
            {
                const float * in = this->map<float>( &inStride );
                simd->prefixSumSqr1_f_to_f( out, dstStride, in, inStride, width(), height() );
                this->unmap( in );
            }
            break;

            default:
                this->unmap( out );
                String msg;
                msg.sprintf( "Squared integralImage not implemented for type: %s", fId );
                throw CVTException( msg.c_str() );
        }
        dst.unmap( out );
    }


	void Image::pyrdown( Image& dst ) const
	{
		dst.reallocate( width() / 2, height() / 2, format(), _mem->type() );

		IFormatID fId = this->format().formatID;
		switch( fId ) {
			case IFORMAT_GRAY_UINT8: return pyrdown1U8( dst );
			default:
				String msg;
				msg.sprintf( "Pyrdown not implemented for type: %s", fId );
				throw CVTException( msg.c_str() );
		}
	}

	void Image::pyrdown1U8( Image& out ) const
	{
		size_t bstride = Math::pad16( out.width() );
		uint16_t* buf;
		size_t sstride, dstride;
		const uint8_t* src = map( &sstride );
		uint8_t* dst = out.map( &dstride );
		const uint8_t* psrc = src;
		uint8_t* pdst = dst;
		uint16_t* rows[ 5 ];

		SIMD* simd = SIMD::instance();

		ScopedBuffer<uint16_t, true> scopebuf( bstride * 5 );
		buf = scopebuf.ptr();
//		if( posix_memalign( ( void** ) &buf, 16, sizeof( uint16_t ) * bstride * 5 ) )
//			throw CVTException("Out of memory");

		simd->pyrdownHalfHorizontal_1u8_to_1u16( buf, psrc, width() );
		psrc += sstride;
		simd->pyrdownHalfHorizontal_1u8_to_1u16( buf + bstride, psrc, width() );
		psrc += sstride;
		simd->pyrdownHalfHorizontal_1u8_to_1u16( buf + 2 * bstride, psrc, width() );
		psrc += sstride;
		simd->pyrdownHalfHorizontal_1u8_to_1u16( buf + 3 * bstride, psrc, width() );
		psrc += sstride;

		rows[ 0 ] = buf;
		rows[ 1 ] = buf;
		rows[ 2 ] = buf + bstride;
		rows[ 3 ] = buf + 2 * bstride;
		rows[ 4 ] = buf + 3 * bstride;

		simd->pyrdownHalfVertical_1u16_to_1u8( pdst, rows, out.width() );
		pdst += dstride;

		rows[ 0 ] = buf + 4 * bstride;
		size_t h = out.height() - 2;
		while( h-- ) {
			uint16_t* tmp1 = rows[ 0 ];
			simd->pyrdownHalfHorizontal_1u8_to_1u16( tmp1, psrc, width() );
			psrc += sstride;
			uint16_t* tmp2 = rows[ 1 ];
			simd->pyrdownHalfHorizontal_1u8_to_1u16( tmp2, psrc, width() );
			psrc += sstride;

			rows[ 0 ] = rows[ 2 ];
			rows[ 1 ] = rows[ 3 ];
			rows[ 2 ] = rows[ 4 ];
			rows[ 3 ] = tmp1;
			rows[ 4 ] = tmp2;

			simd->pyrdownHalfVertical_1u16_to_1u8( pdst, rows, out.width() );
			pdst += dstride;
		}

		uint16_t* tmp1 = rows[ 0 ];
		if( height() & 1 )
			simd->pyrdownHalfHorizontal_1u8_to_1u16( tmp1, psrc, width() );
		else
			tmp1 = rows[ 4 ];

		rows[ 0 ] = rows[ 2 ];
		rows[ 1 ] = rows[ 3 ];
		rows[ 2 ] = rows[ 4 ];
		rows[ 3 ] = tmp1;
		rows[ 4 ] = tmp1;
		simd->pyrdownHalfVertical_1u16_to_1u8( pdst, rows, out.width() );

		unmap( src );
		out.unmap( dst );
	}


	void Image::printValues( std::ostream& o, const Recti& _rect ) const
	{
		size_t w, h, stride;

		Recti rect( _rect );
		rect.intersect( this->rect() );


		if( rect.isEmpty() )
			return;

		switch ( _mem->_format.formatID ) {
			case IFORMAT_GRAY_UINT8:
			case IFORMAT_BAYER_RGGB_UINT8:
				{
					const uint8_t* ptr, *base;
					base = ptr = map( &stride );
					ptr += rect.y * stride + rect.x;
					h = rect.height;
					while( h-- ) {
						w = rect.width;
						for( size_t x = 0; x < w; x++ )
							o << "0x" << std::setw( 2 ) << std::hex << *( ptr + x ) << " ";
						o << "\n";
						ptr += stride;
					}
					unmap( base );
				}
				break;
			case IFORMAT_GRAY_FLOAT:
				{
					const uint8_t* ptr, *base;
					base = ptr = map( &stride );
					ptr += rect.y * stride + rect.x * sizeof( float );
					h = rect.height;
					while( h-- ) {
						w = rect.width;
						for( size_t x = 0; x < w; x++ )
							o << std::setiosflags( std::ios::fixed ) << std::setprecision( 2 ) << *( ( ( float* ) ptr ) + x ) << " ";
						o << "\n";
						ptr += stride;
					}
					unmap( base );
				}
				break;
			case IFORMAT_RGBA_UINT8:
			case IFORMAT_BGRA_UINT8:
				{
					const uint8_t* ptr, *base;
					base = ptr = map( &stride );
					ptr += rect.y * stride + rect.x * 4;
					h = rect.height;
					while( h-- ) {
						w = rect.width * 4;
						for( size_t x = 0; x < w; x++ )
							o << "0x" << std::setw( 2 ) << std::hex << *( ptr + x ) << " ";
						o << "\n";
						ptr += stride;
					}
					unmap( base );
				}
				break;
			case IFORMAT_RGBA_FLOAT:
			case IFORMAT_BGRA_FLOAT:
				{
					const uint8_t* ptr, *base;
					base = ptr = map( &stride );
					ptr += rect.y * stride + rect.x * sizeof( float ) * 4;
					h = rect.height;
					while( h-- ) {
						w = rect.width * 4;
						for( size_t x = 0; x < w; x++ )
							o << std::setiosflags( std::ios::fixed ) << std::setprecision( 2 ) << *( ( ( float* ) ptr ) + x ) << " ";
						o << "\n";
						ptr += stride;
					}
					unmap( base );
				}
				break;
			default:
				throw CVTException("Unimplemented");
				break;
		}
	}
}
