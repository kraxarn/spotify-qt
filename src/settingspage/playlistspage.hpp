#pragma once

#include "settingspage.hpp"
#include "../util/icon.hpp"

#include <QComboBox>
#include <QListWidget>

class PlaylistsPage: public SettingsPage
{
public:
	PlaylistsPage(lib::settings &settings, QWidget *parent);

	QIcon icon() override;
	QString title() override;
	bool save() override;

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

	QWidget *order();
};


