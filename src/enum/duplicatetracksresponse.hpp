#pragma once

enum class DuplicateTrackResponse
{
	/**
	 * Cancel action
	 */
	None,

	/**
	 * Add tracks not already in destination
	 */
	AddMissing,

	/**
	 * Add all tracks
	 */
	AddAll,
};
