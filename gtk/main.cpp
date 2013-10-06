#include <clocale>
#include <gtkmm.h>
#include "solvespace.h"
#include "ui.h"

int main(int argc, char *argv[])
{
	Glib::RefPtr<Gtk::Application> app =
		Gtk::Application::create(argc, argv, "org.solvespace");

	setlocale(LC_NUMERIC, "C");

	GlxGraphicsWindow& graphicsWindow = GlxGraphicsWindow::getInstance();
	GlxTextWindow& textWindow = GlxTextWindow::getInstance();

	graphicsWindow.show_all();
	textWindow.show_all();

	SS.Init(NULL);

	return app->run(graphicsWindow);
}
