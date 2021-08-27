#include "lib/search/link.hpp"

auto lib::ddg::link::get_url() const -> std::string
{
	std::string base_url;

	switch (data_type)
	{
		case artist_profile::twitter:
			base_url = "https://twitter.com/{}";
			break;

		case artist_profile::instagram:
			base_url = "https://instagram.com/{}";
			break;

		case artist_profile::facebook:
			base_url = "https://facebook.com/{}";
			break;

		case artist_profile::youtube:
			base_url = "https://youtube.com/channel/{}";
			break;

		case artist_profile::soundcloud:
			base_url = "https://soundcloud.com/{}";
			break;

		default:
			return base_url;
	}

	return lib::fmt::format(base_url, value);
}

void lib::ddg::from_json(const nlohmann::json &j, link &l)
{
	if (!j.is_object())
	{
		return;
	}

	j.at("label").get_to(l.label);
	j.at("value").get_to(l.value);

	const auto &data_type = j.at("data_type").get<std::string>();
	if (data_type == "string")
	{
		l.data_type = artist_profile::string;
	}
	else if (data_type == "twitter_profile")
	{
		l.data_type = artist_profile::twitter;
	}
	else if (data_type == "instagram_profile")
	{
		l.data_type = artist_profile::instagram;
	}
	else if (data_type == "facebook_profile")
	{
		l.data_type = artist_profile::facebook;
	}
	else if (data_type == "youtube_channel")
	{
		l.data_type = artist_profile::youtube;
	}
	else if (data_type == "soundcloud_id")
	{
		l.data_type = artist_profile::soundcloud;
	}
	else
	{
		l.data_type = artist_profile::unknown;
	}
}
