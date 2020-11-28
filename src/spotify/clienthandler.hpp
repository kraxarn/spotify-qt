#pragma once

#include "../keyring/kwallet.hpp"
#include "lib/enum/clienttype.hpp"
#include "lib/settings.hpp"

#include <QDateTime>
#include <QFileInfo>
#include <QInputDialog>
#include <QList>
#include <QProcess>
#include <QStringList>

namespace spt
{
	class ClientHandler: public QObject
	{
	Q_OBJECT

	public:
		explicit ClientHandler(const lib::Settings &settings, QWidget *parent = nullptr);
		~ClientHandler() override;

		QString start();
		QStringList availableBackends();
		bool isRunning();
		static QString version(const QString &path);
		static QList<QPair<QDateTime, QString>> getLog();
		static float getVolume();
		static void setVolume(float value);

		QProcess *process = nullptr;

	private:
		QWidget *parentWidget = nullptr;
		QString path;
		static QList<QPair<QDateTime, QString>> log;
		const lib::Settings &settings;
		lib::ClientType clientType;

		bool supportsPulse();
		static QString clientExec(const QString &path, const QStringList &arguments);
		static QString getSinkInfo();
		void readyRead() const;
		void readyError() const;
		void logOutput(const QByteArray &output) const;
		static ClientType getClientType(const QString &path);
	};
}