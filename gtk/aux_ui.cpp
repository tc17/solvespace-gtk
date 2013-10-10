#include <cassert>
#include "solvespace.h"
#include "aux_ui.h"

namespace FileDialog {
	static const char *get_next_token(const char *str)
	{
		const char *rv;
		for (rv = str; *rv; rv++)
			;
		return rv+1;
	}

	void addFilters(Gtk::FileChooser *fileChooser, const char *patterns)
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

}

namespace Accel {
	guint key(int accelerator)
	{
		return accelerator & ~(GraphicsWindow::CTRL_MASK | GraphicsWindow::SHIFT_MASK);
	}

	Gdk::ModifierType mods(int accelerator)
	{
		Gdk::ModifierType rv = Gdk::ModifierType(0);
		if (accelerator & GraphicsWindow::CTRL_MASK)
			rv |= Gdk::CONTROL_MASK;
		if (accelerator & GraphicsWindow::SHIFT_MASK)
			rv |= Gdk::SHIFT_MASK;
		return rv;
	}
}
