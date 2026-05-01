#pragma once

#include <QFileInfo>
#include <QProcess>

namespace SpotifyClient
{
	/**
	 * Helper functions for interacting with a Spotify client
	 */
	class Helper
	{
	public:
		static auto availableBackends(const QString &path) -> QStringList;
		static auto version(const QString &path) -> QString;
		static auto running(const QString &path) -> bool;

		static auto processErrorToString(QProcess::ProcessError error) -> QString;

		/**
		 * If the client supports OAuth authentication
		 * @param path Path to client
		 */
		static auto getOAuthSupport(const QString &path) -> bool;

	private:
		Helper() = default;

		static auto clientExec(const QString &path, const QStringList &arguments) -> QString;
	};
}
