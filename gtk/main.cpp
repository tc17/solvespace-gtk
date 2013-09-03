#include <stdexcept>
#include <gtkmm.h>
#include <gdk/gdkx.h>
#include <GL/glx.h>
//#include <gtkmm/drawingarea.h>
#include "solvespace.h"

class Glx : public Gtk::Widget
{
	Display* _xdisplay;
	XVisualInfo* _xvinfo;
	GLXContext _context;
	Glib::RefPtr<Gdk::Window> _window;
public:
	Glx();
	virtual ~Glx();

protected:
	virtual void on_realize();
	virtual bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr);
};

Glx::Glx()
{
	set_has_window(false);

	_xdisplay = gdk_x11_get_default_xdisplay();
	if (!glXQueryExtension(_xdisplay, NULL, NULL))
		throw std::runtime_error("OpenGL not supported");

	int attrlist[] = {
		GLX_RGBA,
		GLX_RED_SIZE, 1,
		GLX_GREEN_SIZE, 1,
		GLX_BLUE_SIZE, 1,
		GLX_DOUBLEBUFFER, 0,
		None
	};

	_xvinfo = glXChooseVisual(_xdisplay, gdk_x11_get_default_screen(), attrlist);
	if (!_xvinfo)
		throw std::runtime_error("Error configuring OpenGL");

	_context = glXCreateContext(_xdisplay, _xvinfo, NULL, True);
}

Glx::~Glx()
{
	glXDestroyContext(_xdisplay, _context);
}

void Glx::on_realize()
{
	Gtk::Widget::on_realize();

	if (!_window) {
		GdkWindowAttr attr;

		Gtk::Allocation allocation = get_allocation();

		attr.x = allocation.get_x();
		attr.y = allocation.get_y();
		attr.width = allocation.get_width();
		attr.height = allocation.get_height();

		attr.event_mask = get_events() | Gdk::EXPOSURE_MASK;
		attr.window_type = GDK_WINDOW_CHILD;
		attr.wclass = GDK_INPUT_OUTPUT;

		_window = Gdk::Window::create(get_window(), &attr, GDK_WA_X | GDK_WA_Y);

		set_has_window(true);
		set_window(_window);

		unset_background_color();
		set_double_buffered(false);

		// make the window recieve expose events
		_window->set_user_data(gobj());

	}
}

bool Glx::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
{
	XID xid = gdk_x11_window_get_xid(get_window()->gobj());
	if (!glXMakeCurrent(_xdisplay, xid, _context))
		return false;
#if 0
	glClear(GL_COLOR_BUFFER_BIT);
        glBegin( GL_TRIANGLES );
            glIndexi( 0 );
            glColor3f( 1.0f, 0.0f, 0.0f );
            glVertex2i( 0, 1 );
            glIndexi( 0 );
            glColor3f( 0.0f, 1.0f, 0.0f );
            glVertex2i( -1, -1 );
            glIndexi( 0 );
            glColor3f( 0.0f, 0.0f, 1.0f );
            glVertex2i( 1, -1 );
        glEnd( );
#endif
	SS.GW.Paint();
	glXSwapBuffers(_xdisplay, xid);

	return true;
}


int main(int argc, char *argv[])
{
	Glib::RefPtr<Gtk::Application> app =
		Gtk::Application::create(argc, argv, "org.solvespace");

	Gtk::Window window;
	
	Glx glx;
	window.add(glx);
	glx.show();

	SS.Init(NULL);

	return app->run(window);
}
