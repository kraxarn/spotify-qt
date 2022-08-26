#pragma once

#include "dialog/base.hpp"
#include "lib/spotify/error.hpp"
#include "spotify/authserver.hpp"

#include <QPushButton>

namespace Dialog
{
	class Auth: public Base
	{
	Q_OBJECT

	public:
		Auth(lib::settings &settings, QWidget *parent);

		void show(const lib::spt::error &error);

	signals:
		void success();
		void failed(const QString &message);

	private:
		static constexpr int margin = 16;

		const lib::settings &settings;

		spt::AuthServer *auth;

		QLabel *errorInfo;
		QPushButton *reauthenticate;

		void onReauthenticate(bool checked);
		void onAuthSuccess();
		void onAuthFailed(const QString &message);
	};
}
