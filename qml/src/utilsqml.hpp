#pragma once

#include "common.hpp"

#include <QClipboard>

class UtilsQml : public QObject
{
	Q_OBJECT
	QML_ELEMENT

public:
	explicit UtilsQml(QObject *parent = nullptr);

	Q_INVOKABLE QString formatTime(int ms);
	Q_INVOKABLE void copyToClipboard(const QString &text);
	Q_INVOKABLE QString sptAuthUrl(const QString &clientId);
	Q_INVOKABLE QJsonObject extractUrlQuery(const QUrl &url);
};


