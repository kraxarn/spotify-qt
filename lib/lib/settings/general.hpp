#pragma once

#include "lib/enum/palette.hpp"
#include "lib/enum/playlistorder.hpp"
#include "lib/enum/spotifycontext.hpp"

#include <string>
#include <vector>

namespace lib
{
	namespace set
	{
		/**
		 * General application settings
		 */
		typedef struct general
		{
			/**
			 * Palette for app
			 */
			palette style_palette = palette_app;

			/**
			 * Order to display playlists in
			 */
			lib::playlist_order playlist_order = playlist_order_default;

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
			std::vector<int> hidden_song_headers;

			/**
			 * In which contexts to show track numbers
			 */
			spotify_context track_numbers = context_all;

			/**
			 * Use bundled icons instead of system icons
			 */
			bool fallback_icons = false;

			/**
			 * Use a monospace font for time progress
			 */
			bool fixed_width_time = true;

			/**
			 * Enable d-bus MPRIS
			 */
			bool media_controller = false;

			/**
			 * Use PulseAudio for volume control
			 */
			bool pulse_volume = false;

			/**
			 * Use relative time for added dates
			 */
			bool relative_added = true;

			/**
			 * Show changelog on start after update
			 */
			bool show_changelog = true;

			/**
			 * Show context information
			 */
			bool show_context_info = true;

			/**
			 * Play tracks in Spotify order instead of queueing all items
			 */
			bool spotify_playback_order = true;

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
			 * Show notifications as desktop notifications
			 */
			bool tray_notifications = false;

			/**
			 * Volume last set
			 * @note Should be 0-20, representing 0-100% as steps of 5
			 */
			int last_volume = 0;

			/**
			 * How often to refresh in seconds
			 */
			int refresh_interval = 3;

			/**
			 * How to resize track list headers
			 * @note QHeaderView::ResizeToContents by default in Qt
			 */
			int song_header_resize_mode = 3;

			/**
			 * Which column to sort by in track list
			 */
			int song_header_sort_by = -1;
		} general;
	}
}
