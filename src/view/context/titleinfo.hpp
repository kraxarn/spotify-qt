#pragma once

#include "lib/spotify/api.hpp"
#include "spotify/current.hpp"

#include <QLabel>

namespace Context
{
	class TitleInfo: public QLabel
	{
	public:
		TitleInfo(lib::spt::api &spotify, spt::Current &current, QWidget *parent);

		auto getIcon() const -> QIcon;

	protected:
		void mouseReleaseEvent(QMouseEvent *event) override;

	private:
		lib::spt::api &spotify;
		spt::Current &current;

		void onContextMenu(const QPoint &pos);
		void onContextMenuTriggered(bool checked);
	};
};
