#include "spotifyclient/helper.hpp"

auto SpotifyClient::Helper::clientExec(const QString &path, const QStringList &arguments) -> QString
{
	// Check if it exists
	const QFileInfo file(path);
	if (!file.exists())
	{
		return {};
	}

	// Prepare process
	QProcess process;

	// Get version info
	process.start(file.absoluteFilePath(), arguments);
	process.waitForFinished();

	// Entire stdout is version
	return process.readAllStandardOutput().trimmed();
}

auto SpotifyClient::Helper::availableBackends(const QString &path) -> QStringList
{
	QStringList items;

	const QString result = clientExec(path, {
		QStringLiteral("--name"), QString(),
		QStringLiteral("--backend"), QStringLiteral("?"),
	});

	for (const QString &line : result.split('\n'))
	{
		if (!line.startsWith("-"))
		{
			continue;
		}

		items.append(line.right(line.length() - 2)
			.remove(QStringLiteral("(default)"))
			.trimmed());
	}

	return items;
}

auto SpotifyClient::Helper::version(const QString &path) -> QString
{
	const QString versionInfo = clientExec(path, {
		QStringLiteral("--version"),
	});

	if (versionInfo.startsWith(QStringLiteral("error:")))
	{
		return QStringLiteral("librespot");
	}

	const qsizetype stop = versionInfo.indexOf(QChar::fromLatin1('('));
	return stop > 0
		? versionInfo.left(stop - 1)
		: QStringLiteral("librespot");
}

auto SpotifyClient::Helper::running(const QString &path) -> bool
{
	if (path.isEmpty() || !QFile("/usr/bin/ps").exists())
	{
		return false;
	}

	QProcess ps;
	ps.start("/usr/bin/ps", {"aux"});
	ps.waitForFinished();
	auto out = ps.readAllStandardOutput();
	return QString(out).contains(path);
}

auto SpotifyClient::Helper::getOAuthSupport(const QString &path) -> bool
{
	const QString help = clientExec(path, {
		QStringLiteral("--help"),
	});

	return help.contains(QStringLiteral("--enable-oauth"));
}

auto SpotifyClient::Helper::processErrorToString(const QProcess::ProcessError error) -> QString
{
	switch (error)
	{
		case QProcess::FailedToStart:
			return QStringLiteral("Process failed to start");

		case QProcess::Crashed:
			return QStringLiteral("Process stopped or crashed");

		case QProcess::Timedout:
			return QStringLiteral("Process timed out");

		case QProcess::WriteError:
			return QStringLiteral("Process write error");

		case QProcess::ReadError:
			return QStringLiteral("Process read error");

		default:
			return QStringLiteral("Unknown error");
	}
}
