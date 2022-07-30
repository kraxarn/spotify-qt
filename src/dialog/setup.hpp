#pragma once

#include "spotify/authserver.hpp"
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

	private:
		spt::AuthServer *auth = nullptr;
		QLineEdit *clientId = nullptr;
		QLineEdit *clientSecret = nullptr;
		lib::settings &settings;

		void onOpenDashboard(bool checked);
		void onAuthenticate(bool checked);
	};
}
