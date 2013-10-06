#ifndef GTK_UI_H
#define GTK_UI_H 1

#include <gtkmm.h>
#include <gdk/gdkx.h>
#include <GL/glx.h>
#include "float_window_ui.h"

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
	virtual void editDone(const char *str) = 0;
	virtual void scroll(int newPos) = 0;
	virtual bool keyPress(int key) = 0;
	virtual void timerCallback() = 0;
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
	void setTimer(int milliseconds);
	bool translation();

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


class GlxWindow : public Gtk::Window
{
	Gtk::Entry entry_;
protected:
	SSWindow* sswindow_;
	Glx* glx_;
	FloatWindow floatWindow_;
	Glib::RefPtr<Gtk::AccelGroup> accelGroup_;
public:
	virtual ~GlxWindow();
	Glx& widget();
	void showEntry(int x, int y, const char *s);
	void hideEntry();
	bool entryIsVisible();
protected:
	GlxWindow();
private:
	GlxWindow(const GlxWindow&);
	GlxWindow& operator=(const GlxWindow&);
	void entryOnActivate(const std::string& str);
};

class GlxGraphicsWindow : public GlxWindow
{
	Gtk::Box box_;
public:
	static GlxGraphicsWindow& getInstance();
	virtual ~GlxGraphicsWindow();
private:
	GlxGraphicsWindow();
	GlxGraphicsWindow(const GlxGraphicsWindow&);
	GlxGraphicsWindow& operator=(const GlxGraphicsWindow&);
	virtual bool on_delete_event(GdkEventAny* event);
};

class GlxTextWindow : public GlxWindow 
{
	Gtk::Box box_;
	Glib::RefPtr<Gtk::Adjustment> adj_;
	Gtk::Scrollbar scroll_;
public:
	static GlxTextWindow& getInstance();
	virtual ~GlxTextWindow();
	void moveScroll(int pos, int maxPos, int page);
private:
	GlxTextWindow();
	GlxTextWindow(const GlxTextWindow&);
	GlxTextWindow& operator=(const GlxTextWindow&);
	void adjOnValueChanged();
	virtual bool on_delete_event(GdkEventAny* event);
};

#endif /* GTK_UI_H */
