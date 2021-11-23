#include "thirdparty/doctest.h"
#include "lib/settings.hpp"

#include "lib/paths/paths.hpp"
#include "thirdparty/filesystem.hpp"

#ifdef __linux__

#define CONFIG_FILE "/tmp/spotify-qt.json"
#define CACHE_DIR "/tmp/spotify-qt/cache"

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
		return CONFIG_FILE;
	}

	auto cache() const -> ghc::filesystem::path override
	{
		return CACHE_DIR;
	}
};

TEST_CASE("settings")
{
	test_paths paths;

	auto read_settings = []() -> nlohmann::json
	{
		std::ifstream file(CONFIG_FILE);
		nlohmann::json json;
		file >> json;

		return json;
	};

	auto create_settings = [&paths, &read_settings]() -> nlohmann::json
	{
		lib::settings settings(paths);
		settings.save();

		return read_settings();
	};

	SUBCASE("can create settings")
	{
		auto json = create_settings();

		CHECK(json.at("Account").is_object());
		CHECK(json.at("General").is_object());
		CHECK(json.at("Spotify").is_object());
	}

	SUBCASE("qt is optional")
	{
		auto json = create_settings();

		CHECK_FALSE(json.contains("Qt"));

		lib::settings settings(paths);
		settings.qt();
		settings.save();

		json = read_settings();
		CHECK(json.contains("Qt"));
		CHECK(json.at("Qt").is_object());
	}
}

#endif
