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
		static auto supportsPulse(const QString &path) -> bool;
		static auto version(const QString &path) -> QString;
		static auto running(const QString &path) -> bool;
		static auto sinkInfo() -> QString;
		static auto getVolume() -> float;
		static void setVolume(float value);

	private:
		Helper() = default;

		static auto clientExec(const QString &path, const QStringList &arguments) -> QString;
	};
}
