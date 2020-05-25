#pragma once

#include "src/settings.hpp"

#include <QProcess>
#include <QFileInfo>
#include <QInputDialog>
#include <QDebug>

namespace spt
{
	class ClientHandler
	{
	public:
		explicit ClientHandler(const Settings &settings, QWidget *parent = nullptr);
		virtual ~ClientHandler();
		static QString version(const QString &path);
		QString start();
		QProcess *process = nullptr;
		bool isRunning();
	private:
		QWidget *parent;
		QString path;
		Settings settings;
		bool supportsPulse();
		static QString clientExec(const QString &path, const QStringList &arguments);
	};
}