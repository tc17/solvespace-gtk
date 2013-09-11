#ifndef GTK_UI_H
#define GTK_UI_H 1

#include <gtkmm.h>
#include <gdk/gdkx.h>
#include <GL/glx.h>

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
	Display* xdisplay_;
	XVisualInfo* xvinfo_;
	GLXContext context_;
	Glib::RefPtr<Gdk::Window> window_;
	SSWindow& sswindow_;
	ButtonsState buttonsState_;
	bool translation_;
	Glib::RefPtr<Gdk::Cursor> cursor_;
public:
	Glx(SSWindow &w, bool translation);
	virtual ~Glx();
	void setCursorToHand(bool yes);

protected:
	virtual void on_realize();
	virtual bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr);
	virtual bool on_event(GdkEvent *event);
private:
	Mouse translate(double x, double y);
	static void setButton(ButtonsState* buttons, unsigned int button, bool state);
	static ModState getMods(unsigned int mods);

	Glx(const Glx&);
	Glx& operator=(const Glx&);
};

class GlxGraphicsWindow : public Gtk::Window
{
	Gtk::Box box_;
	SSWindow *sswindow_;
	Glx* glx_;
public:
	static GlxGraphicsWindow& getGlxGraphicsWindow();
	virtual ~GlxGraphicsWindow();
	Glx& widget();
private:
	GlxGraphicsWindow();
	GlxGraphicsWindow(const GlxGraphicsWindow&);
	GlxGraphicsWindow& operator=(const GlxGraphicsWindow&);
};

class GlxTextWindow : public Gtk::Window
{
	SSWindow *sswindow_;
	Glx *glx_;
public:
	static GlxTextWindow& getGlxTextWindow();
	virtual ~GlxTextWindow();
	Glx& widget();
private:
	GlxTextWindow();
	GlxTextWindow(const GlxTextWindow&);
	GlxTextWindow& operator=(const GlxTextWindow&);
};

#endif /* GTK_UI_H */
