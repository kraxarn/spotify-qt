#pragma once

#include "lib/settings.hpp"
#include "lib/httpclient.hpp"

#include <QDialog>
#include <QVBoxLayout>
#include <QTextEdit>

class WhatsNewDialog: public QDialog
{
Q_OBJECT

public:
	WhatsNewDialog(lib::settings &settings,
		const lib::http_client &httpClient, QWidget *parent);

	/** Show latest changes if fetch was successful */
	void open() override;

private:
	lib::settings &settings;
	const lib::http_client &httpClient;
	QTextEdit *text = nullptr;

	static void failed(const std::string &reason);

	void onReleaseInfo(const nlohmann::json &json);
	void onDontShowAgain(bool checked);
	void onOk(bool checked);
};
