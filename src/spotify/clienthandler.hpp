#pragma once

#include "src/settings.hpp"

#include <QDebug>
#include <QFileInfo>
#include <QInputDialog>
#include <QProcess>
#include <QStringList>
#include <QList>

namespace spt
{
	class ClientHandler : public QObject
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
		QWidget* parentWidget = nullptr;
		QString path;
		static QStringList log;
		const Settings &settings;
		bool supportsPulse();
		static QString clientExec(const QString &path, const QStringList &arguments);
		static QString getSinkInfo();
		void readyRead();
	};
}