#include "settings.hpp"

using namespace lib;

Settings::Settings()
{
	load();
}

std::string Settings::fileName()
{
	return fmt::format("{}/spotify-qt.json", filePath());
}

std::string Settings::filePath()
{
	// TODO: Very temporary
	return fmt::format("{}/.config/kraxarn", std::getenv("HOME"));
}

template<typename T>
void Settings::setValue(const nlohmann::json &json, const std::string &key, T &value)
{
	if (!json.contains(key))
		return;

	try
	{
		json.at(key).get_to(value);
	}
	catch (const nlohmann::json::exception &e)
	{
		log::warn("Failed to parse settings for key: {} ({})", key, e.what());
	}
}

void Settings::fromJson(const nlohmann::json &json)
{
	auto a = json.at("Account");
	auto g = json.at("General");
	auto s = json.at("Spotify");

	// Account
	setValue(a, "access_token", account.accessToken);
	setValue(a, "client_id", account.clientId);
	setValue(a, "client_secret", account.clientSecret);
	setValue(a, "refresh_token", account.refreshToken);

	// General
	setValue(g, "custom_playlist_order", general.customPlaylistOrder);
	setValue(g, "fallback_icons", general.fallbackIcons);
	setValue(g, "fixed_width_time", general.fixedWidthTime);
	setValue(g, "hidden_song_headers", general.hiddenSongHeaders);
	setValue(g, "last_playlist", general.lastPlaylist);
	setValue(g, "last_version", general.lastVersion);
	setValue(g, "last_volume", general.lastVolume);
	setValue(g, "media_controller", general.mediaController);
	setValue(g, "playlist_order", general.playlistOrder);
	setValue(g, "pulse_volume", general.pulseVolume);
	setValue(g, "refresh_interval", general.refreshInterval);
	setValue(g, "show_changelog", general.showChangelog);
	setValue(g, "show_context_info", general.showContextInfo);
	setValue(g, "single_click_play", general.singleClickPlay);
	setValue(g, "song_header_resize_mode", general.songHeaderResizeMode);
	setValue(g, "song_header_sort_by", general.songHeaderSortBy);
	setValue(g, "spotify_playback_order", general.spotifyPlaybackOrder);
	setValue(g, "style", general.style);
	setValue(g, "style_palette", general.stylePalette);
	setValue(g, "track_numbers", general.trackNumbers);
	setValue(g, "tray_album_art", general.trayAlbumArt);
	setValue(g, "tray_icon", general.trayIcon);
	setValue(g, "tray_light_icon", general.trayLightIcon);
	setValue(g, "tray_notifications", general.trayNotifications);

	// Spotify
	setValue(s, "always_start", spotify.alwaysStart);
	setValue(s, "backend", spotify.backend);
	setValue(s, "bitrate", spotify.bitrate);
	setValue(s, "global_config", spotify.globalConfig);
	setValue(s, "keyring_password", spotify.keyringPassword);
	setValue(s, "max_queue", spotify.maxQueue);
	setValue(s, "path", spotify.path);
	setValue(s, "start_client", spotify.startClient);
	setValue(s, "username", spotify.username);

	// Validate
	auto errors = validate();

	for (auto &error : errors)
		log::error("One or more invalid {} settings found, restoring defaults", error.first);
	if (errors.find("General") != errors.end())
		general = General{};
	if (errors.find("Spotify") != errors.end())
		spotify = Spotify{};
}

void Settings::load()
{
	auto path = fileName();

	std::ifstream file(path);
	if (!file.is_open())
	{
		log::warn("Failed to load settings: \"{}\" not found", path);
		return;
	}

	try
	{
		nlohmann::json json;
		file >> json;
		fromJson(json);
	}
	catch (const nlohmann::json::exception &e)
	{
		log::error("Failed to load settings: {}", e.what());
	}

	file.close();
}

nlohmann::json Settings::toJson() const
{
	return {
		{"Account", {
			{"access_token", account.accessToken},
			{"client_id", account.clientId},
			{"client_secret", account.clientSecret},
			{"refresh_token", account.refreshToken}
		}},
		{"General", {
			{"custom_playlist_order", general.customPlaylistOrder},
			{"fallback_icons", general.fallbackIcons},
			{"fixed_width_time", general.fixedWidthTime},
			{"hidden_song_headers", general.hiddenSongHeaders},
			{"last_playlist", general.lastPlaylist},
			{"last_version", general.lastVersion},
			{"last_volume", general.lastVolume},
			{"media_controller", general.mediaController},
			{"playlist_order", general.playlistOrder},
			{"pulse_volume", general.pulseVolume},
			{"refresh_interval", general.refreshInterval},
			{"show_changelog", general.showChangelog},
			{"show_context_info", general.showContextInfo},
			{"single_click_play", general.singleClickPlay},
			{"song_header_resize_mode", general.songHeaderResizeMode},
			{"song_header_sort_by", general.songHeaderSortBy},
			{"spotify_playback_order", general.spotifyPlaybackOrder},
			{"style", general.style},
			{"style_palette", general.stylePalette},
			{"track_numbers", general.trackNumbers},
			{"tray_album_art", general.trayAlbumArt},
			{"tray_icon", general.trayIcon},
			{"tray_light_icon", general.trayLightIcon},
			{"tray_notifications", general.trayNotifications}
		}},
		{"Spotify", {
			{"always_start", spotify.alwaysStart},
			{"backend", spotify.backend},
			{"bitrate", spotify.bitrate},
			{"global_config", spotify.globalConfig},
			{"keyring_password", spotify.keyringPassword},
			{"max_queue", spotify.maxQueue},
			{"path", spotify.path},
			{"start_client", spotify.startClient},
			{"username", spotify.username}
		}}
	};
}

void Settings::save()
{
	mutex.lock();

	auto path = filePath();

	if (!std::experimental::filesystem::exists(path))
		std::experimental::filesystem::create_directories(path);

	std::ofstream file(fileName());
	file << std::setw(4) << toJson();
	file.close();

	mutex.unlock();
}

void Settings::removeClient()
{
	account.clientId = std::string();
	account.clientSecret = std::string();
}

void Settings::removeTokens()
{
	account.accessToken = std::string();
	account.refreshToken = std::string();
}

bool Settings::darkTheme() const
{
	return general.stylePalette == PaletteDark;
}

void Settings::setDarkTheme(bool value)
{
	// When enabling dark theme, also set style to fusion to match better
	general.style = value ? "Fusion" : std::string();
	general.stylePalette = value ? PaletteDark : PaletteApp;
}

bool Settings::hasMediaControllerSupport()
{
	// Currently, only d-bus mpris is supported under Linux
#ifdef USE_DBUS
	return true;
#else
	return false;
#endif
}

std::map<std::string, std::vector<std::string>> Settings::validate() const
{
	std::map<std::string, std::vector<std::string>> errors;

	// Refresh interval needs to be 1-60
	if (general.refreshInterval < 1 || general.refreshInterval > 60)
	{
		errors["General"] = {
			"refresh_interval"
		};
	}

	// Bitrate needs to be 96/160/320
	if (spotify.bitrate != 96 && spotify.bitrate != 160 && spotify.bitrate != 320)
	{
		errors["Spotify"] = {
			"bitrate"
		};
	}

	return errors;
}
