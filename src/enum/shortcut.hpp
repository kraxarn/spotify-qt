#pragma once

#include <QKeySequence>

/**
 * Available shortcuts.
 * More info: https://support.spotify.com/us/article/keyboard-shortcuts
 */
enum class Shortcut: int
{
	NewPlaylist = Qt::CTRL + Qt::Key_N,
	Delete = Qt::Key_Delete,

	PlayPause = Qt::Key_Space,
	Repeat = Qt::CTRL + Qt::Key_R,
	Shuffle = Qt::CTRL + Qt::Key_S,
	NextTrack = Qt::CTRL + Qt::Key_Right,
	PreviousTrack = Qt::CTRL + Qt::Key_Left,
	VolumeUp = Qt::CTRL + Qt::Key_Up,
	VolumeDown = Qt::CTRL + Qt::Key_Down,

	PlaySelectedRow = Qt::Key_Return,
	LogOut = Qt::CTRL + Qt::SHIFT + Qt::Key_W,
};
