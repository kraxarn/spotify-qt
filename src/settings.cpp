#include "settings.hpp"

#include <QHeaderView>

Settings::Settings()
{
	load();
}

QString Settings::fileName()
{
	return QString("%1.json").arg(QStandardPaths::writableLocation(
		QStandardPaths::AppConfigLocation));
}

QString Settings::filePath()
{
	return QFileInfo(fileName()).absolutePath();
}

template<typename T>
void Settings::setValue(const QJsonObject &json, const QString &key, T &value)
{
	if (!json.contains(key))
		return;

	if (!json[key].toVariant().canConvert<T>())
	{
		Log::warn("Settings type mismatch for key: {}", key.toStdString());
		return;
	}

	value = json[key].toVariant().value<T>();
}

void Settings::fromJson(const QJsonObject &json)
{
	auto a = json["Account"].toObject();
	auto g = json["General"].toObject();
	auto s = json["Spotify"].toObject();

	// General/HiddenSongHeaders
	if (g.contains("hidden_song_headers"))
	{
		QVector<int> hiddenSongHeaders;
		for (auto val : g["hidden_song_headers"].toArray())
			hiddenSongHeaders.append(val.toInt());
		general.hiddenSongHeaders = hiddenSongHeaders;
	}

	// General/CustomPlaylistOrder
	if (g.contains("custom_playlist_order"))
	{
		QStringList customPlaylistOrder;
		for (auto val : g["custom_playlist_order"].toArray())
			customPlaylistOrder.append(val.toString());
		general.customPlaylistOrder = customPlaylistOrder;
	}

	// Account
	setValue(a, "access_token", account.accessToken);
	setValue(a, "client_id", account.clientId);
	setValue(a, "client_secret", account.clientSecret);
	setValue(a, "refresh_token", account.refreshToken);

	// General
	setValue(g, "fallback_icons", general.fallbackIcons);
	setValue(g, "fixed_width_time", general.fixedWidthTime);
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
	setValue(s, "path", spotify.path);
	setValue(s, "start_client", spotify.startClient);
	setValue(s, "username", spotify.username);

	// Validate
	auto errors = validate();

	for (auto &error : errors)
		Log::error("One or more invalid {} settings found, restoring defaults", error.first);
	if (errors.find("General") != errors.end())
		general = General{};
	if (errors.find("Spotify") != errors.end())
		spotify = Spotify{};
}

void Settings::load()
{
	QFile file(fileName());
	file.open(QIODevice::ReadOnly);
	auto data = file.readAll();
	if (data.isEmpty())
	{
		Log::warn("JSON config in {} is empty", fileName());
		file.close();
		return;
	}

	QJsonParseError error;
	auto json = QJsonDocument::fromJson(data, &error);
	file.close();
	if (error.error != QJsonParseError::NoError)
	{
		Log::error("Unable to read JSON settings: {}", error.errorString());
		return;
	}

	fromJson(json.object());
}

QJsonObject Settings::toJson()
{
	QJsonArray jsonHiddenSongHeaders;
	for (auto &val : general.hiddenSongHeaders)
		jsonHiddenSongHeaders.append(val);

	QJsonArray jsonCustomPlaylistOrder;
	for (auto &val : general.customPlaylistOrder)
		jsonCustomPlaylistOrder.append(val);

	return QJsonObject({
		QPair<QString, QJsonObject>("Account", {
			{"access_token", account.accessToken},
			{"client_id", account.clientId},
			{"client_secret", account.clientSecret},
			{"refresh_token", account.refreshToken}
		}),
		QPair<QString, QJsonObject>("General", {
			{"custom_playlist_order", jsonCustomPlaylistOrder},
			{"fallback_icons", general.fallbackIcons},
			{"fixed_width_time", general.fixedWidthTime},
			{"hidden_song_headers", jsonHiddenSongHeaders},
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
		}),
		QPair<QString, QJsonObject>("Spotify", {
			{"always_start", spotify.alwaysStart},
			{"backend", spotify.backend},
			{"bitrate", spotify.bitrate},
			{"global_config", spotify.globalConfig},
			{"keyring_password", spotify.keyringPassword},
			{"path", spotify.path},
			{"start_client", spotify.startClient},
			{"username", spotify.username}
		})
	});
}

void Settings::save()
{
	mutex.lock();
	QDir::root().mkpath(Settings::filePath());
	QFile file(fileName());
	file.open(QIODevice::WriteOnly);
	file.write(QJsonDocument(toJson()).toJson(QJsonDocument::Indented));
	file.close();
	mutex.unlock();
}

void Settings::removeClient()
{
	account.clientId = QString();
	account.clientSecret = QString();
}

void Settings::removeTokens()
{
	account.accessToken = QString();
	account.refreshToken = QString();
}

bool Settings::darkTheme() const
{
	return general.stylePalette == PaletteDark;
}

void Settings::setDarkTheme(bool value)
{
	// When enabling dark theme, also set style to fusion to match better
	general.style = value ? "Fusion" : QString();
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
