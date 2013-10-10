#ifndef POPUP_MENU_UI_H
#define POPUP_MENU_UI_H 1

#include <gtkmm.h>

class PopupMenu {
	Gtk::Menu menu_;
	Gtk::Menu *submenu_;
	bool wait_;
	int id_;
	bool populated_;
public:
	static PopupMenu& getInstance();
	void addItem(const char* label, int id);
	void addSubmenu();
	int show();
private:
	PopupMenu();
	~PopupMenu();	
	void onSelectionDone();
	void onActivate(int id);
	void onSelect(Gtk::MenuItem *item);
	void remove(Gtk::Widget& widget);
};

#endif /* POPUP_MENU_UI_H */
