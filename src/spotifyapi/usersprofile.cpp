#include "spotifyapi.hpp"

// Currently unavailable:
// users/{user_id}

void Spotify::me(const std::function<void(const User &user)> &callback)
{
	get("me", [callback](const QJsonDocument &json)
	{
		User user;
		user.displayName = json["display_name"].toString();
		user.id = json["id"].toString();
		user.image = json["images"].toArray()[2].toObject()["url"].toString();
		user.product = json["product"].toString();

		callback(user);
	});
}
