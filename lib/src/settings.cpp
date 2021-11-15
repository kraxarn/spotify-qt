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

//region qt

auto lib::settings::qt() -> setting::qt &
{
	return qt_settings;
}

auto lib::settings::qt_const() const -> const setting::qt &
{
	return qt_settings;
}

//endregion

void settings::from_json(const nlohmann::json &json)
{
	auto s = json.at("Spotify");

	// Account
	lib::json::get(json, "Account", account);

	// General
	lib::json::get(json, "General", general);

	// Spotify
	setValue(s, "always_start", spotify.always_start);
	setValue(s, "backend", spotify.backend);
	setValue(s, "bitrate", spotify.bitrate);
	setValue(s, "disable_discovery", spotify.disable_discovery);
	setValue(s, "global_config", spotify.global_config);
	setValue(s, "keyring_password", spotify.keyring_password);
	setValue(s, "max_queue", spotify.max_queue);
	setValue(s, "path", spotify.path);
	setValue(s, "start_client", spotify.start_client);
	setValue(s, "username", spotify.username);

	// Qt widgets
	// TODO: This should be loaded dynamically for non-Qt clients
	lib::json::get(json, "Qt", qt_settings);
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
	return {
		{"Account", account},
		{"General", general},
		{"Spotify", {
			{"always_start", spotify.always_start},
			{"backend", spotify.backend},
			{"bitrate", spotify.bitrate},
			{"disable_discovery", spotify.disable_discovery},
			{"global_config", spotify.global_config},
			{"keyring_password", spotify.keyring_password},
			{"max_queue", spotify.max_queue},
			{"path", spotify.path},
			{"start_client", spotify.start_client},
			{"username", spotify.username},
		}},
		{"Qt", qt_settings},
	};
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
