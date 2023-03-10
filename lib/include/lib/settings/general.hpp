#pragma once

#include "lib/json.hpp"
#include "lib/enum/palette.hpp"
#include "lib/enum/playlistorder.hpp"
#include "lib/enum/spotifycontext.hpp"
#include "lib/enum/resizemode.hpp"

#include <string>
#include <vector>
#include <unordered_set>

namespace lib
{
	namespace setting
	{
		/**
		 * General application settings
		 */
		using general = struct general
		{
			/**
			 * Palette for app
			 */
			lib::palette style_palette = lib::palette::app;

			/**
			 * Order to display playlists in
			 */
			lib::playlist_order playlist_order = lib::playlist_order::none;

			/**
			 * Last viewed playlist
			 */
			std::string last_playlist;

			/**
			 * Last used version
			 */
			std::string last_version;

			/**
			 * Style name
			 */
			std::string style;

			/**
			 * IDs for playlists in order
			 */
			std::vector<std::string> custom_playlist_order;

			/**
			 * Indices of track list headers to hide
			 */
			std::unordered_set<int> hidden_song_headers;

			/**
			 * In which contexts to show track numbers
			 */
			lib::spotify_context track_numbers = lib::spotify_context::all;

			/**
			 * Use bundled icons instead of system icons
			 */
			bool fallback_icons = false;

			/**
			 * Enable d-bus MPRIS
			 */
			bool media_controller = false;

			/**
			 * Use relative time for added dates
			 */
			bool relative_added = true;

			/**
			 * Show changelog on start after update
			 */
			bool show_changelog = true;

			/**
			 * Show album art in tray icon
			 */
			bool tray_album_art = false;

			/**
			 * Enable tray icon
			 */
			bool tray_icon = true;

			/**
			 * Invert tray icon
			 */
			bool tray_light_icon = false;

			/**
			 * How often to refresh in seconds
			 */
			int refresh_interval = 3;

			/**
			 * How to resize track list headers
			 */
			lib::resize_mode track_list_resize_mode = lib::resize_mode::auto_size;

			/**
			 * Which column to sort by in track list
			 */
			int song_header_sort_by = -1;

			/**
			 * ID of last selected device
			 */
			std::string last_device;

			/**
			 * Show a notification on track change
			 */
			bool notify_track_change = false;

			/**
			 * Close to system tray instead of quitting when the close button is pressed
			 */
			bool close_to_tray = false;

			/**
			 * Check for updates on start
			 */
			bool check_for_updates = true;

			/**
			 * Create a native window handle
			 * @note Always enabled under Linux/Wayland
			 */
			bool native_window = false;

			/**
			 * Register media hotkeys under Windows
			 */
			bool media_hotkeys = true;
		};

		void to_json(nlohmann::json &j, const general &g);

		void from_json(const nlohmann::json &j, general &g);
	}
}
