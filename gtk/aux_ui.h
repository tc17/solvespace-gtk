/*
 * auxiliary UI routines
 */

#ifndef AUX_UI_H
#define AUX_UI_H 1

#include <string>
#include <gtkmm.h>

class Label {
	std::string label_;
	std::string accelerator_;
public:
	Label(const std::string& label)
	{
		size_t sep = label.find_first_of('\t');
		label_ = label.substr(0, sep);
		replace(&label_, "&", "_");
		if (sep != std::string::npos) {
			accelerator_ = label.substr(sep + 1, std::string::npos);
			replace(&accelerator_, "Ctrl+", "<control>");
			replace(&accelerator_, "Shift+", "<shift>");
		}

	}

	const std::string& label()
	{
		return label_;
	}

	const std::string& accelerator()
	{
		return accelerator_;
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

namespace FileDialog {
	void addFilters(Gtk::FileChooser *fileChooser, const char *patterns);
}

namespace Accel {
	guint key(int accelerator);
	Gdk::ModifierType mods(int accelerator);
}

#endif /* AUX_UI_H */
