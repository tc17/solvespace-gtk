/*
 * configuration routines
 */

#include <stdint.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <string>
#include <glibmm/fileutils.h>
#include <glibmm/keyfile.h>
#include <glibmm/miscutils.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include "solvespace.h"

class Conf {
	bool file_exist_;
	Glib::KeyFile file_;
	const char *group_;
public:
	static Conf& getInstance()
	{
		static Conf instance;
		return instance;
	}
	
	bool getUint64(uint64_t *v, const std::string& name)
	{
		assert(v);
		if (key_exist(name)) {
			*v = file_.get_uint64(group_, name);
			return true;
		}

		return false;
	}

	bool getDouble(double *v, const std::string& name)
	{
		assert(v);
		if (key_exist(name)) {
			*v = file_.get_double(group_, name);
			return true;
		}

		return false;
	}

	bool getString(std::string *v, const std::string& name)
	{
		assert(v);
		if (key_exist(name)) {
			*v = file_.get_string(group_, name);
			return true;
		}

		return false;
	}
	
	void setUint64(uint64_t v, const std::string& name)
	{
		file_.set_uint64(group_, name, v);
	}

	void setDouble(double v, const std::string& name)
	{
		file_.set_double(group_, name, v);
	}

	void setString(const std::string& v, const std::string& name)
	{
		file_.set_string(group_, name, v);
	}
	
private:
	Conf() : file_exist_(false), file_(), group_("solvespace")
	{
		const std::string path = Glib::get_user_config_dir() + "/solvespace/";
		const std::string filename = "config";
		std::string error;
		
		try {
			file_exist_ = file_.load_from_file(path + filename, Glib::KEY_FILE_KEEP_COMMENTS);
		}
		catch (Glib::FileError& e) {
			try {
				create_file(path, filename);
				file_.load_from_file(path + filename, Glib::KEY_FILE_KEEP_COMMENTS);
			}
			catch (Glib::Error& e) {
				error = e.what();
			}
		}
		catch (Glib::Error& e) {
			error = e.what();
		}

		if (!error.empty())
			fprintf(stderr, "%s: %s\n", __func__, error.c_str());
	}
	
	~Conf()
	{
	}

	inline bool key_exist(const std::string& name)
	{
		return file_exist_ && file_.has_group(group_) && file_.has_key(group_, name);
	}

	inline void create_file(const std::string& path, const std::string& name)
	{
		int fd;
		if (mkdir(path.c_str(), S_IRUSR | S_IWUSR | S_IXUSR) && errno != EEXIST)
			throw (Glib::FileError(Glib::FileError::Code(errno), strerror(errno)));

		if ((fd = open((path + name).c_str(), O_CREAT, S_IRUSR | S_IWUSR)) == -1)
			throw (Glib::FileError(Glib::FileError::Code(errno), strerror(errno)));

		close(fd);
	}
};


void CnfFreezeString(const std::string& str, const char *name)
{ 
	Conf::getInstance().setString(str, name);
}

void CnfFreezeDWORD(DWORD v, const char *name)
{
	Conf::getInstance().setUint64(v, name);
}

void CnfFreezeFloat(float v, const char *name)
{
	Conf::getInstance().setDouble(v, name);
}

std::string CnfThawString(const char *name)
{
	std::string rv;
	printf("request string with name %s\n", name);
	if (Conf::getInstance().getString(&rv, name))
		return rv;

	return std::string();
}

DWORD CnfThawDWORD(DWORD v, const char *name)
{
	uint64_t rv;
	printf("request dword with name %s\n", name);
	if (Conf::getInstance().getUint64(&rv, name))
		return static_cast<DWORD>(rv);

	return v;
}

float CnfThawFloat(float v, const char *name) 
{
	double rv;
	printf("request float with name %s\n", name);
	if (Conf::getInstance().getDouble(&rv, name))
		return static_cast<float>(rv);

	return v;
}
