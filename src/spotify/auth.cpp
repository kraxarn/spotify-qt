#include "auth.hpp"

spt::Auth::Auth(lib::settings &settings)
	: settings(settings),
	cmn::spt::Auth(this)
{
}

auto spt::Auth::auth(const QString &code, const QString &redirect,
	const QString &id, const QString &secret) -> QString
{
	QString accessToken;
	QString refreshToken;
	auto result = cmn::spt::Auth::auth(code, redirect, id, secret,
		accessToken, refreshToken);

	if (!result.isEmpty())
	{
		return result;
	}

	settings.account.access_token = accessToken.toStdString();
	settings.account.refresh_token = refreshToken.toStdString();
	settings.save();

	return QString();
}
