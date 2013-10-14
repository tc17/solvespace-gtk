#include "menu_item_ui.h"

MenuItem::MenuItem(const Glib::ustring& label, bool mnemonic)
	: Gtk::CheckMenuItem(label, mnemonic),
	signal_activate_()
{}

MenuItem::~MenuItem()
{}

void MenuItem::on_activate()
{
	signal_activate_.emit();
	Glib::RefPtr<Gtk::Action> action = get_related_action();
	if (action)
		action->activate();
}

MenuItem::type_signal_activate MenuItem::signal_activate()
{
	return signal_activate_;
}
