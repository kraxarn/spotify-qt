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