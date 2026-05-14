#pragma once

enum struct SpotifyAppVersion: unsigned char
{
	Unknown = 0,

	/**
	 * "Granted quota extension" in the dashboard
	 */
	Version1 = 1,

	/**
	 * "Development mode" in the dashboard,
	 * created before 2026-02-11
	 */
	Version2 = 2,

	/**
	 * "Development mode" in the dashboard,
	 * created after/on 2026-02-11
	 */
	Version3 = 3,
};
