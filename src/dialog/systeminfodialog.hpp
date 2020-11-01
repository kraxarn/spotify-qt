#pragma once

#include "../mainwindow.hpp"
#include "../spotify/spotify.hpp"
#include "logviewer.hpp"

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
	static QString systemInfo(const spt::Playback &playback);

private:
	QString systemInfo();

	QWidget *mainWindow = nullptr;
};
