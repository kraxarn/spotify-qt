#include "spotifyapi.hpp"

// Currently unavailable:
// playlists/{playlist_id}/followers
// playlists/{playlist_id}/followers/contains

QVector<Artist> Spotify::followedArtists(const QString &offset)
{
	auto json = getAsObject(QString("me/following?type=artist&limit=50%1")
		.arg(offset.isEmpty() ? "" : QString("&after=%1")
			.arg(offset)))["artists"].toObject();
	auto items = json["items"].toArray();
	QVector<Artist> artists;
	artists.reserve(items.size());
	for (auto item : items)
		artists.append(Artist(item.toObject()));
	if (!json["cursors"].isNull() && !json["cursors"].toObject()["after"].isNull())
		artists << followedArtists(json["cursors"].toObject()["after"].toString());
	return artists;
}

void Spotify::follow(FollowType type, const QList<QString> &ids)
{
	put(QString("me/following?type=%1&ids=%2")
		.arg(followTypeString(type)).arg(ids.join(',')));
}

void Spotify::unfollow(FollowType type, const QList<QString> &ids)
{
	del(QString("me/following?type=%1&ids=%2")
		.arg(followTypeString(type)).arg(ids.join(',')), {});
}

void Spotify::isFollowing(FollowType type, const QList<QString> &ids,
	lib::callback<std::vector<bool>> &callback)
{
	get(QString("me/following/contains?type=%1&ids=%2")
		.arg(followTypeString(type))
		.arg(ids.join(',')), [callback](const QJsonArray &json)
	{
		std::vector<bool> values;
		for (auto value : json)
			values.push_back(value.toBool());
		callback(values);
	});
}
