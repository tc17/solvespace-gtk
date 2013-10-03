#ifndef FLOAT_WINDOW_UI_H
#define FLOAT_WINDOW_UI_H 1

#include <gtkmm.h>

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

#endif /* FLOAT_WINDOW_UI_H */
