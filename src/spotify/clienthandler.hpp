#pragma once

#include "src/settings.hpp"

#include <QDebug>
#include <QFileInfo>
#include <QInputDialog>
#include <QProcess>

namespace spt
{
	class ClientHandler : public QObject
	{
	Q_OBJECT

	public:
		explicit ClientHandler(const Settings &settings, QWidget *parent = nullptr);

		static QString version(const QString &path);
		QString start();
		QProcess *process = nullptr;
		bool isRunning();
		static float getVolume();
		static void setVolume(float value);

	private:
		QWidget* parentWidget = nullptr;
		QString path;
		const Settings &settings;
		bool supportsPulse();
		static QString clientExec(const QString &path, const QStringList &arguments);
		static QString getSinkInfo();
	};
}