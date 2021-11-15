#include "lib/settings/account.hpp"

void lib::setting::to_json(nlohmann::json &j, const account &a)
{
	j = nlohmann::json{
		{"access_token", a.access_token},
		{"client_id", a.client_id},
		{"client_secret", a.client_secret},
		{"last_refresh", a.last_refresh},
		{"refresh_token", a.refresh_token},
	};
}

void lib::setting::from_json(const nlohmann::json &j, account &a)
{
	if (!j.is_object())
	{
		return;
	}

	lib::json::get(j, "access_token", a.access_token);
	lib::json::get(j, "client_id", a.client_id);
	lib::json::get(j, "client_secret", a.client_secret);
	lib::json::get(j, "last_refresh", a.last_refresh);
	lib::json::get(j, "refresh_token", a.refresh_token);
}
