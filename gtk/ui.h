#ifndef GTK_UI_H
#define GTK_UI_H 1

#include <gtkmm.h>
#include <gdk/gdkx.h>
#include <GL/glx.h>

extern Gtk::Widget *textWidget;
extern Gtk::Widget *graphicsWidget;

extern Gtk::Window *graphicsWin;
extern Gtk::Window *textWin;

enum Button {
	BUTTON_LEFT = 1,
	BUTTON_MIDDLE = 2,
	BUTTON_RIGHT = 3
};

struct ButtonsState {
	bool left;
	bool middle;
	bool right;
};

struct Mouse {
	double x;
	double y;
};

struct ModState {
	bool control;
	bool shift;
};

class SSWindow 
{
public:
	virtual void paint() = 0;
	virtual void buttonPress(const Mouse& mouse, Button button) = 0;
	virtual void buttonRelease(const Mouse& mouse, Button button) = 0;
	virtual void button2Press(const Mouse& mouse, Button button) = 0;
	virtual void mouseMoved(const Mouse& mouse, const ButtonsState& buttons, const ModState& mods) = 0;
	virtual void mouseLeave() = 0;
	virtual ~SSWindow() {};
};

class Glx : public Gtk::Widget
{
	Display* _xdisplay;
	XVisualInfo* _xvinfo;
	GLXContext _context;
	Glib::RefPtr<Gdk::Window> _window;
	SSWindow& _sswindow;
	ButtonsState buttonsState;
public:
	Glx(SSWindow &w);
	virtual ~Glx();

protected:
	virtual void on_realize();
	virtual bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr);
	virtual bool on_event(GdkEvent *event);
private:
	Mouse translate(double x, double y);
	static void setButton(ButtonsState* buttons, unsigned int button, bool state);
	static ModState getMods(unsigned int mods);
};

#endif /* GTK_UI_H */
