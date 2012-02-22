/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef CVT_GLSTRANSFORM_H
#define CVT_GLSTRANSFORM_H

#include <cvt/gl/scene/GLSNode.h>
#include <cvt/math/Matrix.h>

namespace cvt {
	class GLSTransform
	{
		public:
			GLSTransform( const Matrix4f& mat );
			~GLSTransform();

			const Matrix4f& transformation() const;
			void			setTransformation( const Matrix4f& t );

		private:
			Matrix4f	_t;
	};

	inline GLSTransform::GLSTransform( const Matrix4f& mat ) : GLSNode( GLSNODE_TRANSFORMATION ), _t( mat )
	{
	}

	inline GLSTransform::~GLSTransform()
	{
	}

	inline void GLSTransform::setTransformation( const Matrix4f& t )
	{
		_t = t;
	}

	inline const Matrix4f& GLSTransform::transformation() const
	{
		return _t;
	}
}

#endif
