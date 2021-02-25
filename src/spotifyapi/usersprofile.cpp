#include "spotifyapi.hpp"

// Currently unavailable:
// users/{user_id}

User Spotify::me()
{
	auto json = getAsObject("me");
	User user;
	user.displayName = json["display_name"].toString();
	user.id = json["id"].toString();
	user.image = json["images"].toArray()[2].toObject()["url"].toString();
	user.product = json["product"].toString();
	return user;
}
