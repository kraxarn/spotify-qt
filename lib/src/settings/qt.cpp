#include "lib/settings/qt.hpp"

void lib::setting::to_json(nlohmann::json &j, const qt &q)
{
	j = nlohmann::json{
		{"system_title_bar", q.system_title_bar},
		{"custom_font", q.custom_font},
	};
}

void lib::setting::from_json(const nlohmann::json &j, qt &q)
{
	if (!j.is_object())
	{
		return;
	}

	lib::json::get(j, "system_title_bar", q.system_title_bar);
	lib::json::get(j, "custom_font", q.custom_font);
}
