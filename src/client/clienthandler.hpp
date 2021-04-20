#pragma once

#include "lib/enum/clienttype.hpp"
#include "../keyring/kwallet.hpp"
#include "lib/settings.hpp"
#include "clienthelper.hpp"

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
		explicit ClientHandler(const lib::settings &settings, QWidget *parent = nullptr);
		~ClientHandler() override;

		auto start() -> QString;

		/**
		 * Wrapper for spt::ClientHelper::availableBackends
		 */
		auto availableBackends() -> QStringList;

		static auto getLog() -> const QList<QPair<QDateTime, QString>> &;

		QProcess *process = nullptr;

	private:
		QWidget *parentWidget = nullptr;
		QString path;
		static QList<QPair<QDateTime, QString>> log;
		const lib::settings &settings;
		lib::client_type clientType;

		/**
		 * Wrapper for spt::ClientHelper::supportsPulse
		 */
		auto supportsPulse() -> bool;

		auto isRunning() const -> bool;

		void readyRead() const;

		void readyError() const;

		static void logOutput(const QByteArray &output) ;
	};
}