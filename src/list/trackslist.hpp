#pragma once

#include "../mainwindow.hpp"
#include "../spotify/spotify.hpp"

#include <QListWidget>

class TracksList: public QTreeWidget
{
Q_OBJECT

public:
	TracksList(spt::Spotify &spotify, lib::settings &settings, QWidget *parent);

	void updateResizeMode(lib::resize_mode mode);

protected:
	void resizeEvent(QResizeEvent *event) override;

private:
	void menu(const QPoint &pos);
	void clicked(QTreeWidgetItem *item, int column);
	void headerMenu(const QPoint &pos);

	void resizeHeaders(const QSize &newSize);

	spt::Spotify &spotify;
	lib::settings &settings;
};