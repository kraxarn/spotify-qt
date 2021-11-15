#include "lib/settings/general.hpp"

void lib::setting::to_json(nlohmann::json &j, const general &g)
{
	j = nlohmann::json{
		{"custom_playlist_order", g.custom_playlist_order},
		{"fallback_icons", g.fallback_icons},
		{"fixed_width_time", g.fixed_width_time},
		{"hidden_song_headers", g.hidden_song_headers},
		{"last_device", g.last_device},
		{"last_playlist", g.last_playlist},
		{"last_version", g.last_version},
		{"last_volume", g.last_volume},
		{"media_controller", g.media_controller},
		{"notify_track_change", g.notify_track_change},
		{"playlist_order", g.playlist_order},
		{"pulse_volume", g.pulse_volume},
		{"refresh_interval", g.refresh_interval},
		{"relative_added", g.relative_added},
		{"show_changelog", g.show_changelog},
		{"song_header_sort_by", g.song_header_sort_by},
		{"style", g.style},
		{"style_palette", g.style_palette},
		{"track_list_resize_mode", g.track_list_resize_mode},
		{"track_numbers", g.track_numbers},
		{"tray_album_art", g.tray_album_art},
		{"tray_icon", g.tray_icon},
		{"tray_light_icon", g.tray_light_icon},
	};
}

void lib::setting::from_json(const nlohmann::json &j, general &g)
{
	if (!j.is_object())
	{
		return;
	}

	lib::json::get(j, "custom_playlist_order", g.custom_playlist_order);
	lib::json::get(j, "fallback_icons", g.fallback_icons);
	lib::json::get(j, "fixed_width_time", g.fixed_width_time);
	lib::json::get(j, "hidden_song_headers", g.hidden_song_headers);
	lib::json::get(j, "last_device", g.last_device);
	lib::json::get(j, "last_playlist", g.last_playlist);
	lib::json::get(j, "last_version", g.last_version);
	lib::json::get(j, "last_volume", g.last_volume);
	lib::json::get(j, "media_controller", g.media_controller);
	lib::json::get(j, "notify_track_change", g.notify_track_change);
	lib::json::get(j, "playlist_order", g.playlist_order);
	lib::json::get(j, "pulse_volume", g.pulse_volume);
	lib::json::get(j, "refresh_interval", g.refresh_interval);
	lib::json::get(j, "relative_added", g.relative_added);
	lib::json::get(j, "show_changelog", g.show_changelog);
	lib::json::get(j, "song_header_sort_by", g.song_header_sort_by);
	lib::json::get(j, "style", g.style);
	lib::json::get(j, "style_palette", g.style_palette);
	lib::json::get(j, "track_list_resize_mode", g.track_list_resize_mode);
	lib::json::get(j, "track_numbers", g.track_numbers);
	lib::json::get(j, "tray_album_art", g.tray_album_art);
	lib::json::get(j, "tray_icon", g.tray_icon);
	lib::json::get(j, "tray_light_icon", g.tray_light_icon);
}
