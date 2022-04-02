#pragma once

namespace lib
{
	/**
	 * librespot device type,
	 * see: https://github.com/librespot-org/librespot/blob/dev/core/src/config.rs
	 */
	enum class device_type: short
	{
		unknown = 0,
		computer = 1,
		tablet = 2,
		smartphone = 3,
		speaker = 4,
		tv = 5,
		avr = 6,
		stb = 7,
		audio_dongle = 8,
		game_console = 9,
		cast_audio = 10,
		cast_video = 11,
		automobile = 12,
		smartwatch = 13,
		chromebook = 14,
		unknown_spotify = 100,
		car_thing = 101,
		observer = 102,
		home_thing = 103,
	};
}
