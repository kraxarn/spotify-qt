#include "artist.hpp"

using namespace spt;

Artist::Artist(const QJsonObject &json)
{
	id = json["id"].toString();
	followers = json["followers"].toObject()["total"].toInt();
	popularity = json["popularity"].toInt();
	genres = QStringList();
	name = json["name"].toString();
	image = json["images"].toArray()[1].toObject()["url"].toString();

	for (auto genre : json["genres"].toArray())
		genres.append(genre.toString());

	QMapIterator<QString, QVariant> i(json["external_urls"].toObject().toVariantMap());
	while (i.hasNext())
	{
		i.next();
		externalUrls.append(QPair<QString, QString>(i.key(), i.value().toString()));
	}
}

QVector<lib::spt::track> Artist::topTracks(Spotify &spotify) const
{
	auto json = spotify.getAsJson(QString("artists/%1/top-tracks?country=from_token").arg(id));
	auto items = json["tracks"];
	QVector<lib::spt::track> tracks;
	for (auto item : items)
		tracks.append(item.get<lib::spt::track>());
	return tracks;
}

std::vector<lib::spt::album> Artist::albums(Spotify &spotify) const
{
	auto json = spotify.getAsJson(lib::fmt::format("artists/{}/albums?country=from_token",
		id.toStdString()));
	auto items = json.at("items");
	std::vector<lib::spt::album> albums;
	for (auto &item : items)
		albums.push_back(item.get<lib::spt::album>());
	return albums;
}

QJsonObject Artist::toJson() const
{
	QJsonArray jsonGenres;
	for (auto &genre : genres)
		jsonGenres.append(genre);

	return QJsonObject(
		{
			QPair<QString, int>("followers", followers),
			QPair<QString, int>("popularity", popularity),
			QPair<QString, QJsonArray>("genres", jsonGenres),
			QPair<QString, QString>("id", id),
			QPair<QString, QString>("name", name),
			QPair<QString, QString>("image", image)
		}
	);
}