#include "settings.hpp"

#include <QDebug>
#include <QStandardPaths>

Settings::Settings()
{
	settings = new QSettings();
}

Settings::~Settings()
{
	delete settings;
}

QString Settings::fileName()
{
	return settings->fileName();
}

QJsonDocument Settings::legacyToJson()
{
	QSettings settings;

	QJsonObject json({
		QPair<QString, QJsonObject>("Account", {
			{ "access_token", settings.value("AccessToken").toString() },
			{ "refresh_token", settings.value("RefreshToken").toString() },
			{ "client_id", settings.value("ClientId").toString() },
			{ "client_secret", settings.value("ClientSecret").toString() }
		}),
		QPair<QString, QJsonObject>("General", {
			{ "style", settings.value("Style").toString() },
			{ "pulse_volume", settings.value("PulseVolume").toBool() },
			{ "last_playlist", settings.value("LastPlaylist").toString() },
			{ "style_palette", settings.value("StylePalette").toInt() },
			{ "media_controller", settings.value("MediaController").toBool() },
			{ "spotify_playback_order", settings.value("SpotifyPlaybackOrder").toBool() },
			{ "hidden_song_headers", QJsonArray::fromStringList(
				settings.value("HiddenSongHeaders").toString().split(",")) },
			{ "tray_icon", settings.value("TrayIcon").toBool() },
			{ "tray_notifications", settings.value("TrayNotifications").toBool() },
			{ "tray_light_icon", settings.value("TrayLightIcon").toBool() },
			{ "song_header_resize_mode", settings.value("SongHeaderResizeMode").toInt() },
			{ "song_header_sort_by", settings.value("SongHeaderSortBy").toInt() },
			{ "refresh_interval", settings.value("RefreshInterval").toInt() },
			{ "last_version", settings.value("LastVersion").toString() },
			{ "show_changelog", settings.value("ShowChangelog").toBool() },
			{ "fallback_icons", settings.value("FallbackIcons").toBool() },
			{ "fixed_width_time", settings.value("FixedWidthTime").toBool() }
		}),
		QPair<QString, QJsonObject>("Spotify", {
			{ "path", settings.value("Spotify/Path").toString() },
			{ "start_client", settings.value("Spotify/StartClient").toBool() },
			{ "username", settings.value("Spotify/Username").toString() },
			{ "bitrate", settings.value("Spotify/Bitrate").toInt() },
			{ "global_config", settings.value("Spotify/GlobalConfig").toBool() }
		})
	});

	return QJsonDocument(json);
}

void Settings::removeClient()
{
	settings->remove("ClientId");
	settings->remove("ClientSecret");
}

void Settings::removeTokens()
{
	settings->remove("AccessToken");
	settings->remove("RefreshToken");
}

QString Settings::accessToken()
{
	return settings->value("AccessToken").toString();
}

void Settings::setAccessToken(QString &value)
{
	settings->setValue("AccessToken", value);
}

QString Settings::refreshToken()
{
	return settings->value("RefreshToken").toString();
}

void Settings::setRefreshToken(QString &value)
{
	settings->setValue("RefreshToken", value);
}

QString Settings::clientId()
{
	return settings->value("ClientId").toString();
}

void Settings::setClientId(const QString &value)
{
	settings->setValue("ClientId", value);
}

QString Settings::clientSecret()
{
	return settings->value("ClientSecret").toString();
}

void Settings::setClientSecret(const QString &value)
{
	settings->setValue("ClientSecret", value);
}

QString Settings::style()
{
	return settings->value("Style").toString();
}

void Settings::setStyle(const QString &value)
{
	settings->setValue("Style", value);
}

QString Settings::sptPath()
{
	return settings->value("Spotify/Path").toString();
}

void Settings::setSptPath(const QString &value)
{
	settings->setValue("Spotify/Path", value);
}

bool Settings::sptStartClient()
{
	return settings->value("Spotify/StartClient").toBool();
}

void Settings::setSptStartClient(bool value)
{
	settings->setValue("Spotify/StartClient", value);
}

QString Settings::sptUser()
{
	return settings->value("Spotify/Username").toString();
}

void Settings::setSptUser(const QString &value)
{
	settings->setValue("Spotify/Username", value);
}

int Settings::sptBitrate()
{
	return settings->value("Spotify/Bitrate").toInt();
}

void Settings::setSptBitrate(int value)
{
	settings->setValue("Spotify/Bitrate", value);
}

