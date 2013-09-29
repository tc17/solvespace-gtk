#include <string>
#include <assert.h>
#include "solvespace.h"
#include "ui.h"

std::string RecentFile[MAX_RECENT];

class Label {
	std::string _label;
	std::string _accelerator;
public:
	Label(const std::string& label)
	{
		size_t sep = label.find_first_of('\t');	
		_label = label.substr(0, sep);
		replace(&_label, "&", "_");
		if (sep != std::string::npos) {
			_accelerator = label.substr(sep + 1, std::string::npos);
			replace(&_accelerator, "Ctrl+", "<control>");
			replace(&_accelerator, "Shift+", "<shift>");
		}

	}

	const std::string& label()
	{
		return _label;
	}

	const std::string& accelerator()
	{
		return _accelerator;
	}

private:
	void replace(std::string *str, const std::string& from, const std::string& to)
	{
		assert(str);
		size_t pos = str->find(from);
		if (pos != std::string::npos) {
			str->replace(pos, from.size(), to);
		}
	}
};

class PopupMenu {
	Gtk::Menu menu_;
	Gtk::Menu *submenu_;
	bool wait_;
	int id_;
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

PopupMenu& PopupMenu::getInstance()
{
	static PopupMenu instance;
	return instance;
}

PopupMenu::PopupMenu() : menu_(), submenu_(&menu_), wait_(), id_()
{
	menu_.signal_selection_done().connect(sigc::mem_fun(*this, &PopupMenu::onSelectionDone));
}

PopupMenu::~PopupMenu()
{}

void PopupMenu::addItem(const char* label, int id)
{
	printf("%s: label: %s, id: %d\n", __func__, label, id);
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
}

void PopupMenu::addSubmenu()
{
	submenu_ = new Gtk::Menu();
}

int PopupMenu::show()
{
	menu_.show_all();
	menu_.popup(BUTTON_RIGHT, gtk_get_current_event_time());

	id_ = 0;
	wait_ = true;
	while (wait_)
		Gtk::Main::iteration();

	printf("%s: id: %d\n", __func__, id_);
	menu_.foreach(sigc::mem_fun(*this, &PopupMenu::remove));

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

class SSGraphics : public SSWindow
{
	GraphicsWindow &_window;
public:
	SSGraphics(GraphicsWindow& window) : _window(window) {}
	virtual void paint()
	{
		_window.Paint();
	}

	virtual void buttonPress(const Mouse& mouse, Button button)
	{
		switch (button) {
			case BUTTON_LEFT:
				_window.MouseLeftDown(mouse.x, mouse.y);
				break;
			case BUTTON_MIDDLE:
			case BUTTON_RIGHT:
				_window.MouseMiddleOrRightDown(mouse.x, mouse.y);
				break;
			default: ;
		}
	}

	virtual void buttonRelease(const Mouse& mouse, Button button)
	{
		switch (button) {
			case BUTTON_LEFT:
				_window.MouseLeftUp(mouse.x, mouse.y);
				break;
			case BUTTON_RIGHT:
				_window.MouseRightUp(mouse.x, mouse.y);
				break;
			default: ;
		}
	}

	virtual void button2Press(const Mouse& mouse, Button button)
	{
		if (button == BUTTON_LEFT)
			_window.MouseLeftDoubleClick(mouse.x, mouse.y);
	}

	virtual void mouseMoved(const Mouse& mouse, const ButtonsState& buttons, const ModState& mods)
	{
		_window.MouseMoved(mouse.x, mouse.y, buttons.left,
				buttons.middle, buttons.right, mods.shift, mods.control);	
	}
	
	virtual void mouseLeave()
	{
		_window.MouseLeave();
	}

	virtual void editDone(const char *str)
	{
		_window.EditControlDone(str);
	}

	virtual void scroll(int newPos) {}

	virtual ~SSGraphics() {}
private:
	SSGraphics(const SSGraphics&);
	SSGraphics& operator=(const SSGraphics&);
};

class SSText : public SSWindow
{
	TextWindow &_window;
public:
	SSText(TextWindow& window) : _window(window) {}
	virtual void paint()
	{
		_window.Paint();
	}

	virtual void buttonPress(const Mouse& mouse, Button button)
	{
		if (button == BUTTON_LEFT)
			_window.MouseEvent(true, false, mouse.x, mouse.y);
	}
	
	virtual void buttonRelease(const Mouse& mouse, Button button) {}
	virtual void button2Press(const Mouse& mouse, Button button) {}
	virtual void mouseMoved(const Mouse& mouse, const ButtonsState& buttons, const ModState& mods)
	{
		_window.MouseEvent(false, buttons.left, mouse.x, mouse.y);
	}

	virtual void mouseLeave()
	{
		_window.MouseLeave();
	}

	virtual void editDone(const char *str)
	{
		_window.EditControlDone(str);
	}

	virtual void scroll(int newPos)
	{
		_window.ScrollbarEvent(newPos);
	}

	virtual ~SSText() {}
private:
	SSText(const SSText&);
	SSText& operator=(const SSText&);
};

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
}

void FloatWindow::hideEntry()
{
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

		//unset_background_color();
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

GlxWindow::GlxWindow() : sswindow_(), glx_(), floatWindow_(), accelGroup_()
{
	set_redraw_on_allocate(false);
	floatWindow_.set_parent(*this);
	floatWindow_.signal_entry_activate().connect(sigc::mem_fun(*this, &GlxWindow::entryOnActivate));
}	

GlxWindow::~GlxWindow()
{}

void GlxWindow::forall_vfunc(gboolean include_internals, GtkCallback callback, gpointer callback_data)
{
	Gtk::Window::forall_vfunc(include_internals, callback, callback_data);
	callback(static_cast<Gtk::Widget*>(&floatWindow_)->gobj(), callback_data);
}

Glx& GlxWindow::widget()
{
	assert(glx_);
	return *glx_;
}

void GlxWindow::showEntry(int x, int y, const char *s)
{
	int rx, ry, rw, rh;

	get_size(rw, rh);

	rx = rw/2 + x;
	ry = rh/2 + y;

	if (accelGroup_)
		remove_accel_group(accelGroup_);
	floatWindow_.showEntry(rx, ry, s);
}

void GlxWindow::hideEntry()
{
	floatWindow_.hideEntry();
}

bool GlxWindow::entryIsVisible()
{
	return floatWindow_.entryIsVisible();
}

void GlxWindow::entryOnActivate(const std::string& str)
{
	//floatWindow_.hideEntry();
	if (accelGroup_)
		add_accel_group(accelGroup_);
	if (sswindow_)
		sswindow_->editDone(str.c_str());
}

GlxGraphicsWindow& GlxGraphicsWindow::getInstance()
{
	static GlxGraphicsWindow instance;
	return instance;
}

GlxGraphicsWindow::GlxGraphicsWindow() : box_(Gtk::ORIENTATION_VERTICAL)
{
	sswindow_ = new SSGraphics(SS.GW);
	glx_ = new Glx(*sswindow_, true);
	
	add(box_);
	box_.pack_end(*glx_);

	std::string path = "/ui/";
	Glib::RefPtr<Gtk::ActionGroup> actionGroup = Gtk::ActionGroup::create();
	Glib::RefPtr<Gtk::UIManager> uiManager = Gtk::UIManager::create();

	uiManager->add_ui(uiManager->new_merge_id(), path, "MenuBar", "MenuBar",
			Gtk::UI_MANAGER_MENUBAR);
	path += "MenuBar/";
	std::string subpath;
	
	for (int i = 0; SS.GW.menu[i].level >= 0; i++) {
		const GraphicsWindow::MenuEntry *entry = &SS.GW.menu[i];
		if (entry->label) {
			Label label(entry->label);
			if (entry->id)
				actionGroup->add(Gtk::Action::create(entry->label, label.label()),
						Gtk::AccelKey(label.accelerator()),
						sigc::bind<int>(sigc::ptr_fun(entry->fn), entry->id));

			else 
				actionGroup->add(Gtk::Action::create(entry->label, label.label()),
					Gtk::AccelKey(label.accelerator()));
			if (entry->level == 0) {
				subpath = path + SS.GW.menu[i].label;
				subpath += "/"; 
				uiManager->add_ui(uiManager->new_merge_id(), path,
					entry->label, entry->label,
					Gtk::UI_MANAGER_MENU, false);
			} else {
				if (SS.GW.menu[i].id) {
					uiManager->add_ui(entry->id, subpath, entry->label,
						entry->label, Gtk::UI_MANAGER_MENUITEM, false);
				} else
					uiManager->add_ui(uiManager->new_merge_id(), subpath,
						entry->label, entry->label,
						Gtk::UI_MANAGER_MENUITEM, false);
			}

		} else
			uiManager->add_ui_separator(uiManager->new_merge_id(), subpath);
	}

	uiManager->insert_action_group(actionGroup);
	
	Gtk::Widget *menuBar = uiManager->get_widget("/MenuBar");	
	if (menuBar)
		box_.pack_start(*menuBar, Gtk::PACK_SHRINK);

	accelGroup_ = uiManager->get_accel_group();

	add_accel_group(accelGroup_);

	show_all();
}

GlxGraphicsWindow::~GlxGraphicsWindow()
{
	delete glx_;
	delete sswindow_;	
}

GlxTextWindow& GlxTextWindow::getInstance()
{
	static GlxTextWindow instance;
	return instance;
}

GlxTextWindow::GlxTextWindow() : box_(), adj_(), scroll_()
{
	sswindow_ = new SSText(SS.TW);
	glx_ = new Glx(*sswindow_, false);

//	set_type_hint(Gdk::WINDOW_TYPE_HINT_UTILITY);
//
	adj_ = Gtk::Adjustment::create(0, 0, 0);
	adj_->signal_value_changed().connect(sigc::mem_fun(*this, &GlxTextWindow::adjOnValueChanged));

	scroll_.set_adjustment(adj_);
	scroll_.set_orientation(Gtk::ORIENTATION_VERTICAL);
	box_.pack_start(*glx_);
	box_.pack_start(scroll_, Gtk::PACK_SHRINK);
	add(box_);

	show_all();
}

GlxTextWindow::~GlxTextWindow()
{
	delete glx_;
	delete sswindow_;
}

void GlxTextWindow::moveScroll(int pos, int maxPos, int page)
{
	printf("%s: pos: %d, maxPos: %d, page: %d\n", __func__, pos, maxPos, page);
	//scroll_.set_range(0, maxPos);
	//scroll_.set_increments(1, page);
	//scroll_.set_value(pos);
	adj_->set_upper(maxPos);
	adj_->set_page_size(page);
	adj_->set_value(pos);
}

void GlxTextWindow::adjOnValueChanged()
{
	sswindow_->scroll(adj_->get_value());	
}

Glx::Glx(SSWindow& w, bool translate) : xdisplay_(), xvinfo_(),
	context_(), window_(), sswindow_(w), buttonsState_(),
	translation_(translate), cursor_(Gdk::Cursor::create(Gdk::HAND1))
{
	set_has_window(false);
	//set_redraw_on_allocate(false);

	xdisplay_ = gdk_x11_get_default_xdisplay();
	if (!glXQueryExtension(xdisplay_, NULL, NULL))
		throw std::runtime_error("OpenGL not supported");

	int attrlist[] = {
		GLX_RGBA,
		GLX_RED_SIZE, 1,
		GLX_GREEN_SIZE, 1,
		GLX_BLUE_SIZE, 1,
		GLX_DOUBLEBUFFER, 0,
		None
	};

	xvinfo_ = glXChooseVisual(xdisplay_, gdk_x11_get_default_screen(), attrlist);
	if (!xvinfo_)
		throw std::runtime_error("Error configuring OpenGL");

	context_ = glXCreateContext(xdisplay_, xvinfo_, NULL, True);
}
 
Glx::~Glx()
{
	glXDestroyContext(xdisplay_, context_);
}
 
void Glx::on_realize()
{
	Gtk::Widget::on_realize();

	if (!window_) {
		GdkWindowAttr attr;

		Gtk::Allocation allocation = get_allocation();

		attr.x = allocation.get_x();
		attr.y = allocation.get_y();
		attr.width = allocation.get_width();
		attr.height = allocation.get_height();

		attr.event_mask = get_events()
			| Gdk::EXPOSURE_MASK
			| Gdk::POINTER_MOTION_MASK
			| Gdk::BUTTON_MOTION_MASK
			| Gdk::BUTTON_PRESS_MASK
			| Gdk::BUTTON_RELEASE_MASK
			| Gdk::LEAVE_NOTIFY_MASK;
		attr.window_type = GDK_WINDOW_CHILD;
		attr.wclass = GDK_INPUT_OUTPUT;

		window_ = Gdk::Window::create(get_parent_window(), &attr, GDK_WA_X | GDK_WA_Y);

		set_window(window_);
		set_has_window(true);

		unset_background_color();
		set_double_buffered(false);

		// make the window recieve events
		//register_window(window_);
		window_->set_user_data(gobj());
	}
}

bool Glx::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
{
	XID xid = gdk_x11_window_get_xid(get_window()->gobj());
	if (!glXMakeCurrent(xdisplay_, xid, context_))
		return false;
	sswindow_.paint();
	glXSwapBuffers(xdisplay_, xid);

	//Gtk::Widget::on_draw(cr);

	return true;
}

bool Glx::on_event(GdkEvent *event)
{
	bool rv = true;
	switch (event->type) {
		case GDK_BUTTON_PRESS:
		{
			GdkEventButton *ev = reinterpret_cast<GdkEventButton*>(event);
			Mouse mouse = translate(ev->x, ev->y);
			setButton(&buttonsState_, ev->button, true);

			sswindow_.buttonPress(mouse, Button(ev->button));
		}
		break;

		case GDK_BUTTON_RELEASE:
		{
			GdkEventButton *ev = reinterpret_cast<GdkEventButton*>(event);
			Mouse mouse = translate(ev->x, ev->y);
			setButton(&buttonsState_, ev->button, false);

			sswindow_.buttonRelease(mouse, Button(ev->button));
		}
		break;

		case GDK_2BUTTON_PRESS:
		{
			GdkEventButton *ev = reinterpret_cast<GdkEventButton*>(event);
			Mouse mouse = translate(ev->x, ev->y);
			setButton(&buttonsState_, ev->button, true);
			
			sswindow_.button2Press(mouse, Button(ev->button));
		}
		break;

		case GDK_MOTION_NOTIFY:
		{
			GdkEventMotion *ev = reinterpret_cast<GdkEventMotion*>(event);
			Mouse mouse = translate(ev->x, ev->y);
			ModState mods = getMods(ev->state);

			sswindow_.mouseMoved(mouse, buttonsState_, mods);
		}
		break;

		case GDK_LEAVE_NOTIFY:
		{
			sswindow_.mouseLeave();
		}
		break;

		default:
			//rv = false;
			rv = Gtk::Widget::on_event(event);
	}

	return rv;
}

Mouse Glx::translate(double x, double y)
{
	Mouse rv;
	if (translation_) {
		rv.x = x - get_allocated_width() / 2;
		rv.y = get_allocated_height() / 2 - y;
	} else {
		rv.x = x;
		rv.y = y;
	}

	return rv;
}

void Glx::setButton(ButtonsState *buttons, unsigned int button, bool state)
{
	assert(buttons);
	switch (button) {
		case BUTTON_LEFT:
			buttons->left = state;
			break;
		case BUTTON_MIDDLE:
			buttons->middle = state;
			break;
		case BUTTON_RIGHT:
			buttons->right = state;
			break;
		default:
			;
	}
}

ModState Glx::getMods(unsigned int mods)
{
	ModState rv;

	rv.control = !!(mods & GDK_CONTROL_MASK);
	rv.shift = !!(mods & GDK_SHIFT_MASK);

	return rv;
}

void Glx::setCursorToHand(bool yes)
{
	Glib::RefPtr<Gdk::Window> window = get_window();

	if (yes)
		window->set_cursor(cursor_);
	else
		window->set_cursor();
}

void GetTextWindowSize(int *w, int *h)
{
	const Glx& widget = GlxTextWindow::getInstance().widget();
	*w = widget.get_allocated_width();
	*h = widget.get_allocated_height();
}

void GetGraphicsWindowSize(int *w, int *h)
{
	const Glx& widget = GlxGraphicsWindow::getInstance().widget();
	*w = widget.get_allocated_width();
	*h = widget.get_allocated_height();
}

void CheckMenuById(int id, bool checked)
{
}

void EnableMenuById(int id, bool checked)
{
}

void DoMessageBox(const char *str, int rows, int cols, bool error)
{
	Gtk::MessageDialog dialog(GlxGraphicsWindow::getInstance(),
			str, false, error ? Gtk::MESSAGE_ERROR : Gtk::MESSAGE_INFO);
	dialog.run();
}

void InvalidateText(void)
{
	GlxTextWindow::getInstance().widget().queue_draw();
}

void InvalidateGraphics(void)
{
	GlxGraphicsWindow::getInstance().widget().queue_draw();
}

void AddContextMenuItem(const char *label, int id)
{
	PopupMenu::getInstance().addItem(label, id);
}

int ShowContextMenu(void)
{
	return PopupMenu::getInstance().show();
}

void CreateContextSubmenu(void)
{
	PopupMenu::getInstance().addSubmenu();
}

void PaintGraphics(void)
{
	GlxGraphicsWindow::getInstance().widget().queue_draw();
}

void HideTextEditControl(void)
{
	GlxTextWindow::getInstance().hideEntry();
}

void ShowTextEditControl(int x, int y, char *s)
{
	GlxTextWindow::getInstance().showEntry(x, y, s);
}

bool TextEditControlIsVisible(void)
{
	return GlxTextWindow::getInstance().entryIsVisible();
}	

void HideGraphicsEditControl(void)
{
	GlxGraphicsWindow::getInstance().hideEntry();
}

void ShowGraphicsEditControl(int x, int y, char *s)
{
	GlxGraphicsWindow::getInstance().showEntry(x, y, s);
}

bool GraphicsEditControlIsVisible(void)
{
	return GlxGraphicsWindow::getInstance().entryIsVisible();
}

void ShowTextWindow(bool visible)
{
	GlxTextWindow& window = GlxTextWindow::getInstance();
	visible ? window.show() : window.hide();
}

static const char *get_next_token(const char *str) 
{
	const char *rv;
	for (rv = str; *rv; rv++)
		;
	return rv+1;
}

static void addFilters(Gtk::FileChooser *fileChooser, const char *patterns)
{
	assert(fileChooser);
	assert(patterns);

	const char *token = patterns;
	while (*token) {
		Glib::RefPtr<Gtk::FileFilter> filter = Gtk::FileFilter::create();
		filter->set_name(token);
		
		token = get_next_token(token);
		assert(token);

		filter->add_pattern(token);

		fileChooser->add_filter(filter);

		token = get_next_token(token);
	}	
}

static bool fileDialog(std::string *file, const char *pattern, const Gtk::StockID& id)
{
	Gtk::FileChooserDialog fileChooser(GlxGraphicsWindow::getInstance(), "");
	fileChooser.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
	fileChooser.add_button(id, Gtk::RESPONSE_OK);

	addFilters(&fileChooser, pattern);

	int rv = fileChooser.run();
	*file = fileChooser.get_filename();

	return (rv == Gtk::RESPONSE_OK);
}

bool GetSaveFile(std::string *file, const char *defExtension, const char *selPattern)
{
	return fileDialog(file, selPattern, Gtk::Stock::SAVE);
}

bool GetOpenFile(std::string *file, const char *defExtension, const char *selPattern)
{
	return fileDialog(file, selPattern, Gtk::Stock::OPEN);
}

void SetMousePointerToHand(bool yes)
{
	GlxGraphicsWindow::getInstance().widget().setCursorToHand(yes);
	GlxTextWindow::getInstance().widget().setCursorToHand(yes);
}

void MoveTextScrollbarTo(int pos, int maxPos, int page)
{
	GlxTextWindow::getInstance().moveScroll(pos, maxPos, page);
}

void SetTimerFor(int milliseconds)
{
}

void OpenWebsite(const char *url)
{
}

void SetWindowTitle(const char *str)
{
	GlxGraphicsWindow::getInstance().set_title(str);
}

void RefreshRecentMenus(void)
{
}

int SaveFileYesNoCancel(void)
{
	Gtk::MessageDialog dialog(GlxGraphicsWindow::getInstance(),
			"The program has changed since it was last saved.\r\n\r\n"
			"Do you want to save the changes?", false, Gtk::MESSAGE_INFO,
			Gtk::BUTTONS_NONE);
	dialog.add_button(Gtk::Stock::YES, SAVE_YES);
	dialog.add_button(Gtk::Stock::NO, SAVE_NO);
	dialog.add_button(Gtk::Stock::CANCEL, SAVE_CANCEL);
	return dialog.run();
}

void LoadAllFontFiles(void)
{
}

void ExitNow(void)
{
}
