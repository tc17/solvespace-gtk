#include <assert.h>
#include <stdexcept>
#include <gtkmm.h>
//#include <gtkmm/drawingarea.h>
#include "solvespace.h"
#include "ui.h"

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
			_window.MouseEvent(true, true, mouse.x, mouse.y);
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
};

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


int main(int argc, char *argv[])
{
	Glib::RefPtr<Gtk::Application> app =
		Gtk::Application::create(argc, argv, "org.solvespace");

	Gtk::Window graphicsWindow;
	Gtk::Window textWindow;
	Gtk::Box grBox(Gtk::ORIENTATION_VERTICAL);
	
	SSGraphics ssGraphics(SS.GW);
	Glx graphicsGlx(ssGraphics);

	SSText ssText(SS.TW);
	Glx textGlx(ssText);

	graphicsWindow.add(grBox);
	grBox.pack_start(graphicsGlx);
	graphicsWindow.set_title("SolveSpace (not yet saved)");
	
	textWindow.add(textGlx);
	textWindow.set_title("SolveSpace - Browser");
	textWindow.set_type_hint(Gdk::WINDOW_TYPE_HINT_UTILITY);

	app->add_window(graphicsWindow);
	app->add_window(textWindow);

	graphicsWindow.show_all();
	textWindow.show_all();

	textWidget = &textGlx;
	graphicsWidget = &graphicsGlx;

	SS.Init(NULL);

	std::string path = "/ui/";
	Glib::RefPtr<Gtk::ActionGroup> actionGroup = Gtk::ActionGroup::create();
	Glib::RefPtr<Gtk::UIManager> uiManager = Gtk::UIManager::create();

	uiManager->add_ui(uiManager->new_merge_id(), path, "MenuBar", "MenuBar",
			Gtk::UI_MANAGER_MENUBAR);
	path += "MenuBar/";
	std::string subpath;
	
	for (int i = 0; SS.GW.menu[i].level >= 0; i++) {
		if (SS.GW.menu[i].label) {
			Label label(SS.GW.menu[i].label);
			actionGroup->add(Gtk::Action::create(SS.GW.menu[i].label, label.label()),
					Gtk::AccelKey(label.accelerator()));
			if (SS.GW.menu[i].level == 0) {
				subpath = path + SS.GW.menu[i].label;
				subpath += "/"; 
				uiManager->add_ui(uiManager->new_merge_id(), path,
					SS.GW.menu[i].label, SS.GW.menu[i].label,
					Gtk::UI_MANAGER_MENU, false);
			} else {
				if (SS.GW.menu[i].id)
					uiManager->add_ui(SS.GW.menu[i].id, subpath, SS.GW.menu[i].label,
						SS.GW.menu[i].label, Gtk::UI_MANAGER_MENUITEM, false);
				else
					uiManager->add_ui(uiManager->new_merge_id(), subpath,
						SS.GW.menu[i].label, SS.GW.menu[i].label,
						Gtk::UI_MANAGER_MENUITEM, false);
			}

		} else
			uiManager->add_ui_separator(uiManager->new_merge_id(), subpath);
	}

	uiManager->insert_action_group(actionGroup);

	printf("%s\n", uiManager->get_ui().c_str());
	
	Gtk::Widget *menuBar = uiManager->get_widget("/MenuBar");	
	if (menuBar)
		grBox.pack_start(*menuBar, Gtk::PACK_SHRINK);

	graphicsWindow.add_accel_group(uiManager->get_accel_group());
	graphicsWindow.show_all();

	return app->run();
}

void GetTextWindowSize(int *w, int *h)
{
	if (textWidget) {
		*w = textWidget->get_allocated_width();
		*h = textWidget->get_allocated_height();
	} else {
		*w = 0;
		*h = 0;
	}

}

void GetGraphicsWindowSize(int *w, int *h)
{
	if (graphicsWidget) {
		*w = graphicsWidget->get_allocated_width();
		*h = graphicsWidget->get_allocated_height();
	} else {
		*w = 0;
		*h = 0;
	}
}
