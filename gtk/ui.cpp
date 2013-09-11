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

	virtual ~SSText() {}
private:
	SSText(const SSText&);
	SSText& operator=(const SSText&);
};

GlxGraphicsWindow& GlxGraphicsWindow::getGlxGraphicsWindow()
{
	static GlxGraphicsWindow instance;
	return instance;
}

GlxGraphicsWindow::GlxGraphicsWindow() : box_(Gtk::ORIENTATION_VERTICAL), sswindow_(), glx_()
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

	add_accel_group(uiManager->get_accel_group());

	show_all();
}

GlxGraphicsWindow::~GlxGraphicsWindow()
{
	delete glx_;
	delete sswindow_;	
}

Glx& GlxGraphicsWindow::widget()
{
	return *glx_;
}

GlxTextWindow& GlxTextWindow::getGlxTextWindow()
{
	static GlxTextWindow instance;
	return instance;
}

GlxTextWindow::GlxTextWindow() : sswindow_(), glx_()
{
	sswindow_ = new SSText(SS.TW);
	glx_ = new Glx(*sswindow_, false);

//	set_type_hint(Gdk::WINDOW_TYPE_HINT_UTILITY);
	add(*glx_);

	show_all();
}

GlxTextWindow::~GlxTextWindow()
{
	delete glx_;
	delete sswindow_;
}

Glx& GlxTextWindow::widget()
{
	return *glx_;
}

Glx::Glx(SSWindow& w, bool translate) : xdisplay_(), xvinfo_(),
	context_(), window_(), sswindow_(w), buttonsState_(),
	translation_(translate)
{
	set_has_window(false);

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

		window_ = Gdk::Window::create(get_window(), &attr, GDK_WA_X | GDK_WA_Y);

		set_has_window(true);
		set_window(window_);

		unset_background_color();
		set_double_buffered(false);

		// make the window recieve expose events
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
			rv = false;
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

void GetTextWindowSize(int *w, int *h)
{
	const Glx& widget = GlxTextWindow::getGlxTextWindow().widget();
	*w = widget.get_allocated_width();
	*h = widget.get_allocated_height();
}

void GetGraphicsWindowSize(int *w, int *h)
{
	const Glx& widget = GlxGraphicsWindow::getGlxGraphicsWindow().widget();
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
	Gtk::MessageDialog dialog(GlxGraphicsWindow::getGlxGraphicsWindow(),
			str, false, error ? Gtk::MESSAGE_ERROR : Gtk::MESSAGE_INFO);
	dialog.run();
}

void InvalidateText(void)
{
	GlxTextWindow::getGlxTextWindow().widget().queue_draw();
}

void InvalidateGraphics(void)
{
	GlxGraphicsWindow::getGlxGraphicsWindow().widget().queue_draw();
}

void AddContextMenuItem(const char *label, int id)
{
}

int ShowContextMenu(void)
{
}

void CreateContextSubmenu(void)
{
}

void PaintGraphics(void)
{
	GlxGraphicsWindow::getGlxGraphicsWindow().widget().queue_draw();
}

void HideTextEditControl(void)
{
}

void ShowTextEditControl(int x, int y, char *s)
{
}

bool TextEditControlIsVisible(void)
{
}	

void HideGraphicsEditControl(void)
{
}

void ShowGraphicsEditControl(int x, int y, char *s)
{
}

bool GraphicsEditControlIsVisible(void)
{
	return false;
}

void ShowTextWindow(bool visible)
{
	GlxTextWindow& window = GlxTextWindow::getGlxTextWindow();
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
	Gtk::FileChooserDialog fileChooser(GlxGraphicsWindow::getGlxGraphicsWindow(), "");
	fileChooser.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
	fileChooser.add_button(id, Gtk::RESPONSE_OK);

	addFilters(&fileChooser, pattern);

	int rv = fileChooser.run();
	*file = fileChooser.get_filename();

	return (rv == Gtk::RESPONSE_ACCEPT);
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
}

void MoveTextScrollbarTo(int pos, int maxPos, int page)
{
}

void SetTimerFor(int milliseconds)
{
}

void OpenWebsite(const char *url)
{
}

void SetWindowTitle(const char *str)
{
	GlxGraphicsWindow::getGlxGraphicsWindow().set_title(str);
}

void RefreshRecentMenus(void)
{
}

saveRv SaveFileYesNoCancel(void)
{
}

void LoadAllFontFiles(void)
{
}

void ExitNow(void)
{
}
