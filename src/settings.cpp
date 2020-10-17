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

void Settings::fromJson(const QJsonObject &json)
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

	QJsonParseError error{};
	auto jsonDoc = QJsonDocument::fromJson(data, &error);
	file.close();
	if (error.error != QJsonParseError::NoError)
	{
		qDebug() << "error while reading json settings:" << error.errorString();
		return;
	}
	auto json = jsonDoc.object();

	QVector<int> hiddenSongHeaders;
	for (auto val : json["General"].toObject()["hidden_song_headers"].toArray())
		hiddenSongHeaders.append(val.toInt());

	QStringList customPlaylistOrder;
	for (auto val : json["General"].toObject()["custom_playlist_order"].toArray())
		customPlaylistOrder.append(val.toString());

	auto a = json["Account"].toObject();
	account.accessToken = a["access_token"].toString();
	account.clientId = a["client_id"].toString();
	account.clientSecret = a["client_secret"].toString();
	account.refreshToken = a["refresh_token"].toString();

	auto g = json["General"].toObject();
	general.customPlaylistOrder = customPlaylistOrder;
	general.fallbackIcons = g["fallback_icons"].toBool(false);
	general.fixedWidthTime = g["fixed_width_time"].toBool(true);
	general.hiddenSongHeaders = hiddenSongHeaders;
	general.lastPlaylist = g["last_playlist"].toString();
	general.lastVersion = g["last_version"].toString();
	general.lastVolume = g["last_volume"].toInt();
	general.mediaController = g["media_controller"].toBool(hasMediaControllerSupport());
	general.playlistOrder = (PlaylistOrder) g["playlist_order"].toInt(PlaylistOrderDefault);
	general.pulseVolume = g["pulse_volume"].toBool(false);
	general.refreshInterval = g["refresh_interval"].toInt(3);
	general.showChangelog = g["show_changelog"].toBool(true);
	general.showContextInfo = g["show_context_info"].toBool(true);
	general.singleClickPlay = g["single_click_play"].toBool(false);
	general.songHeaderResizeMode = g["song_header_resize_mode"].toInt(QHeaderView::ResizeToContents);
	general.songHeaderSortBy = g["song_header_sort_by"].toInt(-1);
	general.spotifyPlaybackOrder = g["spotify_playback_order"].toBool(false);
	general.style = g["style"].toString();
	general.stylePalette = (Palette) g["style_palette"].toInt(PaletteApp);
	general.trackNumbers = (SpotifyContext) g["track_numbers"].toInt(ContextAll);
	general.trayAlbumArt = g["tray_album_art"].toBool(false);
	general.trayIcon = g["tray_icon"].toBool(true);
	general.trayLightIcon = g["tray_light_icon"].toBool(false);
	general.trayNotifications = g["tray_notifications"].toBool(false);

	auto s = json["Spotify"].toObject();
	spotify.alwaysStart = s["always_start"].toBool(true);
	spotify.bitrate = s["bitrate"].toInt(320);
	spotify.globalConfig = s["global_config"].toBool(false);
	spotify.path = s["path"].toString();
	spotify.startClient = s["start_client"].toBool(false);
	spotify.username = s["username"].toString();
	spotify.keyringPassword = s["keyring_password"].toBool(false);
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

	return QJsonObject(
		{
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
				{"bitrate", spotify.bitrate},
				{"global_config", spotify.globalConfig},
				{"keyring_password", spotify.keyringPassword},
				{"path", spotify.path},
				{"start_client", spotify.startClient},
				{"username", spotify.username}
			})
		}
	);
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
#ifdef Q_OS_LINUX
	return true;
#else
	return false;
#endif
}