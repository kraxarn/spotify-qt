#pragma once

#include "lib/spotify/api.hpp"
#include "spotify/current.hpp"

#include <QLabel>

namespace Context
{
	class TitleInfo: public QLabel
	{
	public:
		TitleInfo(lib::spt::api &spotify, QWidget *parent);

		auto getIcon() const -> QIcon;

	protected:
		void mouseReleaseEvent(QMouseEvent *event) override;

	private:
		lib::spt::api &spotify;
		lib::spt::playback playback;

		void onContextMenu(const QPoint &pos);
		void onContextMenuTriggered(bool checked);
		void onPlaybackRefreshed(const lib::spt::playback &refreshed,
			const lib::spt::playback &previous);
	};
};
