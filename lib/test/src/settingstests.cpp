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

	auto config_file() const -> ghc::filesystem::path override
	{
		return ghc::filesystem::temp_directory_path() / "spotify-qt.json";
	}

	auto cache() const -> ghc::filesystem::path override
	{
		return ghc::filesystem::temp_directory_path() / "cache";
	}
};

TEST_CASE("settings")
{
	test_paths paths;

	auto read_settings = [&paths]() -> nlohmann::json
	{
		ghc::filesystem::ifstream file(paths.config_file());
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
