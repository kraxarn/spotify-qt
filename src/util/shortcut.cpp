#include "util/shortcut.hpp"

auto Shortcut::newPlaylist() -> QKeySequence
{
	return {Qt::CTRL | Qt::Key_N};
}

auto Shortcut::deleteTrack() -> QKeySequence
{
	return {Qt::Key_Delete};
}

auto Shortcut::playPause() -> QKeySequence
{
	return {Qt::Key_Space};
}

auto Shortcut::repeat() -> QKeySequence
{
	return {Qt::CTRL | Qt::Key_R};
}

auto Shortcut::shuffle() -> QKeySequence
{
	return {Qt::CTRL | Qt::Key_S};
}

auto Shortcut::nextTrack() -> QKeySequence
{
	return {Qt::CTRL | Qt::Key_Right};
}

auto Shortcut::previousTrack() -> QKeySequence
{
	return {Qt::CTRL | Qt::Key_Left};
}

auto Shortcut::volumeUp() -> QKeySequence
{
	return {Qt::CTRL | Qt::Key_Up};
}

auto Shortcut::volumeDown() -> QKeySequence
{
	return {Qt::CTRL | Qt::Key_Down};
}

auto Shortcut::playSelectedRow() -> QKeySequence
{
	return {Qt::Key_Return};
}

auto Shortcut::logOut() -> QKeySequence
{
	return {Qt::CTRL | Qt::SHIFT | Qt::Key_W};
}
