#include "lib/settings/qt.hpp"

void lib::setting::to_json(nlohmann::json &j, const qt &q)
{
	j = nlohmann::json{
		{"album_shape",      q.album_shape},
		{"album_size",       q.album_size},
		{"custom_font_name", q.custom_font_name},
		{"custom_font_size", q.custom_font_size},
		{"library_layout",   q.library_layout},
		{"mirror_title_bar", q.mirror_title_bar},
		{"system_title_bar", q.system_title_bar},
		{"toolbar_position", q.toolbar_position},
		{"track_title",      q.track_title},
	};
}

void lib::setting::from_json(const nlohmann::json &j, qt &q)
{
	if (!j.is_object())
	{
		return;
	}

	lib::json::get(j, "album_shape", q.album_shape);
	lib::json::get(j, "album_size", q.album_size);
	lib::json::get(j, "custom_font_name", q.custom_font_name);
	lib::json::get(j, "custom_font_size", q.custom_font_size);
	lib::json::get(j, "library_layout", q.library_layout);
	lib::json::get(j, "mirror_title_bar", q.mirror_title_bar);
	lib::json::get(j, "system_title_bar", q.system_title_bar);
	lib::json::get(j, "toolbar_position", q.toolbar_position);
	lib::json::get(j, "track_title", q.track_title);
}
