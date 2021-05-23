#include "lib/qt/spotify/auth.hpp"

lib::qt::spt::auth::auth(lib::settings &settings, QObject *parent)
	: QObject(parent)
{
	auto *httpClient = new lib::qt::http_client(this);
	spt_auth = new lib::spt::auth(settings, *httpClient);
}

lib::qt::spt::auth::~auth()
{
	delete spt_auth;
}

auto lib::qt::spt::auth::url(const QString &clientId, const QString &redirect) -> QString
{
	return QString::fromStdString(lib::spt::auth::url(clientId.toStdString(),
		redirect.toStdString()));
}

auto lib::qt::spt::auth::get(const QString &code, const QString &redirect,
	const QString &id, const QString &secret) -> QString
{
	return QString::fromStdString(spt_auth->get(code.toStdString(),
		redirect.toStdString(), id.toStdString(), secret.toStdString()));
}
