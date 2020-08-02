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