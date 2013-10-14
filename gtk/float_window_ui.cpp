#include "float_window_ui.h"

FloatWindow::FloatWindow() : entry_(), signal_entry_activate_()
{
	set_decorated(false);
	add(entry_);
	entry_.signal_activate().connect(sigc::mem_fun(*this, &FloatWindow::entryOnActivate));
	entry_.show();
}

FloatWindow::~FloatWindow()
{}

void FloatWindow::showEntry(int x, int y, const std::string& str)
{
	entry_.set_text(str);
	move(x, y);
	show();
	entry_.grab_focus();
}

void FloatWindow::hideEntry()
{
	entry_.remove_modal_grab();
	hide();
}

bool FloatWindow::entryIsVisible()
{
	return get_visible();
}

FloatWindow::type_signal_entry_activate FloatWindow::signal_entry_activate()
{
	return signal_entry_activate_;
}

void FloatWindow::entryOnActivate()
{
	signal_entry_activate_.emit(entry_.get_text());
}
