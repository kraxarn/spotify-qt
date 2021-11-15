#include "lib/settings.hpp"

using namespace lib;

settings::settings(const paths &paths)
	: path(paths)
{
	load();
}

auto settings::file_name() const -> std::string
{
	return path.config_file();
}

auto settings::file_path() const -> std::string
{
	return ghc::filesystem::path(file_name()).parent_path().string();
}

auto lib::settings::qt() -> setting::qt &
{
	if (!qt_settings)
	{
		qt_settings = std::unique_ptr<lib::setting::qt>(new lib::setting::qt());
	}

	return *qt_settings;
}

void settings::from_json(const nlohmann::json &json)
{
	lib::json::get(json, "Account", account);
	lib::json::get(json, "General", general);
	lib::json::get(json, "Spotify", spotify);

	if (json.contains("Qt"))
	{
		qt_settings = std::unique_ptr<lib::setting::qt>(new lib::setting::qt());
		lib::json::get(json, "Qt", *qt_settings);
	}
}

void settings::load()
{
	auto name = file_name();

	std::ifstream file(name);
	if (!file.is_open())
	{
		log::warn("Failed to load settings: \"{}\" not found", name);
		return;
	}

	try
	{
		nlohmann::json json;
		file >> json;
		from_json(json);
	}
	catch (const nlohmann::json::exception &e)
	{
		log::error("Failed to load settings: {}", e.what());
	}

	file.close();
}

auto settings::to_json() const -> nlohmann::json
{
	auto json = nlohmann::json{
		{"Account", account},
		{"General", general},
		{"Spotify", spotify},
	};

	if (qt_settings)
	{
		json["Qt"] = *qt_settings;
	}

	return json;
}

void settings::save()
{
	std::lock_guard<std::mutex> lock(mutex);

	auto file_dir = file_path();
	if (!ghc::filesystem::exists(file_dir))
	{
		ghc::filesystem::create_directories(file_dir);
	}

	std::ofstream file(file_name());
	file << std::setw(4) << to_json();
	file.close();
}

void settings::remove_client()
{
	account.client_id = std::string();
	account.client_secret = std::string();
}

void settings::remove_tokens()
{
	account.access_token = std::string();
	account.refresh_token = std::string();
}

auto settings::get_dark_theme() const -> bool
{
	return general.style_palette == lib::palette::dark;
}

void settings::set_dark_theme(bool value)
{
	general.style_palette = value ? lib::palette::dark : lib::palette::app;
}
