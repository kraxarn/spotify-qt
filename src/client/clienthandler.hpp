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
		ClientHandler(const lib::settings &settings,
			const lib::paths &paths, QWidget *parent);
		~ClientHandler() override;

		auto start() -> QString;

		auto waitForStarted() const -> bool;

		/**
		 * Wrapper for spt::ClientHelper::availableBackends
		 */
		auto availableBackends() -> QStringList;

		static auto getLog() -> const QList<QPair<QDateTime, QString>> &;

		auto isRunning() const -> bool;

	private:
		QProcess *process = nullptr;
		QWidget *parentWidget = nullptr;
		QString path;
		static QList<QPair<QDateTime, QString>> log;
		const lib::settings &settings;
		const lib::paths &paths;
		lib::client_type clientType;

		/**
		 * Wrapper for spt::ClientHelper::supportsPulse
		 */
		auto supportsPulse() -> bool;

		void readyRead() const;

		void readyError() const;

		static void logOutput(const QByteArray &output);
	};
}
