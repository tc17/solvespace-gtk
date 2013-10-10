/*
 * configuration routines
 */

#include <stdint.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
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
	std::string filename_;
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

	bool getBool(bool *v, const std::string& name)
	{
		assert(v);
		if (key_exist(name)) {
			*v = file_.get_boolean(group_, name);
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

	void setBool(bool v, const std::string& name)
	{
		file_.set_boolean(group_, name, v);
	}
	
private:
	Conf() : file_exist_(false), filename_(), file_(), group_("solvespace")
	{
		const std::string path = Glib::get_user_config_dir() + "/solvespace/";
		filename_ = path + "config";
		std::string error;
		
		try {
			file_exist_ = file_.load_from_file(filename_, Glib::KEY_FILE_KEEP_COMMENTS);
		}
		catch (Glib::FileError& e) {
			try {
				create_dir(path);
				create_file(filename_);
				file_.load_from_file(filename_, Glib::KEY_FILE_KEEP_COMMENTS);
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
		try {
			save(filename_, file_.to_data());
		} catch (Glib::FileError& e) {
			fprintf(stderr, "%s: %s\n", __func__, e.what().c_str());
		}
	}

	inline bool key_exist(const std::string& name)
	{
		return file_exist_ && file_.has_group(group_) && file_.has_key(group_, name);
	}
	
	//FIXME: rework this
	inline void create_file(const std::string& path)
	{
		int fd;
		if ((fd = open(path.c_str(), O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR)) == -1)
			throw (Glib::FileError(Glib::FileError::Code(errno), strerror(errno)));
		close(fd);
	}

	inline void create_dir(const std::string& path)
	{
		if (mkdir(path.c_str(), S_IRUSR | S_IWUSR | S_IXUSR) && errno != EEXIST)
			throw (Glib::FileError(Glib::FileError::Code(errno), strerror(errno)));
	}

	void save(const std::string& path, const std::string& data)
	{
		int fd;
		if ((fd = open(path.c_str(), O_WRONLY | O_TRUNC)) == -1)
			throw (Glib::FileError(Glib::FileError::Code(errno), strerror(errno)));

		const char *buf = data.c_str();
		size_t bufsize = data.size();
		ssize_t written = 0;

		do {
			ssize_t rv = write(fd, buf + written, bufsize - written);
			if (rv == -1) {
				close(fd);
				throw (Glib::FileError(Glib::FileError::Code(errno), strerror(errno)));
			}
			written += rv;
		} while ((size_t)written < bufsize);

		close(fd);
	}
};


void CnfFreezeString(const std::string& str, const char *name)
{ 
	Conf::getInstance().setString(str, name);
}

void CnfFreezeInt(uint32_t v, const char *name)
{
	Conf::getInstance().setUint64(v, name);
}

void CnfFreezeFloat(float v, const char *name)
{
	Conf::getInstance().setDouble(v, name);
}

void CnfFreezeBool(bool v, char const *name)
{
	Conf::getInstance().setBool(v, name);
}

std::string CnfThawString(const std::string& v, const char *name)
{
	std::string rv;
	if (Conf::getInstance().getString(&rv, name))
		return rv;

	return v;
}

uint32_t CnfThawInt(uint32_t v, const char *name)
{
	uint64_t rv;
	if (Conf::getInstance().getUint64(&rv, name))
		return static_cast<uint32_t>(rv);

	return v;
}

float CnfThawFloat(float v, const char *name) 
{
	double rv;
	if (Conf::getInstance().getDouble(&rv, name))
		return static_cast<float>(rv);

	return v;
}

bool CnfThawBool(bool v, char const *name)
{
	bool rv;
	if (Conf::getInstance().getBool(&rv, name))
		return rv;

	return v;
}
