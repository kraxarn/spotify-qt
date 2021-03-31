Versions use major.minor scheming. New versions are released together with spotify-qt.

* Major releases contain breaking changes and may require changes to be made.
* Minor releases contain minor changes and bug fixes and should never require changes to be made.
* Pre-release and beta releases may contain breaking changes, even if it's a minor release.

### v0.4 (spotify-qt v3.6)
* Added enums: `album_group`, `follow_type`, `repeat_state`.
* Added JSON utilities.
* Added a random number generator.
* Added Spotify classes: `album`, `artist`, `context`, `device`, `playback`, `playlist`, 
	`search_results`, `track`, `user`.
* Added `spotify_api` and player API.
* Added function to get part of vector (`vector::sub`).
* Added utilities for loading (`json::load_json`) and saving (`json::save_json`) JSON.
* Reworked `cache`.

### v0.3 (spotify-qt v3.5)
* Added xor encryption/decryption support.
* Added time and size formatting.
* Added vector utilities.
* All headers are now in an "include" folder.
* `general.song_header_resize_mode` is now `general.track_list_resize_mode`.
* Removed `general.spotify_playback_order` setting.

### v0.2 (spotify-qt v3.4)
* Added settings.
* Added formatting.
* Added logging (`log`, `log_message`).
* Added `date_time`.

### v0.1 (spotify-qt v3.3)
* Initial version.