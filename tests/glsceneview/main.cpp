#include <cvt/gui/Window.h>
#include <cvt/gui/WidgetLayout.h>
#include <cvt/gui/Application.h>

#include <cvt/geom/scene/Scene.h>

#include "GLSceneView.h"

using namespace cvt;

int main( int argc, char** argv )
{
	if( argc != 3 ){
		std::cout << "Usage: " << argv[ 0 ] << " <scenefile> <geometry-index>" << std::endl;
		return 0;
	}

	Scene s;
	s.load( argv[ 1 ] );
	size_t index = String( argv[ 2 ] ).toInteger();
	std::cout << index << std::endl;
	if( index >= s.geometrySize() ) {
		std::cout << "No such mesh" << std::endl;
		return 0;
	}


	Window w( "meshView" );
	w.setSize( 640, 480 );
	SceneMesh* mesh = ( ( SceneMesh* ) s.geometry( index ) );
	mesh->translate( -mesh->centroid() );
	Matrix4f r;
	r.setRotationY( Math::deg2Rad( 45.0f ) );
	mesh->transform( r );
//	mesh->calculateNormals();
	mesh->scale( 0.001f );
	GLSceneView view( *( ( SceneMesh* ) s.geometry( index ) ) );

	WidgetLayout wl;
	wl.setAnchoredTopBottom( 0, 0 );
	wl.setAnchoredLeftRight( 0, 0 );
	w.addWidget( &view, wl );
	w.setVisible( true );

	w.update();

	Application::run();

	return 0;

}