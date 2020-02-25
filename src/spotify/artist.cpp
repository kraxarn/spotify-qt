#include "artist.hpp"

using namespace spt;

Artist::Artist(const QJsonObject &json)
{
	followers	= json["followers"].toObject()["total"].toInt();
	popularity	= json["popularity"].toInt();
	genres		= QStringList();
	name		= json["name"].toString();
	image		= json["images"].toArray()[1].toObject()["url"].toString();
	for (auto genre : json["genres"].toArray())
		genres.append(genre.toString());
}