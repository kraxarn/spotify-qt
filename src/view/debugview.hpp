#pragma once

#include "../spotify/spotify.hpp"

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

class DebugView: public QWidget
{
Q_OBJECT

public:
	explicit DebugView(const lib::settings &settings, QWidget *parent);

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