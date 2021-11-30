#pragma once

#include "settingspage/base.hpp"
#include "util/icon.hpp"

#include <QComboBox>
#include <QListWidget>

namespace SettingsPage
{
	class Playlists: public Base
	{
	public:
		Playlists(lib::settings &settings, QWidget *parent);

		auto icon() -> QIcon override;
		auto title() -> QString override;
		auto save() -> bool override;

	private:
		QComboBox *plOrder = nullptr;
		QHBoxLayout *plListLayout = nullptr;
		QLabel *plHint = nullptr;
		QListWidget *plList = nullptr;
		QStringList plHints;
		QAction *plBtnUp = nullptr;
		QAction *plBtnDown = nullptr;

		void playlistOrderChanged(int index);
		void playlistMove(int steps);
		void playlistUp();
		void playlistDown();
		void playlistItemChanged(int row);

		auto order() -> QWidget *;
	};
}
