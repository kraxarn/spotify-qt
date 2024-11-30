#pragma once

/**
 * Type of item shown in the side panel as a tab
 */
enum class SidePanelType: char
{
	/**
	 * Artist, paired with artist ID
	 */
	Artist,

	/**
	 * Search, not paired
	 */
	Search,

	/**
	 * Lyrics, paired with track ID
	 */
	Lyrics,
};
