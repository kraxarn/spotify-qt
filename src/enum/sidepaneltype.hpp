#pragma once

/**
 * Type of item shown in the side panel as a tab
 */
enum class SidePanelType: char
{
	/**
	 * Other, or unknown, type
	 */
	None,

	/**
	 * Artist, paired with artist ID
	 */
	Artist,

	/**
	 * Search, not paired
	 */
	Search,

	/**
	 * Audio features, paired with track ID
	 */
	AudioFeatures,

	/**
	 * Lyrics, paired with track ID
	 */
	Lyrics,
};
