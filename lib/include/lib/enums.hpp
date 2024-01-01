#pragma once

#include "lib/enum/mediatype.hpp"
#include "lib/enum/devicetype.hpp"
#include "lib/enum/playeraction.hpp"
#include "lib/enum/albumshape.hpp"

#include <string>

namespace lib
{
	/**
	 * Enum parser
	 * @tparam T Enum
	 */
	template<typename T>
	class enums
	{
	public:
		static auto to_string(T value) -> std::string
		{
			std::string str;
			enum_to_string(value, str);
			return str;
		}

		static auto parse(const std::string &str) -> T
		{
			T value;
			enum_from_string(str, value);
			return value;
		}

		/**
		 * If value has flag, eg. "value & flag == flag"
		 * @param value Enum value
		 * @param flag Enum flag to check value for
		 * @return Value contains flag
		 */
		static auto has_flag(T value, T flag) -> bool
		{
			const auto valueFlag = static_cast<unsigned int>(value);
			const auto flagFlag = static_cast<unsigned int>(flag);
			return (valueFlag & flagFlag) == flagFlag;
		}

	private:
		// region media_type

		static void enum_to_string(media_type media_type, std::string &str)
		{
			switch (media_type)
			{
				case media_type::unknown:
					str = "unknown";
					break;

				case media_type::audio:
					str = "audio";
					break;
			}
		}

		static void enum_from_string(const std::string &str, media_type &media_type)
		{
			if (str == "audio")
			{
				media_type = media_type::audio;
				return;
			}

			media_type = media_type::unknown;
		}

		//endregion

		//region device_type

		static void enum_to_string(device_type device_type, std::string &str)
		{
			switch (device_type)
			{
				case device_type::unknown:
					str = "unknown";
					break;

				case device_type::computer:
					str = "computer";
					break;

				case device_type::tablet:
					str = "tablet";
					break;

				case device_type::smartphone:
					str = "smartphone";
					break;

				case device_type::speaker:
					str = "speaker";
					break;

				case device_type::tv:
					str = "tv";
					break;

				case device_type::avr:
					str = "avr";
					break;

				case device_type::stb:
					str = "stb";
					break;

				case device_type::audio_dongle:
					str = "audio dongle";
					break;

				case device_type::game_console:
					str = "game console";
					break;

				case device_type::cast_audio:
					str = "cast audio";
					break;

				case device_type::cast_video:
					str = "cast video";
					break;

				case device_type::automobile:
					str = "automobile";
					break;

				case device_type::smartwatch:
					str = "smartwatch";
					break;

				case device_type::chromebook:
					str = "chromebook";
					break;

				case device_type::unknown_spotify:
					str = "unknown spotify";
					break;

				case device_type::car_thing:
					str = "car thing";
					break;

				case device_type::observer:
					str = "observer";
					break;

				case device_type::home_thing:
					str = "home thing";
					break;
			}
		}

		//endregion

		//region player_action

		static void enum_to_string(player_action player_action, std::string &str)
		{
			switch (player_action)
			{
				case player_action::interrupting_playback:
					str = "interrupting_playback";
					break;

				case player_action::pausing:
					str = "pausing";
					break;

				case player_action::resuming:
					str = "resuming";
					break;

				case player_action::seeking:
					str = "seeking";
					break;

				case player_action::skipping_next:
					str = "skipping_next";
					break;

				case player_action::skipping_prev:
					str = "skipping_prev";
					break;

				case player_action::toggling_repeat_context:
					str = "toggling_repeat_context";
					break;

				case player_action::toggling_shuffle:
					str = "toggling_shuffle";
					break;

				case player_action::toggling_repeat_track:
					str = "toggling_repeat_track";
					break;

				case player_action::transferring_playback:
					str = "transferring_playback";
					break;

				case player_action::unknown:
					str = "unknown";
					break;
			}
		}

		static void enum_from_string(const std::string &str, player_action &player_action)
		{
			if (str == "interrupting_playback")
			{
				player_action = player_action::interrupting_playback;
			}
			else if (str == "pausing")
			{
				player_action = player_action::pausing;
			}
			else if (str == "resuming")
			{
				player_action = player_action::resuming;
			}
			else if (str == "seeking")
			{
				player_action = player_action::seeking;
			}
			else if (str == "skipping_next")
			{
				player_action = player_action::skipping_next;
			}
			else if (str == "skipping_prev")
			{
				player_action = player_action::skipping_prev;
			}
			else if (str == "toggling_repeat_context")
			{
				player_action = player_action::toggling_repeat_context;
			}
			else if (str == "toggling_shuffle")
			{
				player_action = player_action::toggling_shuffle;
			}
			else if (str == "toggling_repeat_track")
			{
				player_action = player_action::toggling_repeat_track;
			}
			else if (str == "transferring_playback")
			{
				player_action = player_action::transferring_playback;
			}
			else
			{
				player_action = player_action::unknown;
			}
		}

		//endregion

		//region album_shape

		static void enum_to_string(album_shape album_shape, std::string &str)
		{
			switch (album_shape)
			{
				case album_shape::none:
					str = "none";
					break;

				case album_shape::app:
					str = "app";
					break;

				case album_shape::circle:
					str = "circle";
					break;

				case album_shape::disc:
					str = "disc";
					break;
			}
		}

		//endregion
	};
}
