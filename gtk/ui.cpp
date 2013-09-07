#include <string>
#include <assert.h>
#include "solvespace.h"
#include "ui.h"

std::string RecentFile[MAX_RECENT];

Gtk::Widget *textWidget;
Gtk::Widget *graphicsWidget;

Gtk::Window *graphicsWin;
Gtk::Window *textWin;

Glx::Glx(SSWindow& w) : _sswindow(w)
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

		attr.event_mask = get_events()
			| Gdk::EXPOSURE_MASK
			| Gdk::POINTER_MOTION_MASK
			| Gdk::BUTTON_MOTION_MASK
			| Gdk::BUTTON_PRESS_MASK
			| Gdk::BUTTON_RELEASE_MASK
			| Gdk::LEAVE_NOTIFY_MASK;
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
	_sswindow.paint();
	glXSwapBuffers(_xdisplay, xid);

	return true;
}

bool Glx::on_event(GdkEvent *event)
{
	bool rv = true;
	switch (event->type) {
		case GDK_BUTTON_PRESS:
		{
			GdkEventButton *ev = reinterpret_cast<GdkEventButton*>(event);
			Mouse mouse = translate(ev->x, ev->y);
			setButton(&buttonsState, ev->button, true);

			_sswindow.buttonPress(mouse, Button(ev->button));
		}
		break;

		case GDK_BUTTON_RELEASE:
		{
			GdkEventButton *ev = reinterpret_cast<GdkEventButton*>(event);
			Mouse mouse = translate(ev->x, ev->y);
			setButton(&buttonsState, ev->button, false);

			_sswindow.buttonRelease(mouse, Button(ev->button));
		}
		break;

		case GDK_2BUTTON_PRESS:
		{
			GdkEventButton *ev = reinterpret_cast<GdkEventButton*>(event);
			Mouse mouse = translate(ev->x, ev->y);
			setButton(&buttonsState, ev->button, true);
			
			_sswindow.button2Press(mouse, Button(ev->button));
		}
		break;

		case GDK_MOTION_NOTIFY:
		{
			GdkEventMotion *ev = reinterpret_cast<GdkEventMotion*>(event);
			Mouse mouse = translate(ev->x, ev->y);
			ModState mods = getMods(ev->state);

			_sswindow.mouseMoved(mouse, buttonsState, mods);
		}
		break;

		case GDK_LEAVE_NOTIFY:
		{
			_sswindow.mouseLeave();
		}
		break;

		default:
			rv = false;
	}

	return rv;
}

Mouse Glx::translate(double x, double y)
{
	Mouse rv;

	rv.x = x - get_allocated_width() / 2;
	rv.y = get_allocated_height() / 2 - y;

	return rv;
}

void Glx::setButton(ButtonsState *buttons, unsigned int button, bool state)
{
	assert(buttons);
	switch (button) {
		case BUTTON_LEFT:
			buttons->left = state;
			break;
		case BUTTON_MIDDLE:
			buttons->middle = state;
			break;
		case BUTTON_RIGHT:
			buttons->right = state;
			break;
		default:
			;
	}
}

ModState Glx::getMods(unsigned int mods)
{
	ModState rv;

	rv.control = !!(mods & GDK_CONTROL_MASK);
	rv.shift = !!(mods & GDK_SHIFT_MASK);

	return rv;
}

void CheckMenuById(int id, bool checked)
{
}

void EnableMenuById(int id, bool checked)
{
}

void DoMessageBox(char *str, int rows, int cols, bool error)
{
}

void InvalidateText(void)
{
}

void InvalidateGraphics(void)
{
}

void AddContextMenuItem(const char *label, int id)
{
}

int ShowContextMenu(void)
{
}

void CreateContextSubmenu(void)
{
}

void PaintGraphics(void)
{
}

void HideTextEditControl(void)
{
}

void ShowTextEditControl(int x, int y, char *s)
{
}

bool TextEditControlIsVisible(void)
{
}	

void HideGraphicsEditControl(void)
{
}

void ShowGraphicsEditControl(int x, int y, char *s)
{
}

bool GraphicsEditControlIsVisible(void)
{
	return false;
}
#if 0
void GetTextWindowSize(int *w, int *h)
{

}
#endif
void ShowTextWindow(bool visible)
{
}
#if 0
void GetGraphicsWindowSize(int *w, int *h)
{
}
#endif 
bool GetSaveFile(std::string *file, const char *defExtension, const char *selPattern)
{
}

bool GetOpenFile(std::string *file, const char *defExtension, const char *selPattern)
{
}

void SetMousePointerToHand(bool yes)
{
}

void MoveTextScrollbarTo(int pos, int maxPos, int page)
{
}

void SetTimerFor(int milliseconds)
{
}

void OpenWebsite(const char *url)
{
}

void SetWindowTitle(const char *str)
{
}

void RefreshRecentMenus(void)
{
}

saveRv SaveFileYesNoCancel(void)
{
}

void LoadAllFontFiles(void)
{
}

void ExitNow(void)
{
}
