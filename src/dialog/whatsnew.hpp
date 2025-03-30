#pragma once

#include "lib/settings.hpp"
#include "lib/github/api.hpp"

#include <QDialog>
#include <QTextEdit>

namespace Dialog
{
	class WhatsNew: public QDialog
	{
	Q_OBJECT

	public:
		WhatsNew(lib::settings &settings,
			const lib::http_client &httpClient, QWidget *parent);

	protected:
		void showEvent(QShowEvent *event) override;

	private:
		lib::settings &settings;
		const lib::http_client &httpClient;
		QTextEdit *text = nullptr;

		void onReleaseInfo(const lib::gh::release &release);
		void onDontShowAgain(bool checked);
		void onOk(bool checked);
	};
}
