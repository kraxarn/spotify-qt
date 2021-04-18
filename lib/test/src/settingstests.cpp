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
		ghc::filesystem::remove("spotify-qt.json");
	}

	auto config_file() const -> ghc::filesystem::path override
	{
		return "spotify-qt.json";
	}

	auto cache() const -> ghc::filesystem::path override
	{
		return "cache";
	}
};

TEST_CASE("settings")
{
	test_paths paths;
	lib::settings settings(paths);

	settings.load();
	settings.save();
}
