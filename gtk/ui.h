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
	virtual void editDone(const char *str) = 0;
	virtual void scroll(int newPos) = 0;
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

class FloatWindow : public Gtk::Bin
{
	int x_, y_;
	Gtk::Entry entry_;
	Glib::RefPtr<Gdk::Window> window_;
public:
	typedef sigc::signal<void, const std::string&> type_signal_entry_activate;
	FloatWindow();
	virtual ~FloatWindow();
	void showEntry(int x, int y, const std::string& str);
	void hideEntry();
	bool entryIsVisible();
	type_signal_entry_activate signal_entry_activate();
protected:
	virtual void on_size_allocate(Gtk::Allocation& allocation);
	virtual void on_map();
	virtual void on_realize();
	type_signal_entry_activate signal_entry_activate_;
private:
	FloatWindow(const FloatWindow&);
	FloatWindow& operator=(const FloatWindow&);
	void entryOnActivate();
	void move(int x, int y);
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
	virtual void forall_vfunc(gboolean include_internals, GtkCallback callback, gpointer callback_data);
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
};

#endif /* GTK_UI_H */
