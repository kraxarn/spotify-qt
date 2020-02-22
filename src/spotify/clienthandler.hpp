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
		explicit ClientHandler(QWidget *parent = nullptr);
		virtual ~ClientHandler();
		QString start();
		QProcess *process = nullptr;
	private:
		QWidget *parent;
	};
}