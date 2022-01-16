Versions use major.minor scheming. New versions are released together with spotify-qt.

* Major releases contain breaking changes and may require changes to be made.
* Minor releases contain minor changes and bug fixes and should never require changes to be made.
* Pre-release and beta releases may contain breaking changes, even if it's a minor release.

## v0.6 (spotify-qt v3.8)

* `palette`, `resize_mode`, and `spotify_context` are now enum classes.
* Renamed `log::dev` to `log::debug`.
* Added `enums` for serializing, and deserializing, enums.
* Added `optional` as a basic `std::optional` implementation.
* Added `uri` for basic URI manipulation.
* Added `gh::api` as a GitHub API wrapper.
* Added `ddg::api` as a DuckDuckGo API wrapper.
* Added `cache::get_album_image_path`.
* Added `cache::get_album` and `cache::set_album`.
* Added `artist_profile`, `audio_quality`, and `media_type` enums.
* Added `json::find_item`, and `json::set`.
* Added `qt.custom_font`, and `qt.mirror_title_bar`.
* Added `spt::api::follow_playlist` and `spt::api::unfollow_playlist`.
* Added `spt::api::is_following_playlist`.
* Added `spt::api::create_playlist`.
* Added `spt::api::show` and `spt::api::show_episodes`.
* Added `spt::episode` and `spt::show`.
* Removed `cipher`.
* Removed `ghc::filesystem` support for `fmt::format`.
* Removed `settings::qt_const` (now dynamically created).
* Removed `settings::validate`, and `settings::set_value`
* Removed `general.pulse_volume` (broken with PipeWire).
* Removed `general.tray_notifications` (use in-app notifications).
* Removed `strings::index_of`, and `strings::last_index_of`.

## v0.5 (spotify-qt v3.7)

* `cache` is now abstract, and previous implementation has been moved to `json_cache`.
* Reworked `audio_feature`.
* Version is now defined in `CMakeLists.txt` (still as `LIB_VERSION`).
* `spt::album` and `spt::artist` now inherits `spt::entity`.
* Added `crash_handler` (GCC amd64 only).
* Added `qt` settings.
* Added `http_client`, and `qt::http_client`.
* Added `spt::auth`, and `qt::spt::auth`.
* Added `spt::playlist::is_up_to_date`.
* Added `track_info` and `lyrics`.
* Added `image`.
* Added `strings::replace_all`.
* Added `lib::system::window_sytem`.
* Added `vector::index_of`.
* Added `qt::system_info`.
* Moved `spotify_error` to `spt::error`.
* Moved `seconds_since_epoch` to `date_time`.
* Moved `lib::qt::sytem` to `lib::system` (no longer depends on Qt).
* Removed `general.show_context_info` (always enabled).
* Removed `vector::remove_if`.

## v0.4 (spotify-qt v3.6)

* Paths in `paths` now return `filesystem::path` instead of `std::string`.
* Setting `general.hidden_song_headers` is now a `std::unordered_set` instead of `std::vector`.
* Version is now defined in `lib/libversion.hpp`.
* Reworked `cache`.
* Added enums: `album_group`, `client_type`, `follow_type`, `repeat_state`.
* Added `general.last_device` setting.
* Added Spotify classes: `album`, `artist`, `audio_features`, `device`, `playback`, `playlist`,
  `search_results`, `track`, `user`.
* Added Spotify helper classes: `context`, `playlist_details`, `saved_album`.
* Added `callback<T>` for asynchronous callbacks.
* Added `entity` type for saving id and name of various entities.
* Added `api` as a Spotify Web API wrapper.
* Added `json`.
* Added `random`.
* Removed deprecated `locale`.

## v0.3 (spotify-qt v3.5)

* All headers are now in an "include" folder.
* `general.song_header_resize_mode` is now `general.track_list_resize_mode`.
* Added `cipher`.
* Added `fmt::time` and `fmt::size`.
* Added `vector`.
* Removed `general.spotify_playback_order`.

## v0.2 (spotify-qt v3.4)

* Added `settings`.
* Added `fmt`.
* Added `log`.
* Added `date_time`.

## v0.1 (spotify-qt v3.3)

* Initial version.