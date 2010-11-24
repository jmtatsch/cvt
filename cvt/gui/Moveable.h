#ifndef CVT_MOVEABLE_H
#define CVT_MOVEABLE_H

#include <cvt/gui/WidgetContainer.h>
#include <cvt/gui/ToggleButton.h>
#include <string>

namespace cvt {
	class Moveable : public WidgetContainer
	{
		public:
			Moveable( Widget* child );
			~Moveable();

			Widget* child();
			void setTitle( const std::string& title );

		private:
			void paintEvent( PaintEvent* ev, GFX* g );
			void resizeEvent( ResizeEvent* ev );
			void mousePressEvent( MousePressEvent* ev );
			void mouseMoveEvent( MouseMoveEvent* ev );
			void mouseReleaseEvent( MouseReleaseEvent* ev );

			void onToggle( ToggleButton* button );

			Widget* _child;
			Widget* _activeWidget;
			int _activeMode;
			int _lx, _ly;
			int _oldheight;
			std::string _title;
			ToggleButton _togglebutton;
			Delegate<void ( ToggleButton* )> _onToggleDelegate;
	};

	inline Widget* Moveable::child()
	{
		return _child;
	}
}

#endif
