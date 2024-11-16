#pragma once

#include "lib/enum/clienttype.hpp"

#include <QStringList>
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
		static auto clientType(const QString &path) -> lib::client_type;
		static auto version(const QString &path) -> QString;
		static auto running(const QString &path) -> bool;

		static auto processErrorToString(QProcess::ProcessError error);

		/**
		 * If the client supports OAuth authentication
		 * @param path Path to client
		 */
		static auto getOAuthSupport(const QString &path) -> bool;

	private:
		Helper() = default;

		static auto clientExec(const QString &path, const QStringList &arguments) -> QString;

		static auto getSpotifydPossibleValues(const QString &path,
			const QString &type) -> QStringList;
	};
}
