#pragma once

#include "../mainwindow.hpp"
#include "logviewer.hpp"
#include "spotify/spotify.hpp"

#include <QDialog>
#include <QDialogButtonBox>
#include <QFile>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>

class SystemInfoDialog: public QDialog
{
Q_OBJECT

public:
	explicit SystemInfoDialog(QWidget *mainWindow, QWidget *parent = nullptr);
	static QString systemInfo(const spt::Playback &playback, bool html = true);

private:
	QString systemInfo(bool html = true);
	void copyToClipboard(bool checked);

	QWidget *mainWindow = nullptr;
};
