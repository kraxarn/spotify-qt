#pragma once

#include <QKeySequence>

/**
 * Available shortcuts.
 * More info: https://support.spotify.com/us/article/keyboard-shortcuts
 */
class Shortcut
{
public:
	static auto newPlaylist() -> QKeySequence;
	static auto deleteTrack() -> QKeySequence;

	static auto playPause() -> QKeySequence;
	static auto repeat() -> QKeySequence;
	static auto shuffle() -> QKeySequence;
	static auto nextTrack() -> QKeySequence;
	static auto previousTrack() -> QKeySequence;
	static auto volumeUp() -> QKeySequence;
	static auto volumeDown() -> QKeySequence;

	static auto playSelectedRow() -> QKeySequence;
	static auto logOut() -> QKeySequence;

private:
	Shortcut() = default;
};
