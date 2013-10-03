#ifndef AUX_UI_H
#define AUX_UI_H 1

#include <string>
#include <gtkmm.h>

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
		printf("%s: accelerator: %s\n", __func__, _accelerator.c_str());
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

namespace FileDialog {
	void addFilters(Gtk::FileChooser *fileChooser, const char *patterns);
}

namespace Accel {
	guint key(int accelerator);
	Gdk::ModifierType mods(int accelerator);
}

#endif /* AUX_UI_H */