bool Settings::pulseVolume()
{
	return settings->value("PulseVolume").toBool();
}

void Settings::setPulseVolume(bool value)
{
	settings->setValue("PulseVolume", value);
}

QString Settings::lastPlaylist()
{
	return settings->value("LastPlaylist").toString();
}

void Settings::setLastPlaylist(const QString &value)
{
	settings->setValue("LastPlaylist", value);
}

Settings::Palette Settings::stylePalette()
{
	return (Settings::Palette) settings->value("StylePalette").toInt();
}

void Settings::setStylePalette(Settings::Palette value)
{
	settings->setValue("StylePalette", value);
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

void Settings::setMediaController(bool value)
{
	settings->setValue("MediaController", value);
}

bool Settings::darkTheme()
{
	return stylePalette() == paletteDark;
}

void Settings::setDarkTheme(bool value)
{
	// When enabling dark theme, also set style to fusion to match better
	setStyle(value ? "Fusion" : QString());
	setStylePalette(value ? paletteDark : paletteApp);
}

bool Settings::sptPlaybackOrder()
{
	return settings->value("SpotifyPlaybackOrder", false).toBool();
}

void Settings::setSptPlaybackOrder(bool value)
{
	settings->setValue("SpotifyPlaybackOrder", value);
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

void Settings::setHiddenSongHeaders(const QList<int> &values)
{
	QStringList list;
	for (auto &value : values)
		list.append(QString::number(value));
	settings->setValue("HiddenSongHeaders", list.join(','));
}

void Settings::removeHiddenSongHeader(int id)
{
	auto headers = hiddenSongHeaders();
	headers.removeOne(id);
	setHiddenSongHeaders(headers);
}

void Settings::addHiddenSongHeader(int id)
{
	auto headers = hiddenSongHeaders();
	headers.append(id);
	setHiddenSongHeaders(headers);
}

bool Settings::trayIcon()
{
	return settings->value("TrayIcon", false).toBool();
}

void Settings::setTrayIcon(bool value)
{
	settings->setValue("TrayIcon", value);
}

bool Settings::trayNotifications()
{
	return settings->value("TrayNotifications", false).toBool();
}

void Settings::setTrayNotifications(bool value)
{
	settings->setValue("TrayNotifications", value);
}

bool Settings::trayLightIcon()
{
	return settings->value("TrayLightIcon", false).toBool();
}

void Settings::setTrayLightIcon(bool value)
{
	settings->setValue("TrayLightIcon", value);
}

QHeaderView::ResizeMode Settings::songHeaderResizeMode()
{
	return (QHeaderView::ResizeMode) settings->value(
		"SongHeaderResizeMode", QHeaderView::ResizeToContents).toInt();
}

void Settings::setSongHeaderResizeMode(QHeaderView::ResizeMode value)
{
	settings->setValue("SongHeaderResizeMode", value);
}

int Settings::songHeaderSortBy()
{
	return settings->value("SongHeaderSortBy", -1).toInt();
}

void Settings::setSongHeaderSortBy(int value)
{
	settings->setValue("SongHeaderSortBy", value);
}

int Settings::refreshInterval()
{
	return settings->value("RefreshInterval", 3).toInt();
}

void Settings::setRefreshInterval(int value)
{
	settings->setValue("RefreshInterval", value);
}

bool Settings::sptGlobalConfig()
{
	return settings->value("Spotify/GlobalConfig", false).toBool();
}

void Settings::setSptGlobalConfig(bool value)
{
	settings->setValue("Spotify/GlobalConfig", value);
}

QString Settings::lastVersion()
{
	return settings->value("LastVersion").toString();
}

void Settings::setLastVersion(const QString &value)
{
	settings->setValue("LastVersion", value);
}

bool Settings::showChangelog()
{
	return settings->value("ShowChangelog", true).toBool();
}

void Settings::setShowChangelog(bool value)
{
	settings->setValue("ShowChangelog", value);
}

bool Settings::useFallbackIcons()
{
	return settings->value("FallbackIcons", false).toBool();
}

void Settings::setUseFallbackIcons(bool value)
{
	settings->setValue("FallbackIcons", value);
}

bool Settings::fixedWidthTime()
{
	return settings->value("FixedWidthTime", false).toBool();
}

void Settings::setFixedWidthTime(bool value)
{
	settings->setValue("FixedWidthTime", value);
}