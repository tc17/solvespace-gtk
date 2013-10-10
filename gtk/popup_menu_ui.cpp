#include <cassert>
#include "solvespace.h"
#include "ui.h"
#include "popup_menu_ui.h"

PopupMenu& PopupMenu::getInstance()
{
	static PopupMenu instance;
	return instance;
}

PopupMenu::PopupMenu() : menu_(), submenu_(&menu_), wait_(), id_(), populated_(false)
{
	menu_.signal_selection_done().connect(sigc::mem_fun(*this, &PopupMenu::onSelectionDone));
}

PopupMenu::~PopupMenu()
{}

void PopupMenu::addItem(const char* label, int id)
{
	assert(id == CONTEXT_SEPARATOR || label);

	Gtk::MenuItem *item = Gtk::manage(id == CONTEXT_SEPARATOR ? new Gtk::SeparatorMenuItem() : new Gtk::MenuItem(label));
	
	if (id == CONTEXT_SUBMENU) {
		assert(submenu_ != &menu_);

		Gtk::Menu *submenu = Gtk::manage(submenu_);
		item->set_submenu(*submenu);

		item->signal_select().connect(sigc::bind(sigc::mem_fun(*this, &PopupMenu::onSelect), item));

		submenu_ = &menu_;
	} else if (id != CONTEXT_SEPARATOR)
		item->signal_activate().connect(sigc::bind(sigc::mem_fun(*this, &PopupMenu::onActivate), id));

	submenu_->append(*item);
	populated_ = true;
}

void PopupMenu::addSubmenu()
{
	submenu_ = new Gtk::Menu();
}

int PopupMenu::show()
{
	id_ = 0;
	if (populated_) {
		menu_.show_all();
		menu_.popup(BUTTON_RIGHT, gtk_get_current_event_time());

		wait_ = true;
		while (wait_)
			Gtk::Main::iteration();

		printf("%s: id: %d\n", __func__, id_);
		menu_.foreach(sigc::mem_fun(*this, &PopupMenu::remove));
		populated_ = false;
	}

	return id_;
}

void PopupMenu::onSelectionDone()
{
	wait_ = false;
}

void PopupMenu::onActivate(int id)
{
	id_ = id;
	printf("%s: id: %d\n", __func__, id_);
}

void PopupMenu::onSelect(Gtk::MenuItem *item)
{
	printf("%s\n", __func__);
	assert(item);
	//item->get_submenu()->select_first(true);
}

void PopupMenu::remove(Gtk::Widget& widget)
{
	menu_.remove(widget);
}
