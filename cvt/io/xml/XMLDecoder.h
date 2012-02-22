/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef CVT_XMLDECODER_H
#define CVT_XMLDECODER_H

#include <stdlib.h>

namespace cvt {
	class XMLNode;
	class XMLDocument;

	/**
	  \ingroup XML
	*/
	class XMLDecoder {
		friend class XMLDocument;

		public:
			XMLDecoder() {};
			virtual ~XMLDecoder() {};

			static XMLDecoder* autodetect( const void* data, size_t len );

		protected:
			void parse( XMLDocument* doc, const void* data, size_t len );

			virtual void setData( const void* data, size_t len ) = 0;
			virtual XMLNode* parseDeclaration() = 0;
			virtual XMLNode* parseNode() = 0;

		private:
			XMLDecoder( const XMLDecoder& );
			XMLDecoder& operator=( const XMLDecoder& );
	};
}

#endif
