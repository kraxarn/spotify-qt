#include "utilsqml.hpp"

UtilsQml::UtilsQml(QObject *parent)
	: QObject(parent)
{
}

QString UtilsQml::formatTime(int ms)
{
	return Utils::formatTime(ms);
}

void UtilsQml::copyToClipboard(const QString &text)
{
	QGuiApplication::clipboard()->setText(text);
}

QString UtilsQml::sptAuthUrl(const QString &clientId)
{
	return spt::Auth::authUrl(clientId, "http://localhost:8888");
}

QJsonObject UtilsQml::extractUrlQuery(const QUrl &url)
{
	QJsonObject obj;
	for (auto &item : QUrlQuery(url).queryItems())
	{
		obj[item.first] = item.second;
	}
	return obj;
}