#include "settings.hpp"

Settings *Settings::set = nullptr;

Settings::Settings()
{
	settings = nullptr;
	load();
}

Settings::~Settings()
{
	delete settings;
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

void Settings::load()
{
	QFile file(fileName());
	file.open(QIODevice::ReadOnly);
	auto data = file.readAll();
	if (data.isEmpty())
	{
		qDebug() << "warning: json config in" << fileName() << "is empty";
		file.close();
		return;
	}

	QJsonParseError error;
	auto json = QJsonDocument::fromJson(data, &error);
	file.close();
	if (error.error != QJsonParseError::NoError)
	{
		qDebug() << "error while reading json settings:" << error.errorString();
		return;
	}

	QVector<int> hiddenSongHeaders;
	for (auto val : json["General"].toObject()["hidden_song_headers"].toArray())
		hiddenSongHeaders.append(val.toInt());

	auto a = json["Account"].toObject();
	account.accessToken = a["access_token"].toString();
	account.refreshToken = a["refresh_token"].toString();
	account.clientId = a["client_id"].toString();
	account.clientSecret = a["client_secret"].toString();

	auto g = json["General"];
	general.style = g["style"].toString();
	general.lastPlaylist = g["last_playlist"].toString();
	general.lastVersion = g["last_version"].toString();
	general.pulseVolume = g["pulse_volume"].toBool(false);
	general.mediaController = g["media_controller"].toBool(hasMediaControllerSupport());
	general.spotifyPlaybackOrder = g["spotify_playback_order"].toBool(false);
	general.trayIcon = g["tray_icon"].toBool(false);
	general.trayNotifications = g["tray_notifications"].toBool(false);
	general.trayLightIcon = g["tray_light_icon"].toBool(false);
	general.showChangelog = g["show_changelog"].toBool(true);
	general.fallbackIcons = g["fallback_icons"].toBool(false);
	general.fixedWidthTime = g["fixed_width_time"].toBool(true);
	general.stylePalette = (Palette) g["style_palette"].toInt(paletteApp);
	general.hiddenSongHeaders = hiddenSongHeaders;
	general.songHeaderResizeMode = g["song_header_resize_mode"].toInt(QHeaderView::ResizeToContents);
	general.songHeaderSortBy = g["song_header_sort_by"].toInt(-1);
	general.refreshInterval = g["refresh_interval"].toInt(3);

	auto s = json["Spotify"];
	spotify.path = s["path"].toString();
	spotify.username = s["username"].toString();
	spotify.startClient = s["start_client"].toBool(false);
	spotify.globalConfig = s["global_config"].toBool(false);
	spotify.bitrate = s["bitrate"].toInt(320);
}

void Settings::save()
{
	mutex.lock();

	QJsonArray jsonHiddenSongHeaders;
	for (auto &val : general.hiddenSongHeaders)
		jsonHiddenSongHeaders.append(val);

	QJsonObject json({
		QPair<QString, QJsonObject>("Account", {
			{"access_token", account.accessToken},
			{"client_id", account.clientId},
			{"client_secret", account.clientSecret},
			{"refresh_token", account.refreshToken}
		}),
		QPair<QString, QJsonObject>("General", {
			{"fallback_icons", general.fallbackIcons},
			{"fixed_width_time", general.fixedWidthTime},
			{"hidden_song_headers", jsonHiddenSongHeaders},
			{"last_playlist", general.lastPlaylist},
			{"last_version", general.lastVersion},
			{"media_controller", general.mediaController},
			{"pulse_volume", general.pulseVolume},
			{"refresh_interval", general.refreshInterval},
			{"show_changelog", general.showChangelog},
			{"song_header_resize_mode", general.songHeaderResizeMode},
			{"song_header_sort_by", general.songHeaderSortBy},
			{"spotify_playback_order", general.spotifyPlaybackOrder},
			{"style", general.style},
			{"style_palette", general.stylePalette},
			{"tray_icon", general.trayIcon},
			{"tray_light_icon", general.trayLightIcon},
			{"tray_notifications", general.trayNotifications}
		}),
		QPair<QString, QJsonObject>("Spotify", {
			{"bitrate", spotify.bitrate},
			{"global_config", spotify.globalConfig},
			{"path", spotify.path},
			{"start_client", spotify.startClient},
			{"username", spotify.username},

		})
	});

	QDir::root().mkpath(Settings::filePath());
	QFile file(fileName());
	file.open(QIODevice::WriteOnly);
	file.write(QJsonDocument(json).toJson(QJsonDocument::Indented));
	file.close();
	mutex.unlock();
}

QJsonDocument Settings::legacyToJson()
{
	if (settings == nullptr)
		settings = new QSettings();

	QJsonArray jsonHiddenSongHeaders;
	for (auto &val : hiddenSongHeaders())
		jsonHiddenSongHeaders.append(val);

	QJsonObject json({
		QPair<QString, QJsonObject>("Account", {
			{ "access_token", accessToken() },
			{ "refresh_token", refreshToken() },
			{ "client_id", clientId() },
			{ "client_secret", clientSecret() }
		}),
		QPair<QString, QJsonObject>("General", {
			{ "style", style() },
			{ "pulse_volume", pulseVolume() },
			{ "last_playlist", lastPlaylist() },
			{ "style_palette", stylePalette() },
			{ "media_controller", mediaController() },
			{ "spotify_playback_order", sptPlaybackOrder() },
			{ "hidden_song_headers", jsonHiddenSongHeaders },
			{ "tray_icon", trayIcon() },
			{ "tray_notifications", trayNotifications() },
			{ "tray_light_icon", trayLightIcon() },
			{ "song_header_resize_mode", songHeaderResizeMode() },
			{ "song_header_sort_by", songHeaderSortBy() },
			{ "refresh_interval", refreshInterval() },
			{ "last_version", lastVersion() },
			{ "show_changelog", showChangelog() },
			{ "fallback_icons", useFallbackIcons() },
			{ "fixed_width_time", fixedWidthTime() }
		}),
		QPair<QString, QJsonObject>("Spotify", {
			{ "path", sptPath() },
			{ "start_client", sptStartClient() },
			{ "username", sptUser() },
			{ "bitrate", sptBitrate() },
			{ "global_config", sptGlobalConfig() }
		})
	});

	return QJsonDocument(json);
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

QString Settings::accessToken()
{
	return settings->value("AccessToken").toString();
}

QString Settings::refreshToken()
{
	return settings->value("RefreshToken").toString();
}

QString Settings::clientId()
{
	return settings->value("ClientId").toString();
}

QString Settings::clientSecret()
{
	return settings->value("ClientSecret").toString();
}

QString Settings::style()
{
	return settings->value("Style").toString();
}

QString Settings::sptPath()
{
	return settings->value("Spotify/Path").toString();
}

bool Settings::sptStartClient()
{
	return settings->value("Spotify/StartClient").toBool();
}

QString Settings::sptUser()
{
	return settings->value("Spotify/Username").toString();
}

int Settings::sptBitrate()
{
	return settings->value("Spotify/Bitrate").toInt();
}

bool Settings::pulseVolume()
{
	return settings->value("PulseVolume").toBool();
}

QString Settings::lastPlaylist()
{
	return settings->value("LastPlaylist").toString();
}

Settings::Palette Settings::stylePalette()
{
	return (Settings::Palette) settings->value("StylePalette").toInt();
}

bool Settings::mediaController()
{
	return settings->value("MediaController", hasMediaControllerSupport()).toBool();
}

bool Settings::darkTheme() const
{
	return general.stylePalette == paletteDark;
}

void Settings::setDarkTheme(bool value)
{
	// When enabling dark theme, also set style to fusion to match better
	general.style = value ? "Fusion" : QString();
	general.stylePalette = value ? paletteDark : paletteApp;
}

bool Settings::sptPlaybackOrder()
{
	return settings->value("SpotifyPlaybackOrder", false).toBool();
}

QList<int> Settings::hiddenSongHeaders()
{
	QList<int> list;
	auto headers = settings->value("HiddenSongHeaders").toString();
	if (!headers.isEmpty())
	{
		for (auto &value : headers.split(','))
			list.append(value.toInt());
	}
	return list;
}

bool Settings::trayIcon()
{
	return settings->value("TrayIcon", false).toBool();
}

bool Settings::trayNotifications()
{
	return settings->value("TrayNotifications", false).toBool();
}

bool Settings::trayLightIcon()
{
	return settings->value("TrayLightIcon", false).toBool();
}

QHeaderView::ResizeMode Settings::songHeaderResizeMode()
{
	return (QHeaderView::ResizeMode) settings->value(
		"SongHeaderResizeMode", QHeaderView::ResizeToContents).toInt();
}

int Settings::songHeaderSortBy()
{
	return settings->value("SongHeaderSortBy", -1).toInt();
}

int Settings::refreshInterval()
{
	return settings->value("RefreshInterval", 3).toInt();
}

bool Settings::sptGlobalConfig()
{
	return settings->value("Spotify/GlobalConfig", false).toBool();
}

QString Settings::lastVersion()
{
	return settings->value("LastVersion").toString();
}

bool Settings::showChangelog()
{
	return settings->value("ShowChangelog", true).toBool();
}

bool Settings::useFallbackIcons()
{
	return settings->value("FallbackIcons", false).toBool();
}

bool Settings::fixedWidthTime()
{
	return settings->value("FixedWidthTime", false).toBool();
}

bool Settings::hasMediaControllerSupport()
{
	// Currently, only d-bus mpris is supported under Linux
#ifdef Q_OS_LINUX
	return true;
#else
	return false;
#endif
}