#pragma once

#include "lib/spotify/device.hpp"

namespace lib
{
	namespace spt
	{
		/**
		 * Get absolute API url from relative URL
		 * @param relative_url Relative URL
		 * @return Absolute URL
		 */
		auto to_full_url(const std::string &relative_url) -> std::string;

		/**
		 * Get relative URL from absolute URL
		 * @param full_url Full URL
		 * @return Relative URL
		 */
		auto to_relative_url(const std::string &full_url) -> std::string;

		/**
		 * Spotify ID (4uLU6hMCjMI75M1A2tKUQC) to Spotify URI
		 * (spotify:track:4uLU6hMCjMI75M1A2tKUQC)
		 * @param type URI type, for example artist, album, track, etc.
		 * @param spotify_id Spotify ID
		 * @note Returns result if already an URI
		 * @return Spotify URI
		 */
		auto id_to_uri(const std::string &type, const std::string &spotify_id) -> std::string;

		/**
		 * Spotify URI (spotify:track:4uLU6hMCjMI75M1A2tKUQC) to Spotify ID
		 * (4uLU6hMCjMI75M1A2tKUQC)
		 * @param spotify_uri Spotify URI
		 * @note Returns result if already an URI
		 * @return Spotify ID
		 */
		auto uri_to_id(const std::string &spotify_uri) -> std::string;

		/**
		 * Spotify URL (https://open.spotify.com/track/4uLU6hMCjMI75M1A2tKUQC)
		 * to Spotify URI (spotify:track:4uLU6hMCjMI75M1A2tKUQC)
		 * @param url Spotify URL
		 * @return Spotify URI or an empty string if Spotify URL is invalid
		 */
		auto url_to_uri(const std::string &url) -> std::string;

		/**
		 * Get new URL with device ID added, or replaced
		 * @param url URL to modify
		 * @param device Device to add/replace
		 */
		auto get_device_url(const std::string &url, const lib::spt::device &device) -> std::string;
	}
}
