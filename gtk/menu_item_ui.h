#ifndef MENU_ITEM_UI_H
#define MENU_ITEM_UI_H 1

#include <gtkmm.h>

class MenuItem : public Gtk::CheckMenuItem
{
public:
	typedef sigc::signal<void> type_signal_activate;
	MenuItem(const Glib::ustring& label, bool mnemonic = false);
	~MenuItem();
	type_signal_activate signal_activate();
protected:
	type_signal_activate signal_activate_;
	virtual void on_activate();

};

#endif /* MENU_ITEM_UI_H */
