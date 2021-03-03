#include "spotifyapi.hpp"

// Currently unavailable:
// playlists/{playlist_id}/followers
// playlists/{playlist_id}/followers/contains

void Spotify::followedArtists(const QString &offset,
	const std::function<void(const std::vector<Artist> &artists)> &callback)
{
	get(QString("me/following?type=artist&limit=50%1")
		.arg(offset.isEmpty() ? "" : QString("&after=%1")
			.arg(offset)), [this, callback](const QJsonDocument &json)
	{
		auto items = json["items"].toArray();
		std::vector<Artist> artists;
		artists.reserve(items.size());

		for (auto item : items)
			artists.emplace_back(item.toObject());

		if (!json["cursors"].isNull() && !json["cursors"].toObject()["after"].isNull())
		{
			followedArtists(json["cursors"].toObject()["after"].toString(),
				[callback, artists](const std::vector<Artist> &result)
				{
					callback(lib::vector::combine(artists, result));
				});
		}
	});

}

void Spotify::followedArtists(const std::function<void(const std::vector<Artist> &)> &callback)
{
	followedArtists(QString(), callback);
}

void Spotify::follow(FollowType type, const QList<QString> &ids,
	const std::function<void(const QString &status)> &callback)
{
	put(QString("me/following?type=%1&ids=%2")
			.arg(followTypeString(type)).arg(ids.join(',')),
		nullptr, callback);
}

void Spotify::unfollow(FollowType type, const QList<QString> &ids,
	const std::function<void(const QString &status)> &callback)
{
	del(QString("me/following?type=%1&ids=%2")
		.arg(followTypeString(type)).arg(ids.join(',')), callback);
}

void Spotify::isFollowing(FollowType type,
	const QList<QString> &ids,
	const std::function<void(const std::vector<bool> &)> &callback)
{
	get(QString("me/following/contains?type=%1&ids=%2")
		.arg(followTypeString(type))
		.arg(ids.join(',')), [callback](const QJsonDocument &json)
	{
		std::vector<bool> values;
		for (auto value : json.array())
			values.push_back(value.toBool());
		callback(values);
	});
}
