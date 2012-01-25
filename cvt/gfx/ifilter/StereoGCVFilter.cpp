#include <cvt/gfx/ifilter/StereoGCVFilter.h>
#include <cvt/cl/kernel/stereogcv/costdepth.h>
#include <cvt/cl/kernel/stereogcv/costdepthgrad.h>
#include <cvt/cl/kernel/stereogcv/costmin.h>
#include <cvt/cl/kernel/stereogcv/costdepthconv.h>
#include <cvt/cl/kernel/guidedfilter/guidedfilter_calcab_outerrgb.h>
#include <cvt/cl/kernel/guidedfilter/guidedfilter_applyab_gc_outer.h>
#include <cvt/cl/kernel/fill.h>
#include <cvt/cl/kernel/gradx.h>

namespace cvt {
	static ParamInfoTyped<Image*> pin0( "Input 0", true );
	static ParamInfoTyped<Image*> pin1( "Input 1", true );
	static ParamInfoTyped<Image*> pout( "Output", false );
	static ParamInfoTyped<float>  pdmin( "Depth min.", true );
	static ParamInfoTyped<float>  pdmax( "Depth max.", true );
	static ParamInfoTyped<float>  pdt( "Depth stepsize", true );

	static ParamInfo * _params[ 6 ] = {
		&pin0,
		&pin1,
		&pout,
		&pdmin,
		&pdmax,
		&pdt
	};

	StereoGCVFilter::StereoGCVFilter() :
		IFilter( "StereoGCVFilter", _params, 6, IFILTER_CPU | IFILTER_OPENCL ),
		_cldepthcost( _costdepth_source, "stereogcv_costdepth" ),
		_cldepthcostgrad( _costdepthgrad_source, "stereogcv_costdepthgrad" ),
		_cldepthmin( _costmin_source, "stereogcv_costmin" ),
		_clfill( _fill_source, "fill" ),
		_clcdconv( _costdepthconv_source, "stereogcv_costdepthconv" ),
		_clgrad( _gradx_source, "gradx" ),
		_clguidedfilter_calcab_outerrgb( _guidedfilter_calcab_outerrgb_source, "guidedfilter_calcab_outerrgb" ),
		_clguidedfilter_applyab_gc_outer( _guidedfilter_applyab_gc_outer_source, "guidedfilter_applyab_gc_outer" )
	{
	}


