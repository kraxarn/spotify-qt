#pragma once

#include "common/spotify/auth.hpp"
#include "lib/settings.hpp"

#include <QObject>

namespace spt
{
	class Auth: public QObject, public cmn::spt::Auth
	{
	Q_OBJECT

	public:
		explicit Auth(lib::settings &settings);

		auto auth(const QString &code, const QString &redirect,
			const QString &id, const QString &secret) -> QString;

	private:
		lib::settings &settings;
	};
}
