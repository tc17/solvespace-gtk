#ifndef FLOAT_WINDOW_UI_H
#define FLOAT_WINDOW_UI_H 1

#include <gtkmm.h>

class FloatWindow : public Gtk::Window
{
	Gtk::Entry entry_;
public:
	typedef sigc::signal<void, const std::string&> type_signal_entry_activate;
	FloatWindow();
	virtual ~FloatWindow();
	void showEntry(int x, int y, const std::string& str);
	void hideEntry();
	bool entryIsVisible();
	type_signal_entry_activate signal_entry_activate();
protected:
	type_signal_entry_activate signal_entry_activate_;
private:
	FloatWindow(const FloatWindow&);
	FloatWindow& operator=(const FloatWindow&);
	void entryOnActivate();
};

#endif /* FLOAT_WINDOW_UI_H */
