#pragma once

#include "lib/qt/spotify/auth.hpp"
#include "dialog/openlink.hpp"
#include "util/url.hpp"

#include <QCoreApplication>
#include <QDesktopServices>
#include <QDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QTcpSocket>
#include <QTcpServer>
#include <QUrl>
#include <QVBoxLayout>

namespace Dialog
{
	class Setup: public QDialog
	{
	Q_OBJECT

	public:
		Setup(lib::settings &settings, QWidget *parent);
		~Setup() override;

	private:
		lib::qt::spt::auth *auth;
		QTcpServer *server;
		QLineEdit *clientId;
		QLineEdit *clientSecret;
		QString clientIdText;
		QString clientSecretText;
		QString redirect;
		lib::settings &settings;

		void onCancel(bool checked);
		void onOpenDashboard(bool checked);
		void onAuthenticate(bool checked);
		void onNewConnection();
	};
}
