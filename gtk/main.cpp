#include <assert.h>
#include <stdexcept>
#include <clocale>
#include <gtkmm.h>
//#include <gtkmm/drawingarea.h>
#include "solvespace.h"
#include "ui.h"

int main(int argc, char *argv[])
{
	Glib::RefPtr<Gtk::Application> app =
		Gtk::Application::create(argc, argv, "org.solvespace");

	setlocale(LC_NUMERIC, "C");

	GlxGraphicsWindow& graphicsWindow = GlxGraphicsWindow::getInstance();
	GlxTextWindow& textWindow = GlxTextWindow::getInstance();

	app->add_window(graphicsWindow);
	app->add_window(textWindow);

	graphicsWindow.show_all();
	textWindow.show_all();

	SS.Init(NULL);

	return app->run();
}