	void StereoGCVFilter::apply( Image& dst, const Image& cam0, const Image& cam1, float dmin, float dmax, float dt ) const
	{
#define RADIUS 9
#define EPSILON 1e-4f
		// StereoGCV
		Image cost( cam0.width(), cam0.height(), IFormat::GRAY_FLOAT, IALLOCATOR_CL ); //FIXME: just use GRAYALPHA
		Image costgf( cam0.width(), cam0.height(), IFormat::GRAY_FLOAT, IALLOCATOR_CL ); //FIXME: just use GRAYALPHA
		Image c0( cam0.width(), cam0.height(), IFormat::GRAYALPHA_FLOAT, IALLOCATOR_CL ); //FIXME: just use GRAYALPHA
		Image c1( cam0.width(), cam0.height(), IFormat::GRAYALPHA_FLOAT, IALLOCATOR_CL ); //FIXME: just use GRAYALPHA
		Image g0( cam0.width(), cam0.height(), IFormat::RGBA_FLOAT, IALLOCATOR_CL );
		Image g1( cam0.width(), cam0.height(), IFormat::RGBA_FLOAT, IALLOCATOR_CL );
		// Guided filter stuff
		Image ia( cam0.width(), cam0.height(), IFormat::RGBA_FLOAT, IALLOCATOR_CL );
		Image ib( cam0.width(), cam0.height(), IFormat::RGBA_FLOAT, IALLOCATOR_CL );
		Image iint( cam0.width(), cam0.height(), IFormat::RGBA_FLOAT, IALLOCATOR_CL );
		Image iint2( cam0.width(), cam0.height(), IFormat::RGBA_FLOAT, IALLOCATOR_CL );
		Image imeanG( cam0.width(), cam0.height(), IFormat::RGBA_FLOAT, IALLOCATOR_CL );
		Image imeanS( cam0.width(), cam0.height(), IFormat::GRAY_FLOAT, IALLOCATOR_CL );
		Image imeanGS( cam0.width(), cam0.height(), IFormat::RGBA_FLOAT, IALLOCATOR_CL );
		Image imean_RR_RG_RB( cam0.width(), cam0.height(), IFormat::RGBA_FLOAT, IALLOCATOR_CL );
		Image imean_GG_GB_BB( cam0.width(), cam0.height(), IFormat::RGBA_FLOAT, IALLOCATOR_CL );


		Image* c[ 2 ] = { &c0, &c1 };
		int index = 0;
		float fill[ 4 ] = { 1.0f, 0.0f, 0.0f, 0.0f };

		CLNDRange global( Math::pad16( cam0.width() ), Math::pad16( cam0.height() ) );

		_clfill.setArg( 0, *c[ !index ] );
		_clfill.setArg( 1, sizeof( cl_float ) * 4, fill );
		_clfill.run( global, CLNDRange( 16, 16 ) );

		_clgrad.setArg( 0, g0 );
		_clgrad.setArg( 1, cam0 );
		_clgrad.setArg( 2, CLLocalSpace( sizeof( cl_float4 ) * 18 * 16 ) );
		_clgrad.run( global, CLNDRange( 16, 16 ) );

		_clgrad.setArg( 0, g1 );
		_clgrad.setArg( 1, cam1 );
		_clgrad.setArg( 2, CLLocalSpace( sizeof( cl_float4 ) * 18 * 16 ) );
		_clgrad.run( global, CLNDRange( 16, 16 ) );

		// Guided filter, same for all cost slices
		_intfilter.apply( iint, cam1 );
		_boxfilter.apply( imeanG, iint, RADIUS );
		_intfilter.applyOuterRGB( iint, iint2, cam1 );
		_boxfilter.apply( imean_RR_RG_RB, iint, RADIUS );
		_boxfilter.apply( imean_GG_GB_BB, iint2, RADIUS );

		if( dmax < dmin && dt > 0 ) dt = -dt;
		size_t n = Math::abs( dmax - dmin ) / Math::abs( dt );
		for( float d = dmin; n--; d += dt ) {
			_cldepthcostgrad.setArg( 0, cost );
			_cldepthcostgrad.setArg( 1, cam1 );
			_cldepthcostgrad.setArg( 2, cam0 );
			_cldepthcostgrad.setArg( 3, g1 );
			_cldepthcostgrad.setArg( 4, g0 );
			_cldepthcostgrad.setArg( 5, d );
			_cldepthcostgrad.run( global, CLNDRange( 16, 16 ) );

			// Guided filter, dependent on current cost slice
//			_gf.apply( costgf, cost, cam1, 9, 1e-4f, true );

			_intfilter.apply( iint, cost );
			_boxfilter.apply( imeanS, iint, 8 );
			_intfilter.apply( iint, cam1, &cost );
			_boxfilter.apply( imeanGS, iint, RADIUS );

			_clguidedfilter_calcab_outerrgb.setArg( 0, ia );
			_clguidedfilter_calcab_outerrgb.setArg( 1, ib );
			_clguidedfilter_calcab_outerrgb.setArg( 2, imeanG );
			_clguidedfilter_calcab_outerrgb.setArg( 3, imeanS );
			_clguidedfilter_calcab_outerrgb.setArg( 4, imeanGS );
			_clguidedfilter_calcab_outerrgb.setArg( 5, imean_RR_RG_RB );
			_clguidedfilter_calcab_outerrgb.setArg( 6, imean_GG_GB_BB );
			_clguidedfilter_calcab_outerrgb.setArg( 7, EPSILON );
			_clguidedfilter_calcab_outerrgb.run( global, CLNDRange( 16, 16 ) );

			_intfilter.apply( iint, ia );
			_boxfilter.apply( ia, iint, RADIUS );
			_intfilter.apply( iint, ib );
			_boxfilter.apply( ib, iint, RADIUS );

			_clguidedfilter_applyab_gc_outer.setArg( 0, costgf );
			_clguidedfilter_applyab_gc_outer.setArg( 1, cam1 );
			_clguidedfilter_applyab_gc_outer.setArg( 2, ia );
			_clguidedfilter_applyab_gc_outer.setArg( 3, ib );
			_clguidedfilter_applyab_gc_outer.run( global, CLNDRange( 16, 16 ) );

			_cldepthmin.setArg( 0, *c[ index ] );
			_cldepthmin.setArg( 1, costgf );
			_cldepthmin.setArg( 2, *c[ !index ] );
			_cldepthmin.setArg( 3, Math::abs( d - dmin ) / ( Math::abs( dmax - dmin ) ) );
			_cldepthmin.run( global, CLNDRange( 16, 16 ) );

			index = 1 - index;
		}

		dst.reallocate( cam0.width(), cam0.height(), IFormat::GRAY_UINT8, IALLOCATOR_CL );

		_clcdconv.setArg( 0, dst );
		_clcdconv.setArg( 1, *c[ !index ] );
		_clcdconv.setArg( 2, 1.0f );
		_clcdconv.setArg( 3, 0.0f );
		_clcdconv.run( global, CLNDRange( 16, 16 ) );
	}

}

