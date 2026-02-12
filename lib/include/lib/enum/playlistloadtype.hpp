#pragma once

#include <QtTypes>

enum class PlaylistLoadType: quint8
{
	/**
	 * Unknown method,
	 * probably loaded from cache
	 */
	Unknown,

	/**
	 * Deprecated method,
	 * tracks can always be loaded
	 */
	Version1,

	/**
	 * Current method,
	 * tracks can only be loaded by the playlist owner
	 */
	Version2,
};
