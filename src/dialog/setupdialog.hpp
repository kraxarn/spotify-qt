#pragma once

#include "../spotify/auth.hpp"
#include "openlinkdialog.hpp"

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
	spt::Auth *auth;
	QTcpServer *server;
	QLineEdit *clientId, *clientSecret;
	QString clientIdText, clientSecretText, redirect;
	lib::settings &settings;
};