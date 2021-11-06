#pragma once

/**
 * Actions possible to perform from desktop notification
 */
enum class NotificationAction
{
	/**
	 * Default notification click, highlight application
	 */
	Default,

	/**
	 * Go to previous track
	 */
	Previous,

	/**
	 * Play, or pause, current track
	 */
	PlayPause,

	/**
	 * Go to next track
	 */
	Next,
};
