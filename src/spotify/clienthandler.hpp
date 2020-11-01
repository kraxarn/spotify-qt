#pragma once

#include "../enum/clienttype.hpp"
#include "../keyring/kwallet.hpp"
#include "../settings.hpp"

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
		explicit ClientHandler(const Settings &settings, QWidget *parent = nullptr);
		~ClientHandler() override;

		static QString version(const QString &path);
		QString start();
		QProcess *process = nullptr;
		bool isRunning();
		static float getVolume();
		static void setVolume(float value);
		static QStringList getLog();

	private:
		QWidget *parentWidget = nullptr;
		QString path;
		static QStringList log;
		const Settings &settings;
		ClientType clientType;

		bool supportsPulse();
		static QString clientExec(const QString &path, const QStringList &arguments);
		static QString getSinkInfo();
		void readyRead() const;
	};
}