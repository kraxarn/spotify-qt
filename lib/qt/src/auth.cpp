#include "common/spotify/auth.hpp"

cmn::spt::Auth::Auth(lib::settings &settings, QObject *parent)
	: QObject(parent)
{
	auto *httpClient = new cmn::QtHttpClient(this);
	sptAuth = new lib::spt::auth(settings, *httpClient);
}

auto cmn::spt::Auth::authUrl(const QString &clientId, const QString &redirect) -> QString
{
	return QString::fromStdString(lib::spt::auth::url(clientId.toStdString(),
		redirect.toStdString()));
}

auto cmn::spt::Auth::auth(const QString &code, const QString &redirect,
	const QString &id, const QString &secret) -> QString
{
	return QString::fromStdString(sptAuth->get(code.toStdString(),
		redirect.toStdString(), id.toStdString(), secret.toStdString()));
}
