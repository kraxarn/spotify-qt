#pragma once

#include "lib/settings.hpp"

#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QWidget>
#include <QLineEdit>
#include <QNetworkAccessManager>
#include <QTextEdit>
#include <QTabWidget>
#include <QPushButton>
#include <QMessageBox>
#include <QJsonParseError>
#include <QNetworkReply>
#include <QCoreApplication>

namespace Dialog
{
	class ApiRequest: public QDialog
	{
	Q_OBJECT

	public:
		ApiRequest(const lib::settings &settings, QWidget *parent);

	private:
		QComboBox *requestType = nullptr;
		QLineEdit *urlPath = nullptr;
		QNetworkAccessManager *networkManager = nullptr;
		QTextEdit *jsonRequest = nullptr;
		QTextEdit *jsonResponse = nullptr;
		QTabWidget *tabs = nullptr;

		const lib::settings &settings;

		void sendRequest(bool checked);
	};
}
