#pragma once

#include "lib/qt/spotify/auth.hpp"
#include "dialog/openlink.hpp"
#include "util/urlutils.hpp"

#include <QCoreApplication>
#include <QDesktopServices>
#include <QDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QTcpServer>
#include <QUrl>
#include <QVBoxLayout>

class SetupDialog: public QDialog
{
Q_OBJECT

public:
	explicit SetupDialog(lib::settings &settings, QWidget *parent = nullptr);
	~SetupDialog() override;

private:
	lib::qt::spt::auth *auth;
	QTcpServer *server;
	QLineEdit *clientId, *clientSecret;
	QString clientIdText, clientSecretText, redirect;
	lib::settings &settings;

	void cancel(bool checked);
	void spotifyDashboard(bool checked);
	void authenticate(bool checked);
	void newServerConnection();
};
