#pragma once

#include "lib/enum/clienttype.hpp"

#include <QStringList>
#include <QFileInfo>
#include <QProcess>

namespace spt
{
	class ClientHelper
	{
	public:
		static auto availableBackends(const QString &path) -> QStringList;
		static auto getClientType(const QString &path) -> lib::client_type;
		static auto supportsPulse(const QString &path) -> bool;
		static auto version(const QString &path) -> QString;
		static auto isRunning(const QString &path) -> bool;
		static auto getSinkInfo() -> QString;
		static auto getVolume() -> float;
		static void setVolume(float value);

	private:
		ClientHelper() = default;

		static auto clientExec(const QString &path, const QStringList &arguments) -> QString;
	};
}
