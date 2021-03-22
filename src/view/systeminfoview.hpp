#pragma once

#include "../mainwindow.hpp"
#include "../spotify/spotify.hpp"
#include "logview.hpp"

#include <QFile>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>

class SystemInfoView: public QWidget
{
Q_OBJECT

public:
	explicit SystemInfoView(QWidget *mainWindow, QWidget *parent = nullptr);
	static QString systemInfo(const lib::spt::playback &playback, bool html = true);

private:
	QString systemInfo(bool html = true);
	void copyToClipboard(bool checked);

	QWidget *mainWindow = nullptr;
};
