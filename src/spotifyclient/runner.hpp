#pragma once

#include "lib/enum/clienttype.hpp"
#include "lib/settings.hpp"
#include "lib/logmessage.hpp"

#include "spotifyclient/helper.hpp"

#include <QDateTime>
#include <QFileInfo>
#include <QInputDialog>
#include <QList>
#include <QProcess>
#include <QRegularExpression>
#include <QStringList>

namespace SpotifyClient
{
	/**
	 * Manages any running spotify client instance
	 */
	class Runner: public QObject
	{
	Q_OBJECT

	public:
		Runner(const lib::settings &settings, const lib::paths &paths, QWidget *parent);
		~Runner() override;

		void start();
		void start(const QString &username, const QString &password);

		static auto getLog() -> const std::vector<lib::log_message> &;
		auto isRunning() const -> bool;

	signals:
		/**
		 * Client status changed, usually after attempted start
		 * @param status Error message, or empty string on success
		 */
		void statusChanged(const QString &status);

	private:
		QProcess *process = nullptr;
		QWidget *parentWidget = nullptr;
		QString path;
		static std::vector<lib::log_message> log;
		const lib::settings &settings;
		const lib::paths &paths;
		lib::client_type clientType;

		auto getUsername() const -> QString;

		void logOutput(const QByteArray &output, lib::log_type logType);
		static auto joinArgs(const QStringList &args) -> QString;

		void onReadyReadOutput();
		void onReadyReadError();
		void onStarted();
		void onErrorOccurred(QProcess::ProcessError error);
	};
}
