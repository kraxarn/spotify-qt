#pragma once

#include "dialog/base.hpp"
#include "spotify/authserver.hpp"

#include <QLineEdit>

namespace Dialog
{
	class Setup: public Base
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
