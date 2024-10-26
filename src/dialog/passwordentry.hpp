#pragma once

#include "dialog/base.hpp"
#include "spotifyclient/runner.hpp"

#include <QGroupBox>
#include <QLineEdit>
#include <QCheckBox>

namespace Dialog
{
	class PasswordEntry: public Base
	{
	Q_OBJECT

	public:
		PasswordEntry(SpotifyClient::Runner *runner, QWidget *parent);

		void open(const QString &username);

	protected:
		void onOk(bool checked) override;
		void onCancel(bool checked) override;

	private:
		SpotifyClient::Runner *runner;
		QString currentUsername;

		QGroupBox *container;
		QLineEdit *password;
	};
}
