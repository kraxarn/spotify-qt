#pragma once

#include "spotify/spotify.hpp"

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QDebug>
#include <QUrl>
#include <QLineEdit>
#include <QDesktopServices>
#include <QPushButton>
#include <QCoreApplication>
#include <QWebEngineView>
#include <QMessageBox>

class SetupDialog : public QDialog
{
public:
	explicit SetupDialog(spt::Spotify *spt, QWidget *parent = nullptr);
	int exec() override;
};