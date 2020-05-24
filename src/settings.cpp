#include "settings.hpp"

Settings::Settings()
{
	// Settings is legacy, only instance when needed
	settings = nullptr;

	// Default values
#ifdef Q_OS_LINUX
	general.mediaController = true;
#endif
	general.showChangelog = true;
	general.stylePalette = paletteApp;
	general.songHeaderResizeMode = (int) QHeaderView::ResizeToContents;
	general.songHeaderSortBy = -1;
	general.refreshInterval = 3;
	spotify.bitrate = 320;

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

void Settings::load()
{
	std::ifstream file(fileName().toStdString());
	std::string data((std::istreambuf_iterator<char>(file)), (std::istreambuf_iterator<char>()));
	if (data.empty())
	{
		qDebug() << "warning: json config in" << fileName() << "is empty";
		file.close();
		return;
	}
	auto json = nlohmann::json::parse(data);
	file.close();

	auto a = json["Account"];
	account.accessToken = a["access_token"];
	account.refreshToken = a["refresh_token"];
	account.clientId = a["client_id"];
	account.clientSecret = a["client_secret"];

	auto g = json["General"];
	general.style = g["style"];
	general.lastPlaylist = g["last_playlist"];
	general.lastVersion = g["last_version"];
	general.pulseVolume = g["pulse_volume"];
	general.mediaController = g["media_controller"];
	general.spotifyPlaybackOrder = g["spotify_playback_order"];
	general.trayIcon = g["tray_icon"];
	general.trayNotifications = g["tray_notifications"];
	general.trayLightIcon = g["tray_light_icon"];
	general.showChangelog = g["show_changelog"];
	general.fallbackIcons = g["fallback_icons"];
	general.fixedWidthTime = g["fixed_width_time"];
	general.stylePalette = g["style_palette"];
	//general.hiddenSongHeaders = g["hidden_song_headers"].get<std::vector<int>>();
	general.songHeaderResizeMode = g["song_header_resize_mode"];
	general.songHeaderSortBy = g["song_header_sort_by"];
	general.refreshInterval = g["refresh_interval"];

	//for (auto &val : g["hidden_song_headers"].items())1
	//	general.hiddenSongHeaders.push_back(val.value());

	auto s = json["Spotify"];
	spotify.path = s["path"];
	spotify.username = s["username"];
	spotify.startClient = s["start_client"];
	spotify.globalConfig = s["global_config"];
	spotify.bitrate = s["bitrate"];
}

void Settings::save()
{
	nlohmann::json json = {
		{"Account", {
			{"access_token", account.accessToken},
			{"refresh_token", account.refreshToken},
			{"client_id", account.clientId},
			{"client_secret", account.clientSecret}
		}},
		{"General", {
			{"style", general.style},
			{"pulse_volume", general.pulseVolume},
			{"last_playlist", general.lastPlaylist},
			{"style_palette", general.stylePalette},
			{"media_controller", general.mediaController},
			{"spotify_playback_order", general.spotifyPlaybackOrder},
			{"hidden_song_headers", general.hiddenSongHeaders},
			{"tray_icon", general.trayIcon},
			{"tray_notifications", general.trayNotifications},
			{"tray_light_icon", general.trayLightIcon},
			{"song_header_resize_mode", general.songHeaderResizeMode},
			{"song_header_sort_by", general.songHeaderSortBy},
			{"refresh_interval", general.refreshInterval},
			{"last_version", general.lastVersion},
			{"show_changelog", general.showChangelog},
			{"fallback_icons", general.fallbackIcons},
			{"fixed_width_time", general.fixedWidthTime}
		}},
		{"Spotify", {
			{"path", spotify.path},
			{"start_client", spotify.startClient},
			{"username", spotify.username},
			{"bitrate", spotify.bitrate},
			{"global_config", spotify.globalConfig}
		 }}
	 };
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
	account.clientId = std::string();
	account.clientSecret = std::string();
}

void Settings::removeTokens()
{
	account.accessToken = std::string();
	account.refreshToken = std::string();
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
#ifdef Q_OS_LINUX
	auto defVal = true;
#else
	auto defVal = false;
#endif
	return settings->value("MediaController", defVal).toBool();
}

bool Settings::darkTheme()
{
	return general.stylePalette == paletteDark;
}

void Settings::setDarkTheme(bool value)
{
	// When enabling dark theme, also set style to fusion to match better
	general.style = value ? "Fusion" : std::string();
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