#pragma once

/**
 * How the application was built/installed
 */
enum class InstallType
{
	/**
	 * No known path
	 */
	Unknown,

	/**
	 * Binary is in home directory
	 */
	User,

	/**
	 * Binary is in system directory
	 */
	System,

	/**
	 * Binary is installed as a snap
	 */
	Snap,
};
