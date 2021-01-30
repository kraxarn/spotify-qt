#include "thirdparty/doctest.h"
#include "lib/settings.hpp"

#include "lib/paths/paths.hpp"
#include "thirdparty/filesystem.hpp"

class test_paths: public lib::paths
{
public:
	test_paths()
	{
		lib::log::set_log_to_stdout(false);
	}

	~test_paths()
	{
		ghc::filesystem::remove(config_file());
	}

	std::string config_file() const override
	{
		return std::string("spotify-qt.json");
	}

	std::string cache() const override
	{
		return std::string("cache");
	}
};

TEST_CASE("settings")
{
	test_paths paths;
	lib::settings settings(paths);

	settings.load();
	settings.save();
}
