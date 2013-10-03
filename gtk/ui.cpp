#include <string>
#include <cassert>
#include "solvespace.h"
#include "ui.h"
#include "aux_ui.h"
#include "popup_menu_ui.h"

std::string RecentFile[MAX_RECENT];

class SSGraphics : public SSWindow
{
	GraphicsWindow &window_;
public:
	SSGraphics(GraphicsWindow& window) : window_(window) {}
	virtual void paint()
	{
		window_.Paint();
	}

	virtual void buttonPress(const Mouse& mouse, Button button)
	{
		switch (button) {
			case BUTTON_LEFT:
				window_.MouseLeftDown(mouse.x, mouse.y);
				break;
			case BUTTON_MIDDLE:
			case BUTTON_RIGHT:
				window_.MouseMiddleOrRightDown(mouse.x, mouse.y);
				break;
			default: ;
		}
	}

	virtual void buttonRelease(const Mouse& mouse, Button button)
	{
		switch (button) {
			case BUTTON_LEFT:
				window_.MouseLeftUp(mouse.x, mouse.y);
				break;
			case BUTTON_RIGHT:
				window_.MouseRightUp(mouse.x, mouse.y);
				break;
			default: ;
		}
	}

	virtual void button2Press(const Mouse& mouse, Button button)
	{
		if (button == BUTTON_LEFT)
			window_.MouseLeftDoubleClick(mouse.x, mouse.y);
	}

	virtual void mouseMoved(const Mouse& mouse, const ButtonsState& buttons, const ModState& mods)
	{
		window_.MouseMoved(mouse.x, mouse.y, buttons.left,
				buttons.middle, buttons.right, mods.shift, mods.control);	
	}
	
	virtual void mouseLeave()
	{
		window_.MouseLeave();
	}

	virtual void editDone(const char *str)
	{
		window_.EditControlDone(str);
	}

	virtual void scroll(int newPos) {}

	virtual bool keyPress(int key)
	{
		return window_.KeyDown(key);
	}

	virtual void timerCallback()
	{
		window_.TimerCallback();
	}

	virtual ~SSGraphics() {}
private:
	SSGraphics(const SSGraphics&);
	SSGraphics& operator=(const SSGraphics&);
};

class SSText : public SSWindow
{
	TextWindow &window_;
public:
	SSText(TextWindow& window) : window_(window) {}
	virtual void paint()
	{
		window_.Paint();
	}

	virtual void buttonPress(const Mouse& mouse, Button button)
	{
		if (button == BUTTON_LEFT)
			window_.MouseEvent(true, false, mouse.x, mouse.y);
	}
	
	virtual void buttonRelease(const Mouse& mouse, Button button) {}
	virtual void button2Press(const Mouse& mouse, Button button) {}
	virtual void mouseMoved(const Mouse& mouse, const ButtonsState& buttons, const ModState& mods)
	{
		window_.MouseEvent(false, buttons.left, mouse.x, mouse.y);
	}

	virtual void mouseLeave()
	{
		window_.MouseLeave();
	}

	virtual void editDone(const char *str)
	{
		window_.EditControlDone(str);
	}

	virtual void scroll(int newPos)
	{
		window_.ScrollbarEvent(newPos);
	}

	virtual bool keyPress(int key)
	{
		return false;
	}

	virtual void timerCallback()
	{
		window_.TimerCallback();
	}

	virtual ~SSText() {}
private:
	SSText(const SSText&);
	SSText& operator=(const SSText&);
};


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
	assert(glx_);

	floatWindow_.hideEntry();
	glx_->grab_focus();
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
			Gtk::AccelKey accel = entry->accel ? Gtk::AccelKey(Accel::key(entry->accel), Accel::mods(entry->accel)) :
				Gtk::AccelKey("");
#if 0
			if (entry->id)
				actionGroup->add(Gtk::Action::create(entry->label, label.label()),
						Gtk::AccelKey(label.accelerator()),
						sigc::bind<int>(sigc::ptr_fun(entry->fn), entry->id));

			else 
				actionGroup->add(Gtk::Action::create(entry->label, label.label()),
					Gtk::AccelKey(label.accelerator()));
#endif
			if (entry->id)
				actionGroup->add(Gtk::Action::create(entry->label, label.label()),
						accel, sigc::bind<int>(sigc::ptr_fun(entry->fn), entry->id));
			else 
				actionGroup->add(Gtk::Action::create(entry->label, label.label()));

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
	set_can_focus(true);
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
		Gdk::EventMask eventMask = get_events() 
			| Gdk::EXPOSURE_MASK
			| Gdk::POINTER_MOTION_MASK
			| Gdk::BUTTON_MOTION_MASK
			| Gdk::BUTTON_PRESS_MASK
			| Gdk::BUTTON_RELEASE_MASK
			| Gdk::LEAVE_NOTIFY_MASK
			| Gdk::KEY_PRESS_MASK;

		Gtk::Allocation allocation = get_allocation();

		attr.x = allocation.get_x();
		attr.y = allocation.get_y();
		attr.width = allocation.get_width();
		attr.height = allocation.get_height();

		attr.event_mask = eventMask;
		attr.window_type = GDK_WINDOW_CHILD;
		attr.wclass = GDK_INPUT_OUTPUT;

		window_ = Gdk::Window::create(get_parent_window(), &attr, GDK_WA_X | GDK_WA_Y);

		//set_events(eventMask);

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

		case GDK_KEY_PRESS:
		{
			GdkEventKey *ev = reinterpret_cast<GdkEventKey*>(event);
			rv = sswindow_.keyPress(ev->keyval);
		}
		break;

		default:
			rv = false;
	}

	if (!rv)		
		rv = Gtk::Widget::on_event(event);

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

void Glx::setTimer(int milliseconds)
{
	Glib::signal_timeout().connect_once(sigc::mem_fun(sswindow_, &SSWindow::timerCallback), milliseconds);	
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
	printf("%s: STUB\n", __func__);
}

void EnableMenuById(int id, bool checked)
{
	printf("%s: STUB\n", __func__);
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

static bool fileDialog(std::string *file, const char *pattern, const Gtk::StockID& id)
{
	Gtk::FileChooserDialog fileChooser(GlxGraphicsWindow::getInstance(), "");
	fileChooser.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
	fileChooser.add_button(id, Gtk::RESPONSE_OK);

	FileDialog::addFilters(&fileChooser, pattern);

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

void OpenWebsite(const char *url)
{
	printf("%s: STUB\n", __func__);
}

void SetTimerFor(int milliseconds)
{
	GlxGraphicsWindow::getInstance().widget().setTimer(milliseconds);
	GlxTextWindow::getInstance().widget().setTimer(milliseconds);
}

void SetWindowTitle(const char *str)
{
	GlxGraphicsWindow::getInstance().set_title(str);
}

void RefreshRecentMenus(void)
{
	printf("%s: STUB\n", __func__);
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
	printf("%s: STUB\n", __func__);
}

void ExitNow(void)
{
	printf("%s: STUB\n", __func__);
}
