#include <clocale>
#include <gtkmm.h>
#include "solvespace.h"
#include "ui.h"

int main(int argc, char *argv[])
{
#if 0
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
#endif

	setlocale(LC_NUMERIC, "C");
	Gtk::Main solvespace(argc, argv, false);

	GlxGraphicsWindow& graphicsWindow = GlxGraphicsWindow::getInstance();
	GlxTextWindow& textWindow = GlxTextWindow::getInstance();

	graphicsWindow.show_all();
	textWindow.show_all();

	SS.Init(NULL);

	while (Gtk::Main::iteration()) {
		SS.DoLater();
	}
	

	return 0;
}
