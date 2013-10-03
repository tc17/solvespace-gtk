#include "float_window_ui.h"

FloatWindow::FloatWindow() : x_(0), y_(0), entry_(), window_()
{
	set_has_window(false);
	add(entry_);
	entry_.signal_activate().connect(sigc::mem_fun(*this, &FloatWindow::entryOnActivate));	
}

FloatWindow::~FloatWindow()
{}

void FloatWindow::move(int x, int y)
{
	x_ = x;
	y_ = y;
	if (window_)
		window_->move(x, y);
}

void FloatWindow::showEntry(int x, int y, const std::string& str)
{
	move(x, y);
	entry_.set_text(str);
	show();
	map();
//	entry_.add_modal_grab();
	entry_.grab_focus();
}

void FloatWindow::hideEntry()
{
	entry_.remove_modal_grab();
	unmap();
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

void FloatWindow::on_size_allocate(Gtk::Allocation& allocation)
{
	Gtk::Bin::on_size_allocate(allocation);	
	
	Gtk::Allocation entry_allocation;
	entry_allocation.set_x(0);
	entry_allocation.set_y(0);
	entry_allocation.set_width(allocation.get_width());
	entry_allocation.set_height(allocation.get_height());

	entry_.size_allocate(entry_allocation);
}

void FloatWindow::on_map()
{
	Gtk::Bin::on_map();
	
	Gtk::Allocation allocation;
	allocation.set_x(x_);
	allocation.set_y(y_);
	allocation.set_width(100);
	allocation.set_height(30);
	size_allocate(allocation);

}

void FloatWindow::on_realize()
{
	Gtk::Bin::on_realize();

	if (!window_) {
		GdkWindowAttr attr;

		Gtk::Allocation allocation = get_allocation();

		attr.x = allocation.get_x();
		attr.y = allocation.get_y();
		attr.width = allocation.get_width();
		attr.height = allocation.get_height();

		attr.event_mask = get_events() | Gdk::EXPOSURE_MASK;
		attr.window_type = GDK_WINDOW_CHILD;
		attr.wclass = GDK_INPUT_OUTPUT;

		window_ = Gdk::Window::create(get_parent_window(), &attr, GDK_WA_X | GDK_WA_Y);

		window_->ensure_native();

		//set_double_buffered(false);
		set_window(window_);
		set_has_window(true);

		//unset_background_color();
		//Gdk::RGBA color;
		//color.set_rgba(0, 0, 0, 1000.0);
		//override_background_color(color);

		// make the window recieve events
		//register_window(window_);
	//	entryWindow_->set_keep_above();
		window_->set_user_data(gobj());
	}
}

void FloatWindow::entryOnActivate()
{
	signal_entry_activate_.emit(entry_.get_text());
}
