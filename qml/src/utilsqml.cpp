#include "utilsqml.hpp"

#include <QQuickStyle>
#include <QMessageBox>

#define REDIRECT_URL "http://localhost:8888"

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
	return spt::Auth::authUrl(clientId, REDIRECT_URL);
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

QJsonObject UtilsQml::sptAuth(const QString &code, const QString &clientId, const QString &clientSecret)
{
	Settings settings;
	auto status = spt::Auth(settings).auth(code, REDIRECT_URL, clientId, clientSecret);
	return QJsonObject(
		{
			QPair<QString, bool>("success", status.isEmpty()),
			QPair<QString, QString>("message", status)
		}
	);
}

QStringList UtilsQml::availableStyles()
{
	return QQuickStyle::availableStyles();
}

void UtilsQml::aboutQt()
{
	QMessageBox::aboutQt(nullptr);
}