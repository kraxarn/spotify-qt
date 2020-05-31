#pragma once

#include "spotify/auth.hpp"

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
#include <QMessageBox>
#include <QTcpServer>

class SetupDialog : public QDialog
{
	Q_OBJECT

public:
	SetupDialog(QWidget *parent = nullptr);
	~SetupDialog() override;

private:
	spt::Auth	*auth;
	QTcpServer	*server;
	QLineEdit	*clientId, *clientSecret;
	QString		clientIdText, clientSecretText, redirect;
};