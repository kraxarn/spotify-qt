#pragma once

namespace spt
{
	class Spotify;
}

#include "../dialog/deviceselectdialog.hpp"
#include "../enum/followtype.hpp"
#include "lib/settings.hpp"
#include "lib/vector.hpp"
#include "artist.hpp"
#include "audiofeatures.hpp"
#include "device.hpp"
#include "playback.hpp"
#include "playlist.hpp"
#include "searchresults.hpp"
#include "user.hpp"

#include <QCoreApplication>
#include <QDateTime>
#include <QDesktopServices>
#include <QInputDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QProcessEnvironment>
#include <QSettings>
#include <QString>
#include <QVector>
#include <QtNetwork>

namespace spt
{
	class Spotify: public QObject
	{
	Q_OBJECT

	public:
		explicit Spotify(lib::settings &settings, QObject *parent = nullptr);

		//region Albums

		void albumTracks(const QString &albumId,
			const std::function<void(const std::vector<Track> &tracks)> &callback);

		void getAlbum(const QString &id,
			const std::function<void(const Album &album)> &callback);

		void albumTracks(const QString &albumId, const QString &albumName, int offset,
			const std::function<void(const std::vector<Track> &tracks)> &callback);

		//endregion

		//region Artists

		void artist(const QString &artistId,
			const std::function<void(const spt::Artist &artist)> &callback);

		void topTracks(const spt::Artist &artist,
			const std::function<void(const std::vector<spt::Track> &tracks)> &callback);

		void relatedArtists(const spt::Artist &artist,
			const std::function<void(const std::vector<spt::Artist> &artists)> &callback);

		void albums(const spt::Artist &artist,
			const std::function<void(const std::vector<spt::Album> &albums)> &callback);

		//endregion

		//region Browse

		void newReleases(int offset,
			const std::function<void(const std::vector<Album> &albums)> &callback);

		void newReleases(const std::function<void(const std::vector<Album> &albums)> &callback);

		//endregion

		//region Follow

		void followedArtists(const QString &offset,
			const std::function<void(const std::vector<Artist> &artists)> &callback);

		void followedArtists(const std::function<void(const std::vector<Artist> &artists)> &callback);

		void follow(FollowType type, const QList<QString> &ids,
			const std::function<void(const QString &status)> &callback);

		void unfollow(FollowType type, const QList<QString> &ids,
			const std::function<void(const QString &status)> &callback);

		void isFollowing(FollowType type, const QList<QString> &ids,
			const std::function<void(const std::vector<bool> &follows)> &callback);

		//endregion

		//region Library

		void savedAlbums(const std::function<void(const std::vector<Album> &albums)> &callback);

		void savedTracks(const std::function<void(const std::vector<Track> &albums)> &callback);

		void savedTracks(int offset,
			const std::function<void(const std::vector<Track> &albums)> &callback);

		void addSavedTrack(const QString &trackId,
			const std::function<void(const QString &result)> &callback);

		void removeSavedTrack(const QString &trackId,
			const std::function<void(const QString &result)> &callback);

		//endregion

		//region Personalization

		void topArtists(const std::function<void(const std::vector<Artist> &artists)> &callback);

		void topTracks(const std::function<void(const std::vector<Track> &tracks)> &callback);

		//endregion

		//region Player

		void currentPlayback(const std::function<void(const spt::Playback &playback)> &callback);

		void setDevice(const QString &deviceId,
			const std::function<void(const QString &status)> &callback);

		void devices(const std::function<void(const std::vector<Device> &devices)> &callback);

		void playTracks(int trackIndex, const QString &context,
			const std::function<void(const QString &result)> &callback);

		void playTracks(int trackIndex, const QList<QString> &all,
			const std::function<void(const QString &result)> &callback);

		void playTracks(const QString &context,
			const std::function<void(const QString &result)> &callback);

		void resume(const std::function<void(const QString &result)> &callback);

		void pause(const std::function<void(const QString &result)> &callback);

		void next(const std::function<void(const QString &result)> &callback);

		void previous(const std::function<void(const QString &result)> &callback);

		void seek(int position,
			const std::function<void(const QString &result)> &callback);

		void setRepeat(const QString &state,
			const std::function<void(const QString &result)> &callback);

		void setVolume(int volume,
			const std::function<void(const QString &result)> &callback);

		void setShuffle(bool enabled,
			const std::function<void(const QString &result)> &callback);

		void recentlyPlayed(const std::function<void(const std::vector<Track> &tracks)> &callback);

		void addToQueue(const QString &uri,
			const std::function<void(const QString &result)> &callback);

		//endregion

		//region Playlists

		void playlists(int offset,
			const std::function<void(const std::vector<Playlist> &playlists)> &callback);

		void playlists(const std::function<void(const std::vector<Playlist> &playlists)> &callback);

		void playlist(const QString &playlistId,
			const std::function<void(const Playlist &playlist)> &callback);

		void editPlaylist(const Playlist &playlist,
			const std::function<void(const QString &result)> &callback);

		void addToPlaylist(const QString &playlistId, const QString &trackId,
			const std::function<void(const QString &result)> &callback);

		void removeFromPlaylist(const QString &playlistId, const QString &trackId, int pos,
			const std::function<void(const QString &result)> &callback);

		//endregion

		//region Search

		void search(const QString &query,
			const std::function<void(const SearchResults &results)> &callback);

		//endregion

		//region Tracks

		void getTrack(const QString &id,
			const std::function<void(const Track &track)> &callback);

		void trackAudioFeatures(const QString &trackId,
			const std::function<void(const AudioFeatures &features)> &callback);

		//endregion

		//region User Profile

		void me(const std::function<void(const User &user)> &callback);

		//endregion

		bool isValid() const;

	private:
		qint64 lastAuth;
		QString currentDevice;
		lib::settings &settings;
		QNetworkAccessManager *networkManager;
		bool refreshValid = false;

		QNetworkRequest request(const QString &url);
		bool refresh();
		static QString followTypeString(FollowType type);
		static long secondsSinceEpoch();

		static QString errorMessage(const QUrl &url, const QByteArray &data);

		void await(QNetworkReply *reply,
			const std::function<void(const QByteArray &response)> &callback);

		void post(const QString &url,
			const std::function<void(const QString &result)> &callback);

		void get(const QString &url,
			const std::function<void(const QJsonDocument &json)> &callback);

		void put(const QString &url, QVariantMap *body,
			const std::function<void(const QString &result)> &callback);

		void put(const QString &url,
			const std::function<void(const QString &result)> &callback);

		void del(const QString &url, const QJsonDocument &json,
			const std::function<void(const QString &result)> &callback);

		void del(const QString &url,
			const std::function<void(const QString &result)> &callback);

		template<typename T>
		void get(const QString &url,
			const std::function<void(const std::vector<T> &items)> &callback)
		{
			get(url, [this, callback](const QJsonDocument &json)
			{
				auto object = json.object();
				auto items = object[object.contains("tracks")
					? "tracks"
					: object.contains("artists")
						? "artists"
						: "items"].toArray();
				std::vector<T> result;
				for (auto item : items)
					result.emplace_back(item.toObject());
				callback(result);
			});
		}
	};
}